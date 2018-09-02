#include <QtWidgets/QGraphicsView>

class VizGraphicsView : public QGraphicsView {
  public:
    VizGraphicsView(QWidget* parent) : QGraphicsView(parent) {}
    virtual void wheelEvent(QWheelEvent * event);
  
};
