#pragma once
#include "headers.h"
#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>
#include "ui_mainwindow.h"
#include "vizgraph.h"

#define QT_FATAL_CRITICALS 1

typedef struct {
  bool guiEnabled;
  bool renderEnabled;
  QString outputFile;
  QString outputFormat;
  unsigned int width;
  unsigned int height;
} VizAppSettings;

class VizApp : public QApplication {
  public:
    VizApp(int & argc, char *argv[]);
    
    void open(const QString& filename);
    void parseArguments();
    
  private:
    QString getOutputFilename(QString filename, int n);
  
    QMainWindow *window;
    Ui::VizMainWindow ui;
    
    QCommandLineParser optionParser;
    VizAppSettings settings;
    
    vector<VizGraph*> graphs;
    
    
  
};
