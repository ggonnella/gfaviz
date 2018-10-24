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

class VizEdge : public VizElement {
  public:
    VizEdge(GfaEdge* _gfa_edge, VizGraph* _vg);
    ~VizEdge();
    
    virtual void draw();
    virtual void initOgdf();
    
    bool isDovetail;
    
    virtual GfaLine* getGfaElement();
    
    QPainterPath getPath(VizGroup* group = NULL);
    
    virtual void addTreeViewInfo(VizTreeItem* parentItem);
    
  protected:
    virtual QPointF getCenterCoord();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    
  private:
    GfaEdge* gfa_edge;
    edge ogdf_edge;
    VizNode* viz_nodes[2];
    VizNodeHighlight* highlights[2];
    QColor hcolor;
    node connected_subnodes[2];  
};
