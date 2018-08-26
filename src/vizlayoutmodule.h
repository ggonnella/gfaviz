#pragma once

#include <QObject>
#include <ogdf/module/LayoutModule.h>

using namespace std;
using namespace ogdf;

class VizProgress : public QObject {
  Q_OBJECT
  public:
    
  signals:
    void progress(double value);
};
class VizLayoutModule : public VizProgress, public LayoutModule {
  public:
  
};
