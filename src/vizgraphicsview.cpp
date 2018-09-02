#include "vizgraphicsview.h"
#include <QWheelEvent>
#include <QScrollBar>
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
