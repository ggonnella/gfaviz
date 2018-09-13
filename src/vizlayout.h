#pragma once

#include "ui_LayoutOptions_SM.h"
#include "ui_LayoutOptions_FMMM.h"
#include "vizStressMinimization.h"
#include <vizFMMMLayout.h>
#include "vizComponentSplitterLayout.h"
#include "vizlayoutmodule.h"
#include <vector>


using namespace std;
using namespace ogdf;

class VizGraph;


class VizLayout : public QObject{
  Q_OBJECT
  public:
    VizLayout(VizGraph* _vg);

    virtual QString getName() = 0;
    virtual QString getDescription() = 0;
    virtual void reset() = 0;
    virtual void apply(double ratio, bool fromGui = false) = 0;
    virtual VizLayoutModule* getLayoutModule() = 0;
    virtual QWidget* getWidget() = 0;
  public slots:
    void abort();
  protected:
    QWidget* widget;
    void exec(double ratio);
    VizGraph* vg;
    bool useEdgelengths;
    
  private:
  
  signals:
    void progress(double value);
};

class VizLayoutSM : public VizLayout {
  public:
    VizLayoutSM(VizGraph* _vg) : VizLayout(_vg) { 
      SM = NULL;
    }
    virtual QWidget* getWidget() {
      if (!widget) {
        widget = new QWidget();
        QSizePolicy::Policy policy = QSizePolicy::Ignored;
        widget->setSizePolicy(policy,policy);
        optionsForm.setupUi(widget);
      }
      return widget;
    };
    virtual QString getName() {
      return "Stress Minimization";
    };
    virtual QString getDescription() {
      return "Energy-based layout using stress minimization. It is slower than FMMM, but provides better visual results.";
    };
    virtual void reset();
    virtual void apply(double ratio, bool fromGui = false);
    virtual VizLayoutModule* getLayoutModule() {
      return SM;
    }
  
  protected:
    VizStressMinimization* SM;
    Ui::SMLayoutOptionForm optionsForm;
};

class VizLayoutFMMM : public VizLayout  {
  public:
    VizLayoutFMMM(VizGraph* _vg) : VizLayout(_vg) {
      FMMM = NULL;
      useEdgelengths = true;
    };
    virtual QWidget* getWidget() {
      if (!widget) {
        widget = new QWidget();
        QSizePolicy::Policy policy = QSizePolicy::Ignored;
        widget->setSizePolicy(policy,policy);
        optionsForm.setupUi(widget);
      }
      return widget;
    };
    virtual QString getName() {
      return "Fast Multipole Multilevel Method";
    };
    virtual QString getDescription() {
      return "FMMM is a force-directed graph drawing method. It provides poorer visual results than Stress Minimization, but is very fast and thus also suited also for very large graphs.";
    };
    virtual void reset();
    virtual void apply(double ratio, bool fromGui = false);
    virtual VizLayoutModule* getLayoutModule() {
      return FMMM;
    }
    
    
  protected:
    VizFMMMLayout* FMMM;
    Ui::FMMMLayoutOptionForm optionsForm;
};
