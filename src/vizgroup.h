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
    
    void draw();
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
  
  protected:
    virtual QPointF getCenterCoord();
    virtual GfaLine* getGfaElement();
    
  private:
    GfaGroup* gfa_group;
};
