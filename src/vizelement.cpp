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
  labelItem = NULL;

  if (line->hasTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)) {
    char* styledata =
      line->getTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)->getStringValue();
    QJsonDocument jsondata = QJsonDocument::fromJson(styledata);
    if (!jsondata.isNull()) {
      settings.fromJson(jsondata.object());
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

void VizElement::drawLabel(const QString& family, double size,
    const QColor& color, double outlineWidth, const QColor& outlineColor) {

  if (getType() != VIZ_FRAGMENT && !getGfaElement()->hasName())
    return;

  if (!labelItem) {
    QString text;
    if (getType() == VIZ_FRAGMENT)
      text = QString::fromStdString(
          ((GfaFragment*)getGfaElement())->getFragment());
    else if ((getType() == VIZ_SEGMENT) and getOption(VIZ_SEGLABELSEQ).toBool())
      text = QString::fromStdString(
          ((GfaSegment*)getGfaElement())->getSequence().getString());
    else text = QString::fromStdString(getGfaElement()->getName());
    labelItem = new VizElementLabel(text, this);
    QJsonArray posdata = readLayoutData("L");
    if (posdata.size() == 2 && posdata[0].isDouble() && posdata[1].isDouble()) {
      labelItem->setOffset(QPointF(posdata[0].toDouble()/10.0,
                                   posdata[1].toDouble()/10.0));
    }
  }
  labelItem->setStyle(family, size, color, outlineWidth, outlineColor);
  labelItem->setCenter(getCenterCoord());
}

void VizElement::setLabelText(const QString& text) {
  labelItem->setText(text);
  update();
}

void VizElement::setLabelVisible(bool value) {
  if (labelItem)
    labelItem->setVisible(value);
}

bool VizElement::hasOption(VizGraphParam p) {
  return settings.isset(p);
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
    GfaTag* tag = new GfaTag(VIZ_OPTIONSTAG, GFA_TAG_JSON,
        doc.toJson(QJsonDocument::Compact).constData());
    getGfaElement()->addTag(tag);
  }
}

void VizElement::saveLayout() {
  QJsonObject data = getLayoutData();
  if (data.size() > 0) {
    QJsonDocument doc(data);
    GfaTag* tag = new GfaTag(VIZ_LAYOUTTAG, GFA_TAG_JSON,
        doc.toJson(QJsonDocument::Compact).constData());
    getGfaElement()->addTag(tag);
  }
}

QJsonObject VizElement::getLayoutData() {
  QJsonObject data;
  if (labelItem && !labelItem->offset.isNull()) {
    QJsonArray posdata;
    int px = (int)(labelItem->offset.x()*10.0);
    int py = (int)(labelItem->offset.y()*10.0);
    posdata.push_back(QJsonValue(px));
    posdata.push_back(QJsonValue(py));
    data.insert("L",QJsonValue(posdata));
  }
  return data;
}

QJsonArray VizElement::readLayoutData(QString key) {
  if (getGfaElement()->hasTag(VIZ_LAYOUTTAG, GFA_TAG_JSON)) {
    char* rawdata = getGfaElement()->getTag(VIZ_LAYOUTTAG,
        GFA_TAG_JSON)->getStringValue();
    QJsonDocument jsondata = QJsonDocument::fromJson(rawdata);
    if (!jsondata.isNull() && jsondata.isObject()) {
      QJsonObject layoutdata = jsondata.object();
      if (layoutdata.contains(key) && layoutdata[key].isArray()) {
        return layoutdata[key].toArray();
      }
    }
  }
  return QJsonArray();
}

void VizElement::resetLayout() {
  getGfaElement()->removeTag(VIZ_LAYOUTTAG, GFA_TAG_JSON);
  if (labelItem)
    labelItem->setOffset(QPointF());
}

void VizElement::paint(QPainter * painter,
    const QStyleOptionGraphicsItem * option, QWidget * widget) {
  QStyleOptionGraphicsItem myoption = *option;
  if (myoption.state & QStyle::State_Selected) {
    myoption.state &= !((int)QStyle::State_Selected);
  }
  QGraphicsPathItem::paint (painter, &myoption, widget);
}

QRectF VizElement::boundingRect() const {
  return QGraphicsPathItem::boundingRect().adjusted(-3,-3,3,3);
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

QVariant VizElement::itemChange(GraphicsItemChange change,
                                const QVariant &value) {
  return QGraphicsPathItem::itemChange(change, value);
}

bool VizElement::collidesWithPath(const QPainterPath &path,
                                  Qt::ItemSelectionMode mode) const {
  return QGraphicsPathItem::collidesWithPath(path, mode) ||
    (labelItem && labelItem->collidesWithPath(labelItem->mapFromParent(path),
                                              mode));
}

void VizElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event) {
  if (event->modifiers() & Qt::ShiftModifier) {
    scene()->clearSelection();
  }
  for (VizGroup* group : groups)
    group->setSelected(!group->isSelected());
}

VizElementLabel::VizElementLabel(QString text, VizElement* _parent) :
                                 QGraphicsTextItem(text,_parent) {
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
}

void VizElementLabel::setCenter(QPointF _center) {
  QRectF bounds = boundingRect();
  center = _center - (QPointF(bounds.width(), bounds.height())/2);
  centerChanged = true;
  setPos(center+offset);
}

void VizElementLabel::setOffset(QPointF _offset) {
  offset = _offset;
  centerChanged = true;
  setPos(center+offset);
}

void VizElementLabel::setStyle(const QString& family, double size,
    const QColor& _color, double outlinewidth, const QColor& outlineColor) {
  font.setFamily(family);
  font.setPointSizeF(size);
  setFont(font);
  outlinepen = QPen(outlineColor, outlinewidth, Qt::SolidLine, Qt::RoundCap,
                    Qt::RoundJoin);
  color = _color;
  setDefaultTextColor(color);
  update();
}

void VizElementLabel::setHighlight(bool value) {
  font.setBold(value);
  setFont(font);
  update();
}

void VizElementLabel::paint(QPainter * painter,
    const QStyleOptionGraphicsItem * option, QWidget * widget) {
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

QVariant VizElementLabel::itemChange(GraphicsItemChange change,
    const QVariant &value) {
  if (change == ItemPositionChange && scene()) {
    if (centerChanged) {
      centerChanged = false;
    } else {
      offset += value.toPointF() - pos();
    }
  }
  return QGraphicsTextItem::itemChange(change, value);
}

void VizElementLabel::mousePressEvent(QGraphicsSceneMouseEvent * event) {
  if (event->modifiers() & Qt::ShiftModifier) {
    scene()->clearSelection();
    QGraphicsTextItem::mousePressEvent(event);
  } else {
    if (!parent->isSelected()){
      scene()->clearSelection();
      parent->setSelected(true);
    }
    event->ignore();
  }
}
