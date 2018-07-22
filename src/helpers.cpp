#include "headers.h"
#include <QMessageBox>

void showMessage(QString text) {
  QMessageBox msgBox;
  msgBox.setText(text);
  msgBox.exec();
}
