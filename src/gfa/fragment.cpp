#include "gfa/fragment.h"
#include "gfa/segment.h"
#include "gfa/graph.h"

GfaFragment::GfaFragment() : GfaLine(GFA_FRAGMENT) {
  
}

void GfaFragment::fromLine(GfaFileReader* fr) {
  setVersion(GFA_V2);
  fr->parseGeneric(this, "Lrppppa", &segment.name, &external, &s_begin, &s_end, &f_begin, &f_end, &alignment, NULL);
  setFilled();
}

void GfaFragment::print() const {
  cout << "F\t";
  cout << segment.getName() << "\t";
  external.print();
  cout << "\t";
  s_begin.print();
  cout << "\t";
  s_end.print();
  cout << "\t";
  f_begin.print();
  cout << "\t";
  f_end.print();
  cout << "\t";
  alignment.print();
  printTags();
  cout << endl;
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
