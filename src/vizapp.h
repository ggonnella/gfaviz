#include "headers.h"
#include <QApplication>
#include <QWidget>
#include "ui_mainwindow.h"
#include "vizgraph.h"


class VizApp : public QApplication {
  public:
    VizApp(int & argc, char *argv[]);
    
    void open(char* filename);
    
  private:
    QMainWindow *window;
    Ui::VizMainWindow ui;
    
    vector<VizGraph*> graphs;
    
    
  
};
