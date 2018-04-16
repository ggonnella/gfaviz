#pragma once
#include "headers.h"

#include "gfa/graph.h"
#include "gfa/group.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>


using namespace ogdf;
class VizGraph;

class VizGroup {
  public:
    VizGroup(GfaGroup* _gfa_group, VizGraph* _vg);
    ~VizGroup();
    
    void draw();
  
  private:
    VizGraph* vg;
    GfaGroup* gfa_group;
};
