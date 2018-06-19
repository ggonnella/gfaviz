#include "vizelement.h"
#include "vizgraph.h"
#include "vizgroup.h"
#include "gfa/segment.h"

#include "gfa/line.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QJsonDocument>


VizElement::VizElement(VizGraph* _vg, GfaLine* line) {
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

long unsigned int VizElement::getGroupIndex(VizGroup* group) {
  for (long unsigned int idx = 0; idx < groups.size(); idx++) {
    if (groups[idx] == group) {
      return idx;
    }
  }
  return 0;
}


void VizElement::drawLabel() {
  if (!getGfaElement()->hasName())
    return;
  
  if (!labelItem) 
    labelItem = new VizElementLabel(QString::fromStdString(getGfaElement()->getName()), this);
  //labelItem.setParentItem(this);
  labelItem->setStyle(getOption(VIZ_LABELFONT).toString(),
                      getOption(VIZ_LABELFONTSIZE).toDouble(),
                      getOption(VIZ_LABELCOLOR).value<QColor>(),
                      getOption(VIZ_LABELOUTLINEWIDTH).toDouble(),
                      getOption(VIZ_LABELOUTLINECOLOR).value<QColor>());
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

const QVariant VizElement::getOption(VizGraphParam p) const {
  return settings.get(p, &vg->settings);
}
void VizElement::setOption(VizGraphParam p, QVariant val, bool overwrite) {
  settings.set(p, val, overwrite);
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
  setFlags(ItemIsMovable);
  setBoundingRegionGranularity(1.0);
  document()->setDocumentMargin(1.0);  
}
void VizElementLabel::setStyle(const QString& family, double size, const QColor& _color, double outlinewidth, const QColor& outlinecolor) {
  font.setFamily(family);
  font.setPointSize(size);
  setFont(font);
  outlinepen = QPen(outlinecolor, outlinewidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
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
