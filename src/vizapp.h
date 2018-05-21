#pragma once
#include "headers.h"
#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>
#include "ui_mainwindow.h"
#include "vizgraph.h"
#include "vizsettings.h"


class VizApp : public QApplication {
  public:
    VizApp(int & argc, char *argv[]);
    
    void open(const QString& filename);
    void parseArguments();
    
  private:
    QMainWindow *window;
    Ui::VizMainWindow ui;
    
    QCommandLineParser optionParser;
    VizAppSettings settings;
    
    vector<VizGraph*> graphs;
    
    
  
};
