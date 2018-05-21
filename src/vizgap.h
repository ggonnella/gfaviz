#pragma once
#include "headers.h"
#include "vizelement.h"

#include "gfa/graph.h"
#include "gfa/gap.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>


using namespace ogdf;
class VizGraph;
class VizNode;
class VizGap;

class VizGapGraphicsItem : public VizElementGraphicsItem, public QGraphicsPathItem {
  public:
    VizGapGraphicsItem(VizGap* _parent);
    virtual void setHighlight(bool val);
    
  protected:
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) { setHover(true); };
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) { setHover(false); };
};

class VizGap : public VizElement {
  public:
    VizGap(GfaGap* _gfa_gap, VizGraph* _vg);
    ~VizGap();
    
    void draw();
    virtual void setHighlight(bool _val);
  
  protected:
    virtual QPointF getCenterCoord();
    virtual GfaLine* getGfaElement();
  
  private:
    GfaGap* gfa_gap;
    
    VizNode* viz_nodes[2];
    node connected_subnodes[2];
    VizGapGraphicsItem* graphicsItem;
};
