#include "vizgroup.h"

#include "vizgraph.h"

#include <QGraphicsDropShadowEffect>


VizGroup::VizGroup(GfaGroup* _gfa_group, VizGraph* _vg) : VizElement(_vg, _gfa_group) {
  gfa_group = _gfa_group;
}

VizGroup::~VizGroup() {
  
}

void VizGroup::draw() {
  vg->scene->addItem(this);
  const vector<GfaLine*>& members = gfa_group->getMembers();
  for (GfaLine* line : members) {
    VizElement* elem = NULL;
    if (line->getType() == GFA_SEGMENT) {
      elem = vg->getNode((GfaSegment*)line);
    } else if (line->getType() == GFA_EDGE) {
      elem = vg->getEdge((GfaEdge*)line);
    }
    if (elem) {
      elem->setParentItem(this);
    }
  }
  QGraphicsDropShadowEffect * dse = new QGraphicsDropShadowEffect();
  dse->setBlurRadius(20);
  dse->setColor(QColor(Qt::red));
  dse->setOffset(0);
  setGraphicsEffect(dse);
}

QPointF VizGroup::getCenterCoord() {
  return QPointF(0,0);
}
GfaLine* VizGroup::getGfaElement() {
  return gfa_group;
}
    
