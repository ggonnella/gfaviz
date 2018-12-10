#pragma once

#include <QDialog>
#include "ui/ui_renderForm.h"
#include "ui/ui_saveForm.h"
#include "ui/ui_about.h"
#include "ui/ui_help.h"
#include "gfa/defines.h"

class VizRenderDialog : public QDialog {
  public:
    VizRenderDialog(QWidget *parent = 0);
    int w;
    int h;
    QString filetype;
    QString filename;
    bool transparency;

  public slots:
    virtual void accept();
    void search();
    void filetypeChanged(const QString &text);

  private:
     Ui::RenderForm form;
};

class VizSaveDialog : public QDialog {
  public:
    VizSaveDialog(QWidget *parent = 0);
    bool saveStyle;
    bool saveLayout;
    GfaVersion filetype;
    QString filename;

  public slots:
    virtual void accept();
    void search();

  private:
     Ui::SaveForm form;
};

class VizAboutDialog : public QDialog {
  public:
    VizAboutDialog(QWidget *parent = 0);

  private:
     Ui::AboutForm form;
};

class VizHelpDialog : public QDialog {
  public:
    VizHelpDialog(QWidget *parent = 0);

  private:
     Ui::HelpForm form;
};
