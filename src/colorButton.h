#include <QPushButton>

class ColorButton : public QPushButton {
  Q_OBJECT
  public:
    ColorButton(QWidget* parent);
    QColor value;
    
    void setColor(QColor col);
    
  signals:
    void valueChanged();
    
  protected:
    virtual void mousePressEvent(QMouseEvent* e);
};
