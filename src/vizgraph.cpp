#include "vizgraph.h"
#include <ogdf/energybased/SpringEmbedderKK.h>
#include <ogdf/energybased/SpringEmbedderFRExact.h>
#include <ogdf/planarity/PlanarizationLayout.h>

VizGraph::VizGraph(char *filename, QGraphicsView *_view) {
  view = _view;
  
  gfa = new GfaGraph(filename);
  //gfa->print();
  determineParams();
  
  GA = GraphAttributes(G, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics );
  edgeLengths = EdgeArray<double>(G);
  
  const GfaSegmentMap& gfa_segments = gfa->getSegments();
  for (auto it : gfa_segments) {
    addNode(it.second);
  }
  
  const GfaEdgeMap& gfa_edges = gfa->getEdges();
  for (auto it : gfa_edges) {
    addEdge(it.second);
  }
  
  calcLayout();    
  draw();
}

void VizGraph::calcLayout() { 
  FMMMLayout fmmm;
  fmmm.randSeed(rand());
  fmmm.minDistCC(150);
  fmmm.fixedIterations(30);
  fmmm.fineTuningIterations(5);
  fmmm.fixedIterations(1);
  fmmm.fineTuningIterations(1);
  fmmm.useHighLevelOptions(true);
  //fmmm.unitEdgeLength(1.0*2);
  //fmmm.newInitialPlacement(true);
  //fmmm.qualityVersusSpeed(FMMMLayout::qvsGorgeousAndEfficient);
  fmmm.repForcesStrength(3);
  fmmm.postStrengthOfRepForces(0.2);
  fmmm.call(GA, edgeLengths);

  //PlanarizationLayout PL;
  //PL.call(GA);

  SpringEmbedderKK sekk;
  //sefr.minDistCC(5);
  //sefr.iterations(10);
  sekk.call(GA); //, edgeLengths); 
  
  
  cout << GA.boundingBox().p1() << endl;

  double p_xmul = view->viewport()->width() / (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  double p_ymul = view->viewport()->height() / (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_x);
  double scale = min(p_xmul, p_ymul);
  GA.scale(scale*1.0);
}

void VizGraph::draw() {
  scene = new QGraphicsScene();
  view->setScene(scene);
  view->setRenderHints(QPainter::Antialiasing);
  
  for (auto it : edges) {
    it.second->draw();
  }
  for (auto it : nodes) {
    it.second->draw();
  }
}

VizGraph::~VizGraph() {
  
}

void VizGraph::addNode(GfaSegment* seg) {
  nodes[seg] = new VizNode(seg, this);
}

void VizGraph::addEdge(GfaEdge* edge) {
  edges[edge] = new VizEdge(edge, this);
}

void VizGraph::determineParams() {
  const GfaSegmentMap& gfa_segments = gfa->getSegments();
  double p_maxnodes = 15;
  unsigned long p_longestNode = 1;
  for (auto it : gfa_segments) {
    p_longestNode = max(p_longestNode, it.second->getLength());
  }
  settings.basesPerNode = p_longestNode / p_maxnodes;
}

VizNode* VizGraph::getNode(GfaSegment *seg) {
  NodeMap::const_iterator it = nodes.find(seg);
  if(it != nodes.end())
    return it->second;
  else
    return NULL;
}
