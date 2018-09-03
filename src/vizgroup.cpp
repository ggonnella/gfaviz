#include "vizgroup.h"

#include "vizgraph.h"

#include <QStyleOptionGraphicsItem>

//#include <QGraphicsDropShadowEffect>


VizGroup::VizGroup(GfaGroup* _gfa_group, VizGraph* _vg) : VizElement(VIZ_GROUP, _vg, _gfa_group) {
  gfa_group = _gfa_group;
  
  const vector<GfaLine*>& members = gfa_group->getMembers();
  for (GfaLine* line : members) {
    VizElement* elem = vg->getElement(line);
    elem->addGroup(this);
    //elem->setParentItem(this);
  }
  
  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  vg->scene->addItem(this);
}

VizGroup::~VizGroup() {
  
}

void VizGroup::draw() {
  
  /*
  QPainterPath path;
  const vector<GfaLine*>& members = gfa_group->getMembers();
  for (GfaLine* line : members) {
    VizElement* elem = vg->getElement(line);
    if (line->getType() == GFA_SEGMENT) {
      path += ((VizNode*)elem)->getPath(this);
    } else if (line->getType() == GFA_EDGE) {
      path += ((VizEdge*)elem)->getPath(this);
    }
  }
  setPath(path);
  setBrush(QBrush(Qt::transparent));
  setPen(QPen(getOption(VIZ_GROUPCOLOR).value<QColor>(), getOption(VIZ_GROUPWIDTH).toDouble()));*/
  
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
  if (getOption(VIZ_GROUPLABELSHOW).toBool()) {
    drawLabel(getOption(VIZ_GROUPLABELFONT).toString(),
              getOption(VIZ_GROUPLABELFONTSIZE).toDouble(),
              getOption(VIZ_GROUPCOLOR).value<QColor>(),
              getOption(VIZ_GROUPLABELOUTLINEWIDTH).toDouble(),
              getOption(VIZ_GROUPLABELOUTLINECOLOR).value<QColor>());
    //TODO: Color Label like Group or like Label? (extra setting)
    /*
    drawLabel();
    labelItem->setStyle(getOption(VIZ_LABELFONT).toString(),
                        getOption(VIZ_LABELFONTSIZE).toDouble(),
                        getOption(VIZ_GROUPCOLOR).value<QColor>(),
                        getOption(VIZ_LABELOUTLINEWIDTH).toDouble(),
                        getOption(VIZ_LABELOUTLINECOLOR).value<QColor>());*/
    setLabelVisible(true);
  } else {
    setLabelVisible(false);
  }
  setVisible(!getOption(VIZ_DISABLEGROUPS).toBool());
  //setCacheMode(QGraphicsItem::NoCache);
}

QPointF VizGroup::getCenterCoord() {
  return boundingRect().center();
}
GfaLine* VizGroup::getGfaElement() {
  return gfa_group;
}

QRectF VizGroup::boundingRect() const {
  //return VizElement::boundingRect();
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
  
  QStyleOptionGraphicsItem myoption = *option;
  myoption.state &= !((int)QStyle::State_Selected);
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
    elem->paint(painter, &myoption, widget);
    elem->setBrush(mybrush);
    elem->setPen(mypen);
    
    //painter->setCompositionMode(QPainter::CompositionMode_DestinationOut);
    //elem->setBrush(QBrush(Qt::white));
    //elem->paint(painter, option, widget);
    //elem->setBrush(mybrush);
    //painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    
    if (line->getType() == GFA_SEGMENT) {
      painter->translate(-elem->pos());
    }
  }
  //VizElement::paint(painter, option, widget);
  VizElement::paint(painter, option, widget);
}
    
