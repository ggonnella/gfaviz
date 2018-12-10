#include "vizDialogs.h"
#include <iostream>
#include <QFileDialog>
#include "headers.h"

VizRenderDialog::VizRenderDialog(QWidget *parent) : QDialog(parent) {
  form.setupUi(this);
  connect(form.filenameSearch, &QToolButton::clicked, this,
          &VizRenderDialog::search);
  connect(form.renderButton, &QPushButton::clicked, this,
          &VizRenderDialog::accept);
  connect(form.cancelButton, &QPushButton::clicked, this,
          &QDialog::reject);
  connect(form.filetypeBox, &QComboBox::currentTextChanged, this,
          &VizRenderDialog::filetypeChanged);
  filename = "default";
  filetype = "png";
}

void VizRenderDialog::accept() {
  QDialog::accept();
  w = form.widthBox->value();
  h = form.heightBox->value();
  transparency = form.transparencyCheckBox->isChecked();
}

void VizRenderDialog::search() {
  QFileDialog dialog(this);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix(form.filetypeBox->currentText().toLower());
  dialog.setNameFilter(form.filetypeBox->currentText().toLower());
  if (dialog.exec()) {
    const QStringList& filenames = dialog.selectedFiles();
    if (filenames.size() > 0) {
      filename = filenames[0];
      int lastdot = filename.lastIndexOf(".");
      if (lastdot != -1) {
        QString fmt = filename.mid(lastdot+1).toLower();
        if (isValidImageFormat(fmt)) {
          filetype = fmt;
          filename.chop(fmt.length()+1);
        }
      }
      form.filetypeBox->setCurrentText(filetype);
      form.filenameEdit->setText(filename + "." + filetype);
    }
  }
}

void VizRenderDialog::filetypeChanged(const QString &text) {
  filetype = form.filetypeBox->currentText().toLower();
  form.filenameEdit->setText(filename + "." + filetype);
}

VizSaveDialog::VizSaveDialog(QWidget *parent) : QDialog(parent) {
  form.setupUi(this);
  connect(form.filenameSearch, &QToolButton::clicked, this,
          &VizSaveDialog::search);
  connect(form.saveButton, &QPushButton::clicked, this, &VizSaveDialog::accept);
  connect(form.cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  form.filetypeBox->addItem("GFA1", QVariant((int)GFA_V1));
  form.filetypeBox->addItem("GFA2", QVariant((int)GFA_V2));
}

void VizSaveDialog::accept() {
  QDialog::accept();
  saveLayout = form.layoutCheckBox->isChecked();
  saveStyle = form.styleCheckBox->isChecked();
  filetype = (GfaVersion)form.filetypeBox->currentData().toInt();
  filename = form.filenameEdit->text();
}

void VizSaveDialog::search() {
  QFileDialog dialog(this);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("gfa");
  dialog.setNameFilter(form.filetypeBox->currentText().toLower());
  if (dialog.exec()) {
    const QStringList& filenames = dialog.selectedFiles();
    if (filenames.size() > 0) {
      form.filenameEdit->setText(filenames[0]);
    }
  }
}

VizAboutDialog::VizAboutDialog(QWidget *parent) : QDialog(parent) {
  form.setupUi(this);
  connect(form.closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

VizHelpDialog::VizHelpDialog(QWidget *parent) : QDialog(parent) {
  form.setupUi(this);
  connect(form.closeButton, &QPushButton::clicked, this, &QDialog::accept);
}
