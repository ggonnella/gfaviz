#include "vizgroup.h"

#include "vizgraph.h"

//#include <QGraphicsDropShadowEffect>


VizGroup::VizGroup(GfaGroup* _gfa_group, VizGraph* _vg) : VizElement(_vg, _gfa_group) {
  gfa_group = _gfa_group;
  
}

VizGroup::~VizGroup() {
  
}

void VizGroup::draw() {
  setAcceptHoverEvents(true);
  //Should only be called once
  vg->scene->addItem(this);
  const vector<GfaLine*>& members = gfa_group->getMembers();
  for (GfaLine* line : members) {
    VizElement* elem = vg->getElement(line);
    elem->addGroup(this);
    //elem->setParentItem(this);
  }
 /* const vector<GfaLine*>& members = gfa_group->getMembers();
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
  setGraphicsEffect(dse);*/
  if (getOption(VIZ_SHOWGROUPLABELS).toBool()) {
    drawLabel();
    labelItem->setStyle(getOption(VIZ_LABELFONT).toString(),
                        getOption(VIZ_LABELFONTSIZE).toDouble(),
                        getOption(VIZ_GROUPCOLOR).value<QColor>(),
                        getOption(VIZ_LABELOUTLINEWIDTH).toDouble(),
                        getOption(VIZ_LABELOUTLINECOLOR).value<QColor>());
  }
  if (getOption(VIZ_DISABLEGROUPS).toBool()) {
    setVisible(false);
  }
  //setCacheMode(QGraphicsItem::NoCache);
}

QPointF VizGroup::getCenterCoord() {
  return boundingRect().center();
}
GfaLine* VizGroup::getGfaElement() {
  return gfa_group;
}

void VizGroup::recenterLabel() {
  if (labelItem) {
    QRectF bounds = labelItem->boundingRect();
    labelItem->setPos(getCenterCoord());
    labelItem->moveBy(-bounds.width() / 2, -bounds.height() / 2);
  }
}

QRectF VizGroup::boundingRect() const {
  QRectF res = QRectF();
  const vector<GfaLine*>& members = gfa_group->getMembers();
  for (GfaLine* line : members) {
    VizElement* elem = vg->getElement(line);
    QRectF bounds = elem->boundingRect();
    if (line->getType() == GFA_SEGMENT)
      bounds.translate(elem->pos());
    qreal margin = elem->getGroups().size() * getOption(VIZ_GROUPWIDTH).toDouble();
    bounds.adjust(-margin,-margin,margin,margin);
    res |= bounds;
  }
  //cout << res.x() << " " << res.y() << " " << res.width() << " " << res.height() << endl;
  res.adjust(-5,-5,5,5);
  return res;
}
void VizGroup::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
  //cout << "groupDraw!" << endl;
  const vector<GfaLine*>& members = gfa_group->getMembers();
  for (GfaLine* line : members) {
    VizElement* elem = vg->getElement(line);
    QPen mypen = elem->pen();
    QBrush mybrush = elem->brush();
    elem->setBrush(QBrush(Qt::transparent));
    elem->setPen(QPen(getOption(VIZ_GROUPCOLOR).value<QColor>(), mypen.widthF() + (elem->getGroups().size() - elem->getGroupIndex(this)) * getOption(VIZ_GROUPWIDTH).toDouble() * 2));
    
    if (line->getType() == GFA_SEGMENT) {
      painter->translate(elem->pos());
    }
    elem->paint(painter, option, widget);
    elem->setBrush(mybrush);
    elem->setPen(mypen);
    
    /*painter->setCompositionMode(QPainter::CompositionMode_DestinationOut);
    elem->setBrush(QBrush(Qt::white));
    elem->paint(painter, option, widget);
    elem->setBrush(mybrush);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);*/
    
    if (line->getType() == GFA_SEGMENT) {
      painter->translate(-elem->pos());
    }
  }
}
    
