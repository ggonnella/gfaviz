#pragma once
#include "ui/ui_labelStyleForm.h"

class vizLabelStyleWidget : public QWidget {
  public:
    vizLabelStyleWidget(QWidget* parent = 0) : QWidget(parent) {
      form.setupUi(this);
    }
    Ui::labelStyleForm form;
};
