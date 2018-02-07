#include "vizapp.h"


VizApp::VizApp(int & argc, char *argv[]) : QApplication(argc, argv) {  
  window = new QMainWindow();
  ui.setupUi(window);
  window->show();
  //window->setMouseTracking(true);
  
  if (argc >= 2) {
    open(argv[1]);
  }
}

void VizApp::open(char* filename) {
  try {
    VizGraph *g = new VizGraph(filename, ui.vizCanvas);
    graphs.push_back(g);
  }
  catch (const exception& e) {
    cerr << "Fatal error: " << e.what() << endl << endl;
  }
}
