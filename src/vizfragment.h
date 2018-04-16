#pragma once
#include "headers.h"

#include "gfa/graph.h"
#include "gfa/fragment.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>


using namespace ogdf;
class VizGraph;
class VizNode;
class VizFragment;

class VizFragment {
  public:
    VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg);
    ~VizFragment();
    
    void draw();
  
  private:
    VizGraph* vg;
    GfaFragment* gfa_fragment;
};
