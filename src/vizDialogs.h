#pragma once

#include <QDialog>
#include "ui_renderForm.h"
#include "ui_saveForm.h"
#include "gfa/defines.h"

class VizRenderDialog : public QDialog {
  public:
    VizRenderDialog(QWidget *parent = 0);
    int w;
    int h;
    QString filetype;
    QString filename;
    
    
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
