#include "gfa/fragment.h"
#include "gfa/segment.h"
#include "gfa/graph.h"

GfaFragment::GfaFragment() : GfaLine(GFA_FRAGMENT) {
  
}
GfaFragment::~GfaFragment() {
  
}

void GfaFragment::fromLine(GfaFileReader* fr) {
  setVersion(GFA_V2);
  fr->parseGeneric(this, "Lrppppa", &segment.name, &external, &s_begin, &s_end, &f_begin, &f_end, &alignment, NULL);
  setFilled();
}
ostream& operator<< (ostream &out, const GfaFragment &f) {
  f.print(out, GFA_VUNKNOWN);
  return out;
}
void GfaFragment::print(ostream &out, GfaVersion _version) const {
  out << "F\t";
  out << segment.getName() << "\t";
  out << external;
  out << "\t";
  out << s_begin;
  out << "\t";
  out << s_end;
  out << "\t";
  out << f_begin;
  out << "\t";
  out << f_end;
  out << "\t";
  out << alignment;
  printTags(out);
  out << endl;
}

void GfaFragment::resolve(GfaGraph* _g) {
  GfaSegment *seg = _g->getSegment(segment.name);
  if (seg == NULL) {
    throw fatal_error() << "Cannot find segment '" << segment.name << "'.";
  } else {
    segment.resolve(seg);
    seg->addFragment(this);
  }
}

GfaSegment* GfaFragment::getSegment() const {
  return (GfaSegment*)segment.ptr;
}
unsigned long GfaFragment::getSegmentBegin() const {
  return s_begin.val;
}
unsigned long GfaFragment::getSegmentEnd() const {
  return s_end.val;
}

const string& GfaFragment::getFragment() const {
  return external.getName();
}
unsigned long GfaFragment::getFragmentBegin() const {
  return f_begin.val;
}
unsigned long GfaFragment::getFragmentEnd() const {
  return f_end.val;
}
