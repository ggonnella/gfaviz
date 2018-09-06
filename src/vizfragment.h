#pragma once
#include "headers.h"
#include "vizelement.h"

#include "gfa/graph.h"
#include "gfa/fragment.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

#include <QGraphicsLineItem>



using namespace ogdf;
class VizGraph;
class VizNode;
class VizNodeHighlight;
class VizFragment;

class VizFragmentLine : public QGraphicsLineItem {
  public:
    VizFragmentLine(VizFragment* parent);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    
  private:
    VizFragment* frag;
};

class VizFragment : public VizElement {
  public:
    VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg);
    ~VizFragment();
    
    virtual void draw();
    
    virtual GfaLine* getGfaElement();
    
    virtual void addTreeViewInfo(VizTreeItem* parentItem);
    
    //virtual void saveLayout();
    virtual QJsonObject getLayoutData();
    void move(double dx, double dy);
    void hover(bool enter, QGraphicsSceneHoverEvent *e);
    
  protected:
    virtual QPointF getCenterCoord();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
  private:
    GfaFragment* gfa_fragment;
    node ogdf_node;
    edge ogdf_edge;
    VizNode* viz_node;
    VizNodeHighlight* highlight;
    node connected_subnode;
    VizFragmentLine* line;
    unsigned long base;
};
