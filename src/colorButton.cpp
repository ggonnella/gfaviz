#include "colorButton.h"
#include <QColorDialog>

ColorButton::ColorButton(QWidget* parent) : QPushButton(parent) {
  setColor(QColor(Qt::black));
}
void ColorButton::setColor(QColor col) {
  value = col;
  QPalette pal = palette();
  pal.setColor(QPalette::Button, value);
  setAutoFillBackground(true);
  setPalette(pal);
  update();
}
    
void ColorButton::mousePressEvent(QMouseEvent* e) {
  QColor r = QColorDialog::getColor(value, this, "Choose color", QColorDialog::ShowAlphaChannel);
  if (r.isValid())
    setColor(r);
  emit valueChanged();
  QPushButton::mousePressEvent(e);
}
