#include "vizlayout.h"
#include "vizgraph.h"

VizLayout::VizLayout() {
      
}

VizLayoutSM::VizLayoutSM() {
  
}
void VizLayoutSM::prepare(VizGraph *graph) {
  /*for (auto it : graph->getEdges()) {
    VizEdge* edge = it.second;
    if (edge->isDovetail) {
      graph->GA.doubleWeight(edge->ogdf_edge) = 10;
    } else {
      graph->GA.doubleWeight(edge->ogdf_edge) = 80;
    }
  }*/
}
void VizLayoutSM::exec(VizGraph *graph) {
  
}

VizLayoutFMMM::VizLayoutFMMM() {
  
}
void VizLayoutFMMM::prepare(VizGraph *graph) {

}
void VizLayoutFMMM::exec(VizGraph *graph) {
  
}
