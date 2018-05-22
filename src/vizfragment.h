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
class VizFragment;

class VizFragmentGraphicsItem : public VizElementGraphicsItem, public QGraphicsPathItem {
  public:
    VizFragmentGraphicsItem(VizFragment* _parent);
    virtual void setHighlight(bool val);
  private:
    
  protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) { setHover(true); };
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) { setHover(false); };
    
};

class VizFragment : public VizElement {
  public:
    VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg);
    ~VizFragment();
    
    void draw();
    virtual void setHighlight(bool _val);
    
  protected:
    virtual QPointF getCenterCoord();
    virtual GfaLine* getGfaElement();
  
  private:
    GfaFragment* gfa_fragment;
    node ogdf_node;
    edge ogdf_edge;
    VizNode* viz_node;
    node connected_subnode;
    VizFragmentGraphicsItem* graphicsItem;
};
