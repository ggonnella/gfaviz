#include "vizelement.h"
#include "vizgraph.h"

#include "gfa/line.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

VizElement::VizElement(VizGraph* _vg) {
  vg = _vg;
  setCacheMode( QGraphicsItem::DeviceCoordinateCache );
}

VizElement::~VizElement() {
  
}

void VizElement::drawLabel() {
  if (!getGfaElement()->hasName())
    return;
  
  //labelItem = new VizElementLabel(QString::fromStdString(getGfaElement()->getName()), this);
  labelItem.setParentItem(this);
  QString text = QString::fromStdString(getGfaElement()->getName());
  if (text != labelItem.toPlainText())
    labelItem.setPlainText(text);
  
  QRectF bounds = labelItem.boundingRect();
  labelItem.setPos(getCenterCoord());
  labelItem.moveBy(-bounds.width() / 2, -bounds.height() / 2);
  //vg->scene->addItem(labelItem);
}

const QVariant& VizElement::getOption(VizGraphParam p) const {
  return vg->settings.get(p);
}

VizElementLabel::VizElementLabel() { //QString text, VizElement* _parent) : QGraphicsTextItem(text,_parent) {
  setCacheMode( QGraphicsItem::DeviceCoordinateCache );
  setFlags(ItemIsMovable);
  setBoundingRegionGranularity(1.0);
  QFont font;
  font.setBold(true);
  //font.setFamily("Arial");
  setFont(font);
  /*QTextDocument* document = new QTextDocument;
  QTextCharFormat charFormat;
  QFont font;
  //font.setBold(true);
  //font.setWeight(QFont::Black);
  font.setFamily("Arial");
  setDefaultTextColor(QColor(0,0,0));
  //font.setPointSize(16);
  charFormat.setFont(font);
  QPen outlinePen = QPen (QColor(255,255,255), 0.6, Qt::SolidLine);
  charFormat.setTextOutline(outlinePen);
  QTextCursor cursor = QTextCursor(document);
  cursor.insertText(text, charFormat);
  setDocument(document);*/
  //labelItem->setTextInteractionFlags(Qt::TextEditable);
}

void VizElementLabel::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
  QTextCharFormat format;
  format.setTextOutline (QPen (Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)); // Color and width of outline
  QTextCursor cursor (this->document());
  cursor.select (QTextCursor::Document);
  cursor.mergeCharFormat (format);
  QGraphicsTextItem::paint (painter, option, widget);
  format.setTextOutline (QPen (Qt::transparent));
  cursor.mergeCharFormat (format);
  QGraphicsTextItem::paint (painter, option, widget);
}
