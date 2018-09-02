#include "vizelement.h"
#include "vizgraph.h"
#include "vizgroup.h"
#include "gfa/segment.h"

#include "gfa/line.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QJsonDocument>
#include <QJsonArray>


VizElement::VizElement(VizElementType _type, VizGraph* _vg, GfaLine* line) {
  type = _type;
  vg = _vg;
  setCacheMode( QGraphicsItem::DeviceCoordinateCache );
  //setFlag(ItemSendsGeometryChanges);
  labelItem = NULL;
  layoutdata = QJsonObject();
  if (line->hasTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)) {
    char* styledata = line->getTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)->getStringValue();
    QJsonDocument jsondata = QJsonDocument::fromJson(styledata);
    if (!jsondata.isNull()) {
      settings.fromJson(jsondata.object());
    }
  }
  if (line->hasTag(VIZ_LAYOUTTAG, GFA_TAG_JSON)) {
    char* rawdata = line->getTag(VIZ_LAYOUTTAG, GFA_TAG_JSON)->getStringValue();
    QJsonDocument jsondata = QJsonDocument::fromJson(rawdata);
    if (!jsondata.isNull() && jsondata.isObject()) {
      layoutdata = jsondata.object();
    }
  }
  
  QString tooltip;
  for (size_t idx = 0; idx < line->getTags().size(); idx++) {
    GfaTag* tag = line->getTags()[idx];
    if (strncmp(tag->getKey(),VIZ_LAYOUTTAG,2) == 0 ||
        strncmp(tag->getKey(),VIZ_OPTIONSTAG,2) == 0) {
      continue;
    }
    if (idx)
      tooltip += "\n";
    tooltip += QString::fromUtf8(tag->getKey(),2) + ":" + (char)tag->getType() +
               ":" + QString::fromStdString(tag->asString());
  }
  setToolTip(tooltip);
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
  
  if (!labelItem) {
    labelItem = new VizElementLabel(QString::fromStdString(getGfaElement()->getName()), this);
    QJsonArray posdata = readLayoutData("L");
    if (posdata.size() == 2 && posdata[0].isDouble() && posdata[1].isDouble()) {
      labelItem->setOffset(QPointF(posdata[0].toDouble(), posdata[1].toDouble()));
    }
  }
  //labelItem.setParentItem(this);
  labelItem->setStyle(family, size, color, outlineWidth, outlineColor);
  QString text = QString::fromStdString(getGfaElement()->getName());
  labelItem->setText(text);
  //labelItem->updateLabel();
  labelItem->setCenter(getCenterCoord());
  //vg->scene->addItem(labelItem);
}
void VizElement::setLabelText(const QString& text) {
  labelItem->setText(text);
  /*QRectF bounds = labelItem->boundingRect();
  labelItem->setPos(getCenterCoord() + labelItem->offset);
  labelItem->moveBy(-bounds.width() / 2, -bounds.height() / 2);*/
  //labelItem->updateLabel();
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
void VizElement::saveStyle() {
  if (settings.size() > 0) {
    QJsonDocument doc(settings.toJson());
    GfaTag* tag = new GfaTag(VIZ_OPTIONSTAG, GFA_TAG_JSON, doc.toJson(QJsonDocument::Compact).constData());
    getGfaElement()->addTag(tag);
  }
}
void VizElement::saveLayout() {
  QJsonObject data = getLayoutData();
  if (data.size() > 0) {
    QJsonDocument doc(data);
    GfaTag* tag = new GfaTag(VIZ_LAYOUTTAG, GFA_TAG_JSON, doc.toJson(QJsonDocument::Compact).constData());
    getGfaElement()->addTag(tag);
  }
}
QJsonObject VizElement::getLayoutData() {
  QJsonObject data;
  if (labelItem && !labelItem->offset.isNull()) {
    QJsonArray posdata;
    double px = (double)((int)(labelItem->offset.x()*10.0))/10.0;
    double py = (double)((int)(labelItem->offset.y()*10.0))/10.0;
    posdata.push_back(QJsonValue(px));
    posdata.push_back(QJsonValue(py));
    data.insert("L",QJsonValue(posdata));
  }
  return data;
}
QJsonArray VizElement::readLayoutData(QString key) {
  if (layoutdata.contains(key) && layoutdata[key].isArray()) {
    return layoutdata[key].toArray();
  }
  return QJsonArray();
}
void VizElement::resetLayout() {
  layoutdata = QJsonObject();
  if (labelItem)
    labelItem->setOffset(QPointF());
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
  return QGraphicsPathItem::itemChange(change, value);
}

VizElementLabel::VizElementLabel(QString text, VizElement* _parent) : QGraphicsTextItem(text,_parent) {
  parent=_parent;
  setParentItem(parent);
  offset = QPointF(0,0);
  center = QPointF(0,0);
  centerChanged = false;

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  setFlags(ItemIsMovable | ItemSendsScenePositionChanges);
  
  setBoundingRegionGranularity(1.0);
  document()->setDocumentMargin(1.0);  
}

void VizElementLabel::setText(QString _str) {
  str = _str;
  //setOffset(offset);
}
void VizElementLabel::setCenter(QPointF _center) {
  QRectF bounds = boundingRect();
  center = _center - (QPointF(bounds.width(), bounds.height())/2);
  centerChanged = true;
  setPos(center+offset);
}
void VizElementLabel::setOffset(QPointF _offset) {
  offset = _offset; // - (QPointF(bounds.width(), bounds.height())/2));
  centerChanged = true;
  setPos(center+offset);
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
  if (change == ItemPositionChange && scene()) {
    if (centerChanged) {
      centerChanged = false;
    } else {
      offset += value.toPointF() - pos();
      cout << "new offset: " << offset.x() << " " << offset.y() << endl;
    }
  }
  return QGraphicsTextItem::itemChange(change, value);
}

void VizElementLabel::mousePressEvent(QGraphicsSceneMouseEvent * event) {
  if (event->modifiers() & Qt::ShiftModifier) {
    scene()->clearSelection();
    QGraphicsTextItem::mousePressEvent(event);
  } else {
    if (parent->isSelected()) {
      event->ignore();
    } else {
      scene()->clearSelection();
      parent->setSelected(true);
    }
  }
}
