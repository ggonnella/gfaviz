#pragma once
#include "headers.h"
#include "vizelement.h"

#include "gfa/graph.h"
#include "gfa/edge.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>


using namespace ogdf;
class VizGraph;
class VizNode;
class VizNodeHighlight;
class VizEdge;

class VizEdgeGraphicsItem : public VizElementGraphicsItem, public QGraphicsPathItem {
  public:
    VizEdgeGraphicsItem(VizEdge* _parent);
    virtual void setHighlight(bool val);
  private:
    
  protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) { setHover(true); };
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) { setHover(false); };
    
};

class VizEdge : public VizElement {
  public:
    VizEdge(GfaEdge* _gfa_edge, VizGraph* _vg);
    ~VizEdge();
    
    void draw();
    
    bool isDovetail;
    
    virtual void setHighlight(bool _val);
    
  protected:
    virtual QPointF getCenterCoord();
    virtual GfaLine* getGfaElement();
    
  private:
    GfaEdge* gfa_edge;
    edge ogdf_edge;
    VizNode* viz_nodes[2];
    node connected_subnodes[2];
    VizNodeHighlight* highlights[2];
    VizEdgeGraphicsItem* graphicsItem;
    
    
};
