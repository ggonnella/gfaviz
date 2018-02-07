#pragma once
#include <headers.h>

#include "gfa/graph.h"
#include "gfa/segment.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsPathItem>
#include <QPainterPath>

using namespace ogdf;
class VizGraph;

class VizNodeSegItem : public QGraphicsPathItem {
  public:
    VizNodeSegItem();
  protected:
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    
};

typedef struct {
  unsigned long begin;
  unsigned long end;
} VizInternalEdge;

class VizNode {
  public:
    VizNode(GfaSegment* _gfa_node, VizGraph* _vg);
    ~VizNode();
  
    node getStart();
    node getEnd();
    QPointF getStartDir();
    QPointF getEndDir();
    node getNodeAtBase(unsigned long base);
    
    void draw();
    void addInternalEdge(unsigned long begin, unsigned long end);
    QPointF getCoordForBase(unsigned long base);
    QPointF getCoordForBase(unsigned long base, bool above);
  
  private:
    VizGraph* vg;
    GfaSegment* gfa_node;
    VizNodeSegItem *graphicsPathItem;
    vector<node> ogdf_nodes;
    vector<VizInternalEdge> internalEdges;
    
    QPointF getCoordForSubnode(size_t idx);
    QPointF getCoordForSubnode(size_t idx, bool above);
    
    node getNextSubnode(node n);
    //vector<QPointF> coordsAbove;
    //vector<QPointF> coordsBelow;
    
};
