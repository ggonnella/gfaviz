#include "vizfragment.h"

#include "vizgraph.h"
#include "vecops.h"

VizFragment::VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg) {
  gfa_fragment = _gfa_fragment;
  vg = _vg;
}

VizFragment::~VizFragment() {
  
}

void VizFragment::draw() {
  
}
