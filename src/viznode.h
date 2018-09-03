#pragma once
#include "headers.h"
#include "vizelement.h"

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


class VizNode : public VizElement {
  public:
    VizNode(GfaSegment* _gfa_node, VizGraph* _vg);
    ~VizNode();
  
    node getStart();
    node getEnd();
    QPointF getStartDir();
    QPointF getEndDir();
    QPointF getDirAtBase(unsigned long base);
    node getNodeAtBase(unsigned long base);
    
    virtual void draw();
    QPointF getCoordForBase(unsigned long base, double offset = 0.0f);
  
    vector<node> ogdf_nodes;
    vector<edge> ogdf_edges;
    
    virtual QRectF boundingRect() const;
    
    virtual GfaLine* getGfaElement();
    QPainterPath getPath(VizGroup* group = NULL);
  
    virtual void addTreeViewInfo(VizTreeItem* parentItem);
    
    //virtual void saveLayout();
    virtual QJsonObject getLayoutData();
    
  protected:
    virtual QPointF getCenterCoord();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
  private:
    GfaSegment* gfa_node;
    
    QPointF getCoordForSubnode(size_t idx, double offset = 0.0f);
    //QPointF getCenterCoord();
    
    node getNextSubnode(node n);
};
