#pragma once

#include <set>
#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>

#include "ui/ui_mainwindow.h"

#include "headers.h"
#include "vizgraph.h"
#include "vizsettings.h"

class VizApp : public QApplication {
  public:
    VizApp(int & argc, char *argv[]);
    void open(const QString& filename);
    void parseArguments();
    VizAppSettings settings;

  public slots:
    void closeTab(int index);
    void openDialog();
    void saveDialog();
    void renderDialog();
    void quitDialog();
    void aboutDialog();
    void helpDialog();
    void selectAll();
    void selectNone();
    void selectInvert();

  private:
    QMainWindow *window;
    Ui::VizMainWindow ui;
    QCommandLineParser optionParser;
    set<VizGraph*> graphs;
};
