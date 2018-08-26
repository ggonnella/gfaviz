#include "headers.h"
#include <QMessageBox>

void showMessage(QString text) {
  QMessageBox msgBox;
  msgBox.setText(text);
  msgBox.exec();
}
bool isValidFormat(QString fmt) {
  fmt = fmt.toUpper();
  return (fmt == "BMP" ||
      fmt == "PNG" ||
      fmt == "JPG" ||
      fmt == "JPEG" ||
      fmt == "PBM" ||
      fmt == "XBM" ||
      fmt == "XPM" ||
      fmt == "SVG");
}
