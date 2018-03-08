#pragma once
#include "headers.h"

#include "gfa/graph.h"
#include "gfa/gap.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>


using namespace ogdf;
class VizGraph;
class VizNode;
class VizGap;

class VizGapGraphicsItem : public QGraphicsPathItem {
  public:
    VizGapGraphicsItem(VizGap* _parent);
  private:
    VizGap* parent;
  protected:
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    
};

class VizGap {
  public:
    VizGap(GfaGap* _gfa_gap, VizGraph* _vg);
    ~VizGap();
    
    void draw();
    void setHighlight(bool _val);
  
  private:
    VizGraph* vg;
    GfaGap* gfa_gap;
    
    VizNode* viz_nodes[2];
    node connected_subnodes[2];
    VizGapGraphicsItem* graphicsItem;
};
