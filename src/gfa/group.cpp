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

void GfaGroup::print(GfaVersion _version) const {
  if (getTypeOriginal() == 'P') {
    cout << "P\t" << name << '\t';
    for (size_t idx=0; idx < refs.size(); idx++) {
      if (idx>0)
        cout << ',';
      refs[idx].print(true);
    }
    cout << '\t';
    for (size_t idx=0; idx < alignments.size(); idx++) {
      if (idx>0)
        cout << ',';
      alignments[idx].print();
      //cout << alignments[idx].cigar;
    }
  } else if (getTypeOriginal() == 'O') {
    cout << "O\t" << name << '\t';
    refs[0].print(true);
    for (size_t idx=1; idx < refs.size(); idx++) {
      cout << " ";
      refs[idx].print(true);
    }
  } else if (getTypeOriginal() == 'U') {
    cout << "U\t" << name << '\t';
    refs[0].print(false);
    for (size_t idx=1; idx < refs.size(); idx++) {
      cout << " ";
      refs[idx].print(false);
    }
  }
  printTags();
  cout << endl;
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
/*
void GfaGroup::resolveMembers(size_t idx) {
  if (getTypeOriginal() == 'O' && idx > 0) {
    //find implicit members
    vector<GfaLine*> newmembers;
    GfaLine* next = refs[idx-1].ptr;
    bool reverse = refs[idx-1].is_reverse;
    GfaLine* last;    
    GfaLine* target = refs[idx].ptr;
    bool targetreverse = refs[idx].is_reverse;
    while (next->getType() == GFA_GROUP) {
      GfaGroup* group = (GfaGroup*)next;
      group->resolve(graph);
      GfaRef ref;
      if (refs[idx-1].is_reverse)
        ref = group->getFirstRef();
      else
        ref = group->getLastRef();
      next = ref.ptr;
      reverse = ref.is_reverse ^ reverse;
    }
    while (target->getType() == GFA_GROUP) {
      GfaGroup* group = (GfaGroup*)target;
      group->resolve(graph);
      GfaRef ref;
      if (refs[idx].is_reverse)
        ref = group->getLastRef();
      else
        ref = group->getFirstRef();
      target = ref.ptr;
      targetreverse = ref.is_reverse ^ targetreverse;
    }
    
    
    while (next != target) {
      newmembers.push_back(next);
      last = next;
      if (next->getType() == GFA_SEGMENT) {
        GfaSegment* seg = (GfaSegment*)next;
        const vector<GfaEdge*>* edges;
        if (reverse)
          edges = &(seg->getInedges());
        else
          edges = &(seg->getOutedges());
        if (edges->size() != 1)
          break;
        next = edges->at(0);
        reverse = (edges->at(0)->getSegment(1) == last);
      } else if (next->getType() == GFA_EDGE) {
        GfaEdge* edge = (GfaEdge*)next;
        GfaSegment* seg = edge->getSegment((reverse ? 0 : 1));
        reverse = edge->isOutedge(seg);
        next = seg;
      }
    }
    if (next != target || reverse != targetreverse) {
      newmembers.clear();
      //throw fatal_error() << "Cannot resolve path between refs '" << refs[idx-1].name << "' and '" << refs[idx].name << "' in path '" << getName() << "'.";
    }
    for (size_t t_idx = 1; t_idx < newmembers.size(); t_idx++)
      members.push_back(newmembers[t_idx]);
    
  }
  members.push_back(refs[idx].ptr);
}*/
enum GfaPathSearchStatus {
  GFAPATH_FOUND,
  GFAPATH_NOTFOUND,
  GFAPATH_MULTIPLE
};
static GfaPathSearchStatus findPathRecursive(GfaLine* src, bool srcrev, GfaLine* tgt, bool tgtrev, vector<GfaLine*>* path, int depth) {
  if (depth < 0) {
    return GFAPATH_NOTFOUND;
  }
  if (src == tgt && srcrev == tgtrev) {
    path->push_back(src);
    return GFAPATH_FOUND;
  }
  
  GfaPathSearchStatus result;
  bool found = false;
  if (src->getType() == GFA_SEGMENT) {
    GfaSegment* seg = (GfaSegment*)src;
    const vector<GfaEdge*>* edges;
    if (srcrev)
      edges = &(seg->getInedges());
    else
      edges = &(seg->getOutedges());
    for (GfaEdge* edge : *edges) {
      bool reverse = (edge->getSegment(1) == src);
      result = findPathRecursive(edge,reverse,tgt,tgtrev,path,depth-1);
      if (result == GFAPATH_FOUND) {
        if (!found) {
          found = true;
          path->push_back(src);
        } else {
          return GFAPATH_MULTIPLE;
        }
      } else if (result == GFAPATH_MULTIPLE) {
        return GFAPATH_MULTIPLE;
      }
    }
    if (found) {
      return GFAPATH_FOUND;
    } else {
      return GFAPATH_NOTFOUND;
    }
  } else if (src->getType() == GFA_EDGE) {
    GfaEdge* edge = (GfaEdge*)src;
    GfaSegment* seg = edge->getSegment((srcrev ? 0 : 1));
    bool reverse = edge->isOutedge(seg);
    result = findPathRecursive(seg,reverse,tgt,tgtrev,path,depth-1);
    if (result == GFAPATH_FOUND) {
      path->push_back(src);
    }
    return result;
  }
  return GFAPATH_NOTFOUND;
}

void GfaGroup::resolveMembers(size_t idx) {
  if (getTypeOriginal() == 'O' && idx > 0) {
    //find implicit members
    vector<GfaLine*> newmembers;
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
    GfaPathSearchStatus result = findPathRecursive(src,srcrev,tgt,tgtrev,&newmembers,graph->getPathSearchMaxDepth());
    if (result == GFAPATH_FOUND) {
      for (size_t t_idx = newmembers.size()-2; t_idx > 0; t_idx--) {
        members.push_back(newmembers[t_idx]);
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
