#include <QtWidgets/QGraphicsView>

class VizGraphicsView : public QGraphicsView {
  public:
    VizGraphicsView(QWidget* parent) : QGraphicsView(parent) {}
    virtual void wheelEvent(QWheelEvent * event);
  
  protected:
    void drawSelectionMarker(QPainter * painter);
    virtual void drawBackground(QPainter * painter, const QRectF & rect);
    virtual void drawForeground(QPainter * painter, const QRectF & rect);
};
