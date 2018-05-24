#pragma once
#include "headers.h"
#include "vizelement.h"

#include "gfa/graph.h"
#include "gfa/fragment.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>


using namespace ogdf;
class VizGraph;
class VizNode;

class VizFragment : public VizElement {
  public:
    VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg);
    ~VizFragment();
    
    void draw();
    
  protected:
    virtual QPointF getCenterCoord();
    virtual GfaLine* getGfaElement();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  
  private:
    GfaFragment* gfa_fragment;
    node ogdf_node;
    edge ogdf_edge;
    VizNode* viz_node;
    node connected_subnode;
};
