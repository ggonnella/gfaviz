#include "vizgraph.h"
#include <ogdf/energybased/SpringEmbedderKK.h>
#include <ogdf/energybased/SpringEmbedderFRExact.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/energybased/DavidsonHarelLayout.h>
#include <ogdf/energybased/DTreeMultilevelEmbedder.h>
#include <ogdf/energybased/FastMultipoleEmbedder.h>
#include <ogdf/energybased/GEMLayout.h>
#include <ogdf/energybased/PivotMDS.h>
#include <ogdf/energybased/StressMinimization.h>
#include <vizStressMinimization.h>

#include <ogdf/misclayout/BertaultLayout.h>
#include <vizBertaultLayout.h>

#include <QElapsedTimer>

//todo: Kommandozeile, export als Bitmap, Stress minimization Zusammenhangskomponente, Fragments, Segmentnamen

VizGraph::VizGraph(char *filename, QWidget *parent) : QWidget(parent) {
  viewWidth = 1280;
  viewHeight = 820;
  setObjectName(QStringLiteral("tab"));
  view = new QGraphicsView(this);
  view->setObjectName(QStringLiteral("vizCanvas"));
  view->setGeometry(QRect(0, 0, viewWidth+2, viewHeight+2));
  view->scale(1.0,1.0);
  
  QElapsedTimer timer;
  timer.start();
  
  gfa = new GfaGraph(filename);
  //gfa->print();
  determineParams();
  
  GA = GraphAttributes(G, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics | GraphAttributes::edgeDoubleWeight );
  edgeLengths = EdgeArray<double>(G);
  
  const GfaSegmentMap& gfa_segments = gfa->getSegments();
  for (auto it : gfa_segments) {
    addNode(it.second);
  }
  
  const GfaEdgeMap& gfa_edges = gfa->getEdges();
  for (auto it : gfa_edges) {
    addEdge(it.second);
  }
  
  const GfaGapMap& gfa_gaps = gfa->getGaps();
  for (auto it : gfa_gaps) {
    addGap(it.second);
  }
  
  const GfaGroupMap& gfa_groups = gfa->getGroups();
  for (auto it : gfa_groups) {
    addGroup(it.second);
  }
  
  const GfaFragmentVector& gfa_fragments = gfa->getFragments();
  for (GfaFragment* frag : gfa_fragments) {
    addFragment(frag);
  }
  
  calcLayout();    
  draw();
  
  cout << "Finished after " << timer.elapsed() << " milliseconds." << endl;
}

void VizGraph::calcLayout() { 
  /*cout << "Calculating Layout" << endl;
  PlanarizationLayout PL;
  PL.call(GA);
  cout << "Planarized graph" << endl;
  
  VizBertaultLayout bl(20,20);
  bl.call(GA);
  cout << "Layouting done" << endl;*/
  /*FMMMLayout fmmm;
  fmmm.randSeed(rand());
  //fmmm.minDistCC(150);
  fmmm.fixedIterations(1);
  fmmm.fineTuningIterations(0);
  fmmm.useHighLevelOptions(true);
  //fmmm.unitEdgeLength(1.0*2);
  fmmm.newInitialPlacement(true);
  //fmmm.qualityVersusSpeed(FMMMLayout::qvsGorgeousAndEfficient);
  fmmm.repForcesStrength(3);
  fmmm.postStrengthOfRepForces(0.2);
  fmmm.call(GA, edgeLengths);*/

  //SpringEmbedderKK sekk;
  //sekk.call(GA, edgeLengths);
  
  //SpringEmbedderFRExact sefr;
  //sefr.call(GA);

  //GA.scale(15);
  //ownLayout();
  
  
  //VizBertaultLayout bl(20,20);
  //bl.call(GA);
  
  //sefr.minDistCC(5);
  //sefr.iterations(10);
  
  //DavidsonHarelLayout dhl;
  //dhl.call(GA);
  
  //DTreeMultilevelEmbedder2D dme; //Auch noch recht gut
  //dme.call(GA);
  
  //FastMultipoleEmbedder fme;
  //fme.call(GA);
  
  //GEMLayout gl;
  //gl.call(GA);
  
  //PivotMDS pmds; //sehr schnell, viele Vertizes übereinander
  //pmds.call(GA);
  
  VizStressMinimization sm; //Favorit!
  sm.useEdgeCostsAttribute(true);
  sm.call(GA);
  
  
  cout << GA.boundingBox().p1() << endl;
  cout << GA.boundingBox().p2() << endl;

  //double p_xmul = view->viewport()->width() / (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  //double p_ymul = view->viewport()->height() / (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_x);
  double p_xmul = viewWidth / (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  double p_ymul = viewHeight / (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_x);
  double scale = min(p_xmul, p_ymul);
  GA.scale(scale*1.0);
  
  cout << view->viewport()->width() << " x " << view->viewport()->height() << endl;
}

void VizGraph::draw() {
  scene = new QGraphicsScene();
  view->setScene(scene);
  view->setRenderHints(QPainter::Antialiasing);
  
  cout << "drawing gaps... ";
  for (auto it : gaps) {
    it.second->draw();
  }
  cout << "done!" << endl;
  
  cout << "drawing edges... ";
  for (auto it : edges) {
    it.second->draw();
  }
  cout << "done!" << endl;
  
  cout << "drawing fragments... ";
  for (auto it : fragments) {
    it.second->draw();
  }
  cout << "done!" << endl;
  
  cout << "drawing nodes... ";
  for (auto it : nodes) {
    it.second->draw();
  }
  cout << "done!" << endl;
  
  cout << "drawing groups... ";
  for (auto it : groups) {
    it.second->draw();
  }
  cout << "done!" << endl;
  
}

/*VizGraph::~VizGraph() {
  
}*/

void VizGraph::addNode(GfaSegment* seg) {
  nodes[seg] = new VizNode(seg, this);
}

void VizGraph::addEdge(GfaEdge* edge) {
  edges[edge] = new VizEdge(edge, this);
}

void VizGraph::addGap(GfaGap* gap) {
  gaps[gap] = new VizGap(gap, this);
}

void VizGraph::addGroup(GfaGroup* group) {
  groups[group] = new VizGroup(group, this);
}

void VizGraph::addFragment(GfaFragment* fragment) {
  fragments[fragment] = new VizFragment(fragment, this);
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
