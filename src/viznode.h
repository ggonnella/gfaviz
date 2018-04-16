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
  unsigned long pos;
  double size;
  bool up;
} VizNodeHighlightEvent;

class VizNodeHighlight { //: public QGraphicsItem {
  public:
    VizNodeHighlight(unsigned long _begin, unsigned long _end, double _size); //QGraphicsItem* parent = NULL
    unsigned long begin;
    unsigned long end;
    double startheight;
    double size;
    vector<VizNodeHighlightEvent> events;
  private:
};

class VizNode {
  public:
    VizNode(GfaSegment* _gfa_node, VizGraph* _vg);
    ~VizNode();
  
    node getStart();
    node getEnd();
    QPointF getStartDir();
    QPointF getEndDir();
    node getNodeAtBase(unsigned long base);
    
    VizNodeHighlight* registerHighlight(unsigned long begin, unsigned long end, double size);
    //void processHighlights();
    
    void draw();
    void drawHighlight(VizNodeHighlight* highlight);
    QPointF getCoordForBase(unsigned long base, double offset = 0.0f);
  
    vector<node> ogdf_nodes;
    vector<edge> ogdf_edges;
  
  private:
    double width;
    VizGraph* vg;
    GfaSegment* gfa_node;
    VizNodeSegItem *graphicsPathItem;
    vector<VizNodeHighlight*> highlights;
    
    
    QPointF getCoordForSubnode(size_t idx, double offset = 0.0f);
    
    node getNextSubnode(node n);
};
