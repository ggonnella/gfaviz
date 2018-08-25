#include "vizelement.h"
#include "vizgraph.h"
#include "vizgroup.h"
#include "gfa/segment.h"

#include "gfa/line.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QJsonDocument>


VizElement::VizElement(VizElementType _type, VizGraph* _vg, GfaLine* line) {
  type = _type;
  vg = _vg;
  setCacheMode( QGraphicsItem::DeviceCoordinateCache );
  //setFlag(ItemSendsGeometryChanges);
  labelItem = NULL;
  
  if (line->hasTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)) {
    char* styledata = line->getTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)->getStringValue();
    QJsonDocument jsondata = QJsonDocument::fromJson(styledata);
    if (!jsondata.isNull()) {
      settings.fromJson(jsondata.object());
    }
  }
}

VizElement::~VizElement() {
  delete labelItem;
}

void VizElement::addGroup(VizGroup* group) {
  groups.push_back(group);
}

const vector<VizGroup*>& VizElement::getGroups() const {
  return groups;
}

VizElementType VizElement::getType() {
  return type;
}
const QString VizElement::getTypeName(VizElementType t) {
  switch(t) {
    case VIZ_SEGMENT: return "segment";
    case VIZ_EDGE: return "edge";
    case VIZ_GROUP: return "group";
    case VIZ_GAP: return "gap";
    case VIZ_FRAGMENT: return "fragment";
    default: break;
  }
  return "unknown";
}

long unsigned int VizElement::getGroupIndex(VizGroup* group) {
  for (long unsigned int idx = 0; idx < groups.size(); idx++) {
    if (groups[idx] == group) {
      return idx;
    }
  }
  return 0;
}


void VizElement::drawLabel(const QString& family, double size, const QColor& color, double outlineWidth, const QColor& outlineColor) {
  if (!getGfaElement()->hasName())
    return;
  
  if (!labelItem) 
    labelItem = new VizElementLabel(QString::fromStdString(getGfaElement()->getName()), this);
  //labelItem.setParentItem(this);
  labelItem->setStyle(family, size, color, outlineWidth, outlineColor);
  QString text = QString::fromStdString(getGfaElement()->getName());
  if (text != labelItem->toPlainText())
    labelItem->setPlainText(text);
  
  QRectF bounds = labelItem->boundingRect();
  labelItem->setPos(getCenterCoord());
  labelItem->moveBy(-bounds.width() / 2, -bounds.height() / 2);
  //vg->scene->addItem(labelItem);
}
void VizElement::setLabelText(const QString& text) {
  labelItem->setPlainText(text);
  QRectF bounds = labelItem->boundingRect();
  labelItem->setPos(getCenterCoord());
  labelItem->moveBy(-bounds.width() / 2, -bounds.height() / 2);
  update();
}
void VizElement::setLabelVisible(bool value) {
  if (labelItem)
    labelItem->setVisible(value);
}

const QVariant VizElement::getOption(VizGraphParam p) const {
  return settings.get(p, &vg->settings);
}
void VizElement::setOption(VizGraphParam p, QVariant val, bool overwrite) {
  settings.set(p, val, overwrite);
  if (scene())
    draw();
}
void VizElement::unsetOption(VizGraphParam p) {
  settings.unset(p);
}
void VizElement::unsetAllOptions() {
  settings.unsetAll();
}

void VizElement::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
  QGraphicsPathItem::paint (painter, option, widget);
  /*if (getGfaElement()->getType() == GFA_SEGMENT || getGfaElement()->getType() == GFA_EDGE) {
    QPen mypen = pen();
    QBrush mybrush = brush();
    setBrush(QBrush(Qt::transparent));
    setPen(QPen(Qt::green, 5));
    painter->setCompositionMode(QPainter::CompositionMode_DestinationOver);
    QGraphicsPathItem::paint (painter, option, widget);
    setBrush(mybrush);
    setPen(mypen);
    if (getGfaElement()->getType() == GFA_SEGMENT) {
      painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
      GfaSegment* seg = (GfaSegment*)getGfaElement();
      painter->translate(-pos());
      for (GfaEdge* edge : seg->getEdges()) {
        vg->getEdge(edge)->paint(painter,option,widget);
      }
      painter->translate(pos());
    }
  }*/
  
}
void VizElement::hoverEnterEvent(QGraphicsSceneHoverEvent *e) { 
  for (VizGroup* group : groups)
    group->hoverEnterEvent(e);
  if (labelItem)
    labelItem->setHighlight(true);
};
void VizElement::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  for (VizGroup* group : groups)
    group->hoverLeaveEvent(e);
  if (labelItem)
    labelItem->setHighlight(false);
};  



QVariant VizElement::itemChange(GraphicsItemChange change, const QVariant &value) {
  /*if (change == QGraphicsItem::ItemSelectedHasChanged && scene()) {
    if (isSelected()) {
      cout << "lol" << endl;
      for (VizGroup* group : groups)
        group->setSelected(true);
    }
  }*/
  //cout << "lol123 " << (int)change <<  endl;
  /*if (change == ItemPositionHasChanged && scene()) {
    for (VizGroup* group : groups) {
      group->update();
    }
  }
  if (change == ItemSelectedHasChanged && scene()) {
    for (VizGroup* group : groups) {
      group->update(boundingRect());
    }
  }*/
  return QGraphicsPathItem::itemChange(change, value);
}

VizElementLabel::VizElementLabel(QString text, VizElement* _parent) : QGraphicsTextItem(text,_parent) {
  parent=_parent;
  setParentItem(parent);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  setFlags(ItemIsMovable | ItemIsSelectable);
  
  setBoundingRegionGranularity(1.0);
  document()->setDocumentMargin(1.0);  
}
void VizElementLabel::setStyle(const QString& family, double size, const QColor& _color, double outlinewidth, const QColor& outlineColor) {
  font.setFamily(family);
  font.setPointSize(size);
  setFont(font);
  outlinepen = QPen(outlineColor, outlinewidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  color = _color;
  setDefaultTextColor(color);
  update();
}

void VizElementLabel::setHighlight(bool value) {
  font.setBold(value);
  setFont(font);
  update();
}

void VizElementLabel::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
  QTextCharFormat format;
  format.setTextOutline (outlinepen);
  QTextCursor cursor (this->document());
  cursor.select (QTextCursor::Document);
  cursor.mergeCharFormat (format);
  QGraphicsTextItem::paint (painter, option, widget);
  format.setTextOutline (QPen (Qt::transparent));
  cursor.mergeCharFormat (format);
  QGraphicsTextItem::paint (painter, option, widget);
}

QVariant VizElementLabel::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == QGraphicsItem::ItemSelectedChange && scene()) {
    if (value.toBool()) {
      parent->setSelected(true);
    }
    return QVariant(false);
  }
  return QGraphicsTextItem::itemChange(change, value);
}
/*
void VizElementLabel::mousePressEvent(QGraphicsSceneMouseEvent * event) {
  cout << (int)event->flags() << endl;
  if (event->flags() & Qt::MouseEventCreatedDoubleClick)
    cout << "lol!" << endl;
  QGraphicsTextItem::mousePressEvent(event);
}

void VizElementLabel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event) {
  QGraphicsTextItem::mouseDoubleClickEvent(event);
  //parent->vg->scene->clearSelection();
  //QGraphicsTextItem::mousePressEvent(event);
}*/
