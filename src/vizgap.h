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

class VizGap : public VizElement {
  public:
    VizGap(GfaGap* _gfa_gap, VizGraph* _vg);
    ~VizGap();
    
    virtual void draw();
    
    virtual GfaLine* getGfaElement();
  
  protected:
    virtual QPointF getCenterCoord();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  
  private:
    GfaGap* gfa_gap;
    
    VizNode* viz_nodes[2];
    node connected_subnodes[2];
};
