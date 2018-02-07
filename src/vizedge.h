#pragma once
#include "headers.h"

#include "gfa/graph.h"
#include "gfa/edge.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>


using namespace ogdf;
class VizGraph;
class VizNode;
class VizEdge;

class VizEdgeGraphicsItem : public QGraphicsPathItem {
  public:
    VizEdgeGraphicsItem(VizEdge* _parent);
  private:
    VizEdge* parent;
  protected:
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    
};

class VizEdge {
  public:
    VizEdge(GfaEdge* _gfa_edge, VizGraph* _vg);
    ~VizEdge();
    
    void setHighlight(bool _val);
    
    void draw();
  
    bool isDovetail;
  private:
    VizGraph* vg;
    GfaEdge* gfa_edge;
    edge ogdf_edge;
    VizNode* viz_nodes[2];
    node connected_subnodes[2];
    VizEdgeGraphicsItem* graphicsItem;
    
};
