#include "gfa/segment.h"
#include "gfa/graph.h"

GfaSegment::GfaSegment() : GfaReferenceableLine(GFA_SEGMENT) {
  //edges.reserve(2);
  //inedges.reserve(1);
  //outedges.reserve(1);
  length_set = false;
  length = GFA_SEGMENTDEFAULTLENGTH;
}
GfaSegment::~GfaSegment() {
  
}

void GfaSegment::fromLine(GfaFileReader* fr) {
  if (fr->fieldCount() <= 3) {
    setVersion(GFA_V1);
  } else {
    if (fr->getFieldLength(3) >= 3 && fr->getFieldContent(3)[2] == ':')
      setVersion(GFA_V1);
    else
      setVersion(GFA_V2);
  }
  if (getVersion() == GFA_V1) {
    fr->parseGeneric(this, "Ls", &name, &sequence, NULL);
    
    if (sequence.isSet()) {
      length_set = true;
      length = sequence.getLength();
    } else {
      GfaTag* lntag = getTag("LN", GFA_TAG_INT);
      if (lntag != NULL) {
        length_set = true;
        length = lntag->getIntValue();
      }
    }
  } else if (getVersion() == GFA_V2) {
    fr->parseGeneric(this, "Lus", &name, &length, &sequence, NULL);
    length_set = true;
  }
  id_set = true;
  is_named = true;
  setFilled();
}
ostream& operator<< (ostream &out, const GfaSegment &s) {
  s.print(out, GFA_VUNKNOWN);
  return out;
}
void GfaSegment::print(ostream &out, GfaVersion _version) const {
  if (_version == GFA_VUNKNOWN)
    _version = getVersion();
  
  if (_version == GFA_V1) {
    out << "S\t" << name << '\t' << sequence;
  } else {
    if (!isLengthSet()) {
      throw fatal_error() << "Cannot print Segment '" << name << "' according to GFA v2 standards because length is unknown.";
    }
    out << "S\t" << name << '\t' << length << '\t' << sequence;
  }
  printTags(out);
  out << endl;
}

void GfaSegment::addEdge(GfaEdge *edge) {
  edges.push_back(edge);
  if (isInedge(edge))
    inedges.push_back(edge);
  if (isOutedge(edge))
    outedges.push_back(edge);
}

void GfaSegment::removeEdge(GfaEdge *edge) {
  vector<GfaEdge*>* vectors[3] = {&edges, &inedges, &outedges};
  for (int v_idx = 0; v_idx < 3; v_idx++) {
    for (size_t idx=0; idx < vectors[v_idx]->size(); idx++) {
      if (vectors[v_idx]->operator[](idx) == edge) {
        vectors[v_idx]->erase(vectors[v_idx]->begin()+idx);
        idx--;
      }
    }
  }
}

const vector<GfaEdge*>& GfaSegment::getEdges() const {
  return edges;
}
const vector<GfaEdge*>& GfaSegment::getInedges() const {
  return inedges;
}
const vector<GfaEdge*>& GfaSegment::getOutedges() const {
  return outedges;
}
const vector<GfaEdge*>& GfaSegment::getParallelEdges(GfaEdge* edge) const {
  if (edge->isInedge(this))
    return inedges;
  else //if (edge->isOutedge(this))
    return outedges;
}
const vector<GfaEdge*>& GfaSegment::getOppositeEdges(GfaEdge* edge) const {
  if (edge->isInedge(this))
    return outedges;
  else //if (edge->isOutedge(this))
    return inedges;
  
}
    
bool GfaSegment::isInedge(GfaEdge* edge) const {
  return edge->isInedge(this);
}

bool GfaSegment::isOutedge(GfaEdge* edge) const {
  return edge->isOutedge(this);
}

void GfaSegment::deleteme() {
  inedges.clear();
  outedges.clear();
  while (edges.size() > 0) {
    GfaEdge* edge = edges.back();
    edges.pop_back();
    edge->remove();
  }
  delete this;
};

unsigned long GfaSegment::getLength() const {
  return length;
}

const GfaSequence& GfaSegment::getSequence() const {
  return sequence;
}
void GfaSegment::setSequence(const GfaSequence& _seq) {
  sequence = _seq;
}
bool GfaSegment::isLengthSet() const {
  return length_set;
}

void GfaSegment::addFragment(GfaFragment* frag) {
  fragments.push_back(frag);
}
const vector<GfaFragment*>& GfaSegment::getFragments() const {
  return fragments;
}

void GfaSegment::addGap(GfaGap* gap) {
  gaps.push_back(gap);
}
const vector<GfaGap*>& GfaSegment::getGaps() const {
  return gaps;
}
