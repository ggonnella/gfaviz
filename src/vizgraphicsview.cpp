#include "vizgraphicsview.h"
#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsItem>
#include <math.h>

void VizGraphicsView::wheelEvent(QWheelEvent * event) {
  if (event->modifiers() & Qt::ControlModifier) {
    QPointF scenepos = mapToScene(event->pos());
    qreal factor = pow(1.2, event->angleDelta().y() / 120.0);
    scale(factor, factor);
    QPointF mousepos = mapFromScene(scenepos);
    QPointF offset = mousepos - event->pos();
    horizontalScrollBar()->setValue(offset.x() + horizontalScrollBar()->value());
    verticalScrollBar()->setValue(offset.y() + verticalScrollBar()->value());
  } else {
    QGraphicsView::wheelEvent(event);
  }
}

void VizGraphicsView::drawSelectionMarker(QPainter * painter) {
  if (!scene())
    return;
  QList<QGraphicsItem*> items = scene()->selectedItems();
  if (items.size() == 0) {
    return;
  }
  QPen pen;
  pen.setStyle(Qt::DashLine);
  pen.setWidthF(0);
  painter->setPen(pen);
  painter->setBrush(QBrush(QColor(0,0,255,24)));
  QPainterPath path;
  path.setFillRule(Qt::WindingFill);
  for (QGraphicsItem* item : items) {
    path.addRect(item->boundingRect().translated(item->pos()));
  }
  
  painter->drawPath(path.simplified());
}
void VizGraphicsView::drawForeground(QPainter * painter, const QRectF & rect) {
  QGraphicsView::drawForeground(painter,rect);
  drawSelectionMarker(painter);
}

void VizGraphicsView::drawBackground(QPainter * painter, const QRectF & rect) {
  QGraphicsView::drawBackground(painter,rect);
}
