#pragma once
#include "headers.h"
#include "vizelement.h"

#include "gfa/graph.h"
#include "gfa/group.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

using namespace ogdf;
class VizGraph;

class VizGroup : public VizElement {
  public:
    VizGroup(GfaGroup* _gfa_group, VizGraph* _vg);
    ~VizGroup();
    
    virtual void draw();
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    
    virtual GfaLine* getGfaElement();
    
    virtual void addTreeViewInfo(VizTreeItem* parentItem);
    
  protected:
    virtual QPointF getCenterCoord();
    
  private:
    GfaGroup* gfa_group;
};
