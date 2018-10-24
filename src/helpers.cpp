#include "headers.h"
#include <QMessageBox>
#include <random>

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
      fmt == "XPM" 
#ifndef NOSVG
      || fmt == "SVG"
#endif
      );
}

std::ranlux48 randbyte_gen;
std::uniform_int_distribution<int> randbyte_d(0,255);

int randbyte() {
  return randbyte_d(randbyte_gen);
}
