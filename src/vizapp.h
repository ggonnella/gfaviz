#pragma once
#include "headers.h"
#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>
#include "ui_mainwindow.h"
#include "vizgraph.h"
#include "vizsettings.h"
#include <set>


class VizApp : public QApplication {
  public:
    VizApp(int & argc, char *argv[]);
    
    void open(const QString& filename);
    void parseArguments();
    
    VizAppSettings settings;
    
  public slots:
    void closeTab(int index);
    void openDialog();
    void renderDialog();
    void quitDialog();
  
  private:
    QMainWindow *window;
    Ui::VizMainWindow ui;
    
    QCommandLineParser optionParser;

    set<VizGraph*> graphs;
    
    
  
};
