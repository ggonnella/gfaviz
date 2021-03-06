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
class VizNode;

class VizNodeHighlight : public QGraphicsPathItem {
  public:
    VizNodeHighlight(VizNode* _parent, unsigned long _base_start, unsigned long _base_end);
    void draw();
    void setColor(QColor c);
    void setVisibility(bool val);
  
  private:
    VizNode* parent;
    QColor color;
    unsigned long start;
    unsigned long end;
};
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
    virtual void initOgdf();
    QPointF getCoordForBase(unsigned long base, double offset = 0.0f);
  
    vector<node> ogdf_nodes;
    vector<edge> ogdf_edges;
    
    virtual QRectF boundingRect() const;
    
    virtual GfaLine* getGfaElement();
    QPainterPath getPath(VizGroup* group = NULL);
    QPainterPath getSubPath(unsigned long start, unsigned long end);
  
    virtual void addTreeViewInfo(VizTreeItem* parentItem);
    
    //virtual void saveLayout();
    virtual QJsonObject getLayoutData();
    
    VizNodeHighlight* registerHighlight(unsigned long start, unsigned long end);

    unsigned long get_n_subsegs();
    double getDrawingLength();
    
  protected:
    virtual QPointF getCenterCoord();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
  private:
    GfaSegment* gfa_node;
    unsigned long n_subsegs;
    
    QPointF getCoordForSubnode(size_t idx, double offset = 0.0f);
    //QPointF getCenterCoord();
    vector<VizNodeHighlight*> highlights;
    
    node getNextSubnode(node n);
};
