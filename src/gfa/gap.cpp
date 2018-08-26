#include "gfa/gap.h"
#include "gfa/segment.h"
#include "gfa/graph.h"

GfaGap::GfaGap() : GfaReferenceableLine(GFA_GAP) {
  
}

void GfaGap::fromLine(GfaFileReader* fr) {
  setVersion(GFA_V2);
  fr->parseGeneric(this, "lrruv", &name, &segments[0], &segments[1], &dist, &variance, NULL);
  id_set = true;
  if (name[0] != '*')
    is_named = true;
  setFilled();
}

ostream& operator<< (ostream &out, const GfaGap &g) {
  g.print(out);
  return out;
}
void GfaGap::print(ostream &out) const {
  out << "G\t" << getName() << "\t";
  segments[0].print(out,true);
  out << '\t';
  segments[1].print(out,true);
  out << '\t' << dist << '\t';
  out << variance;
  printTags(out);
  out << endl;
}

void GfaGap::resolve(GfaGraph* _g) {
  GfaSegment *seg1 = _g->getSegment(segments[0].name);
  GfaSegment *seg2 = _g->getSegment(segments[1].name);
  if (seg1 == NULL) {
    throw fatal_error() << "Cannot find segment '" << segments[0].name << "'.";
  } else if (seg2 == NULL) {
    throw fatal_error() << "Cannot find segment '" << segments[1].name << "'.";
  } else {
    segments[0].resolve(seg1);
    seg1->addGap(this);
    segments[1].resolve(seg2);
    seg2->addGap(this);
  }
}


GfaSegment* GfaGap::getSegment(int segnum) const {
  return (GfaSegment*)segments[segnum].ptr;
}
GfaSegment* GfaGap::getOppositeSegment(GfaSegment* seg) const {
  if (seg == segments[0].ptr)
    return getSegment(1);
  else
    return getSegment(0);
}
bool GfaGap::isInedge(int segnum) const {
  return segments[segnum].is_reverse == (segnum == 0);
}
bool GfaGap::isInedge(const GfaSegment *seg) const {
  if (seg == segments[0].ptr)
    return isInedge(0);
  else if (seg == segments[1].ptr)
    return isInedge(1);
  else
    return false;
}
bool GfaGap::isOutedge(int segnum) const {
  return segments[segnum].is_reverse == (segnum == 1);
}
bool GfaGap::isOutedge(const GfaSegment *seg) const {
  if (seg == segments[0].ptr)
    return isOutedge(0);
  else if (seg == segments[1].ptr)
    return isOutedge(1);
  else
    return false;
}
unsigned long GfaGap::getDist() const {
  return dist;
}
