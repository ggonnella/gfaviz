#include "gfa/group.h"
#include "gfa/graph.h"

enum GfaGroupResolveStatus {
  GFA_GROUP_UNRESOLVED,
  GFA_GROUP_RESOLVING,
  GFA_GROUP_RESOLVED
};

GfaGroup::GfaGroup() : GfaReferenceableLine(GFA_GROUP) {
  resolve_status = GFA_GROUP_UNRESOLVED;
  members.reserve(2);
}

void GfaGroup::deleteme() {
  delete this;
}

void GfaGroup::fromLine(GfaFileReader* fr) {
  type_original = fr->getLineType();
  if (fr->getLineType() == 'P') {
    setVersion(GFA_V1);
    fr->parseGeneric(this, "lPA", &name, &refs, &alignments, NULL);
  } else if (fr->getLineType() == 'O') {
    setVersion(GFA_V2);
    fr->parseGeneric(this, "lO", &name, &refs, NULL);
  } else if (fr->getLineType() == 'U') {
    setVersion(GFA_V2);
    fr->parseGeneric(this, "lU", &name, &refs, NULL);
  }
  id_set = true;
  is_named = true;
  setFilled();
}

ostream& operator<< (ostream &out, const GfaGroup &g) {
  g.print(out, GFA_VUNKNOWN);
  return out;
}

void GfaGroup::print(ostream &out, GfaVersion _version) const {
  if (getTypeOriginal() == 'P') {
    out << "P\t" << name << '\t';
    for (size_t idx=0; idx < refs.size(); idx++) {
      if (idx>0)
        out << ',';
      refs[idx].print(out,true);
    }
    out << '\t';
    for (size_t idx=0; idx < alignments.size(); idx++) {
      if (idx>0)
        out << ',';
      out << alignments[idx];
      //cout << alignments[idx].cigar;
    }
  } else if (getTypeOriginal() == 'O') {
    out << "O\t" << name << '\t';
    refs[0].print(out,true);
    for (size_t idx=1; idx < refs.size(); idx++) {
      out << " ";
      refs[idx].print(out,true);
    }
  } else if (getTypeOriginal() == 'U') {
    out << "U\t" << name << '\t';
    refs[0].print(out,false);
    for (size_t idx=1; idx < refs.size(); idx++) {
      out << " ";
      refs[idx].print(out,false);
    }
  }
  printTags(out);
  out << endl;
}

char GfaGroup::getTypeOriginal() const {
  return type_original;
}


const vector<GfaLine*>& GfaGroup::getMembers() {
  return members;
}

const GfaRef& GfaGroup::getFirstRef() const {
  return refs[0];
}
const GfaRef& GfaGroup::getLastRef() const{
  return refs.back();
}

void GfaGroup::resolveMembers(size_t idx) {
  if (getTypeOriginal() == 'O' && idx > 0) {
    //find implicit members
    //TODO: Does not check directions yet
    GfaLine* src = refs[idx-1].ptr;
    bool srcrev = refs[idx-1].is_reverse;
    GfaLine* last;    
    GfaLine* tgt = refs[idx].ptr;
    bool tgtrev = refs[idx].is_reverse;
    while (src->getType() == GFA_GROUP) {
      GfaGroup* group = (GfaGroup*)src;
      group->resolve(graph);
      GfaRef ref;
      if (refs[idx-1].is_reverse)
        ref = group->getFirstRef();
      else
        ref = group->getLastRef();
      src = ref.ptr;
      srcrev = ref.is_reverse ^ srcrev;
    }
    while (tgt->getType() == GFA_GROUP) {
      GfaGroup* group = (GfaGroup*)tgt;
      group->resolve(graph);
      GfaRef ref;
      if (refs[idx].is_reverse)
        ref = group->getLastRef();
      else
        ref = group->getFirstRef();
      tgt = ref.ptr;
      tgtrev = ref.is_reverse ^ tgtrev;
    }
    if (src->getType() == GFA_EDGE && tgt->getType() == GFA_EDGE) {
      //find implicit segment
      GfaEdge* edge = (GfaEdge*)src;
      GfaSegment* seg = edge->getSegment((srcrev ? 0 : 1));
      bool reverse = edge->isOutedge(seg);
      if (seg->isInedge((GfaEdge*)tgt) || seg->isOutedge((GfaEdge*)tgt)) {
        members.push_back(seg);
      }
    } else if (src->getType() == GFA_SEGMENT && tgt->getType() == GFA_SEGMENT) {
      //find implicit edge
      GfaSegment* seg = (GfaSegment*)src;
      const vector<GfaEdge*>* edges;
      if (srcrev)
        edges = &(seg->getInedges());
      else
        edges = &(seg->getOutedges());
      for (GfaEdge* edge : *edges) {
        bool reverse = (edge->getSegment(1) == src);
        if (edge->getOppositeSegment(seg) == tgt) {
          members.push_back(edge);
          break;
        }
      }
      
    }
    
    
    
  }
  if (refs[idx].ptr->getType() == GFA_GROUP) {
    const vector<GfaLine*>& gmembers = ((GfaGroup*)refs[idx].ptr)->getMembers();
    members.insert(members.end(), gmembers.begin(), gmembers.end());
  } else {
    members.push_back(refs[idx].ptr);
  }
  
}

void GfaGroup::resolve(GfaGraph* _g) {
  if (resolve_status == GFA_GROUP_RESOLVED) {
    return;
  } if (resolve_status == GFA_GROUP_RESOLVING) {
    throw fatal_error() << "Circular referencing detected in group '" << getName() << "'.";
  }
  resolve_status = GFA_GROUP_RESOLVING;
  for (size_t idx=0; idx < refs.size(); idx++) {
    GfaLine *line = _g->getLine(refs[idx].name);
    if (line == NULL) {
      throw fatal_error() << "Cannot resolve reference '" << refs[idx].name << "'.";
    } else if (line->getType() != GFA_SEGMENT && line->getType() != GFA_EDGE && line->getType() != GFA_GROUP) { 
      throw fatal_error() << "Cannot reference lines of type '" << (char)line->getType() << "' in group line (reference '" << refs[idx].name << "').";
    } else if (getTypeOriginal() == 'O' && line->getType() == GFA_GROUP && ((GfaGroup*)line)->getTypeOriginal() != 'O') {
      throw fatal_error() << "Ordered groups cannot reference unordered groups (reference '" << refs[idx].name << "').";
    } else {
      GfaReferenceableLine* rline = (GfaReferenceableLine*)line;
      refs[idx].resolve(line);
      resolveMembers(idx);
      rline->addGroup(this);      
    }
  }
  resolve_status = GFA_GROUP_RESOLVED;
}
