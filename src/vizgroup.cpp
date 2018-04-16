#include "vizgroup.h"

#include "vizgraph.h"

VizGroup::VizGroup(GfaGroup* _gfa_group, VizGraph* _vg) {
  gfa_group = _gfa_group;
  vg = _vg;
}

VizGroup::~VizGroup() {
  
}

void VizGroup::draw() {
  
}
