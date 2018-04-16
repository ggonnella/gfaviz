#include "vizapp.h"


VizApp::VizApp(int & argc, char *argv[]) : QApplication(argc, argv) {  
  window = new QMainWindow();
  ui.setupUi(window);
  window->show();
  //window->setMouseTracking(true);
  
  for (int i = 1; i < argc; i++) {
    open(argv[i]);
  }
  /*if (argc >= 2) {
    open(argv[1]);
  }*/
  //ui.vizCanvas->scale(2.0,2.0);
}

void VizApp::open(char* filename) {
  try {
    VizGraph *g = new VizGraph(filename, ui.tabWidget);
    graphs.push_back(g);
    ui.tabWidget->addTab(g, QString(filename));
  }
  catch (const exception& e) {
    cerr << "Fatal error: " << e.what() << endl << endl;
  }
}
