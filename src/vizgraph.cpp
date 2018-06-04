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
#include <ogdf/packing/ComponentSplitterLayout.h>
#include <vizStressMinimization.h>
#include <vizComponentSplitterLayout.h>

#include <ogdf/misclayout/BertaultLayout.h>
#include <vizBertaultLayout.h>

#include <QSvgGenerator>
//#include <QSvg>
#include <QElapsedTimer>
#include <QFileInfo>

//todo: Kommandozeile, export als Bitmap, Stress minimization Zusammenhangskomponente, Fragments, Segmentnamen

VizGraph::VizGraph(const QString& filename, const VizAppSettings& appSettings, QWidget *parent) : QWidget(parent) {
  cout << "Opening " << filename.toStdString() << "..." << endl;
  settings = appSettings.graphSettings;
  settings.filename = filename;
  viewWidth = appSettings.width;
  viewHeight = appSettings.height;
  setObjectName(QStringLiteral("tab"));
  form.setupUi(this);
  connect(form.ButtonZoomIn, &QPushButton::clicked, this, &VizGraph::zoomIn);
  connect(form.ButtonZoomOut, &QPushButton::clicked, this, &VizGraph::zoomOut);
  connect(form.ButtonZoomDefault, &QPushButton::clicked, this, &VizGraph::zoomDefault);
  
  
  view = form.vizCanvas;
  //view->setObjectName(QStringLiteral("vizCanvas"));
  view->setGeometry(QRect(0, 0, viewWidth+2, viewHeight+2));
  view->setDragMode(QGraphicsView::RubberBandDrag); //QGraphicsView::ScrollHandDrag);
  //view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  
  QElapsedTimer timer;
  timer.start();
  
  gfa = new GfaGraph(qPrintable(filename));
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
  
  
  if (appSettings.renderEnabled) {
    QString outputFile = "";
    if (appSettings.outputFile == "") {
      QFileInfo fi(filename);
      outputFile = fi.fileName();
    } else {
      outputFile = appSettings.outputFile;
    }
    QString basename = outputFile;
    int suffix = 1;
    while (QFileInfo(outputFile + "." + appSettings.outputFormat).exists()) {
      outputFile = basename + "." + QString::number(suffix);
      suffix++;
    }
    renderToFile(outputFile, appSettings.outputFormat);
  }
  
  cout << "Finished after " << timer.elapsed() << " milliseconds." << endl;
}
void VizGraph::setDisplaySize(unsigned int width, unsigned int height) {
  double p_xmul = (double)width / (double)viewWidth;
  double p_ymul = (double)height / (double)viewHeight;
  double scale = min(p_xmul, p_ymul);
  view->scale(scale,scale);
  view->setGeometry(QRect(0, 0, width+2, height+2));
}
void VizGraph::calcLayout() { 
  /*cout << "Calculating Layout" << endl;
  PlanarizationLayout PL;
  PL.call(GA);
  cout << "Planarized graph" << endl;
  
  VizBertaultLayout bl(20,20);
  bl.call(GA);
  cout << "Layouting done" << endl;*/
  FMMMLayout* fmmm = new FMMMLayout();
  fmmm->randSeed(rand());
  //fmmm.minDistCC(150);
  //fmmm.fixedIterations(1);
  //fmmm.fineTuningIterations(0);
  fmmm->useHighLevelOptions(true);
  //fmmm.unitEdgeLength(1.0*2);
  fmmm->newInitialPlacement(true);
  //fmmm.qualityVersusSpeed(FMMMLayout::qvsGorgeousAndEfficient);
  fmmm->repForcesStrength(3);
  fmmm->postStrengthOfRepForces(0.2);
  //fmmm->call(GA, edgeLengths);

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
  
  
  StressMinimization *sm = new StressMinimization(); //Favorit!
  sm->useEdgeCostsAttribute(true);
  sm->layoutComponentsSeparately(true);
  sm->setIterations(100);
  //sm.call(GA);
  //TODO: TargetRatio des Packers anpassen
  VizComponentSplitterLayout compLayouter;//(m_hasEdgeCostsAttribute);
  compLayouter.setRatio(viewWidth/viewHeight); // * 1.3);
  compLayouter.setLayoutModule(sm);
  //compLayouter.setLayoutModule(fmmm);
  compLayouter.call(GA);
  
  
  //cout << GA.boundingBox().p1() << endl;
  //cout << GA.boundingBox().p2() << endl;

  //double p_xmul = view->viewport()->width() / (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  //double p_ymul = view->viewport()->height() / (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_x);
  GA.scale(2.0);
  double p_xmul = viewWidth / (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  double p_ymul = viewHeight / (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_y);
  double scale = min(p_xmul, p_ymul);
  //GA.scale(scale*1.0);
  //scale *= 0.5;
  view->scale(scale,scale);
  
  //cout << view->viewport()->width() << " x " << view->viewport()->height() << endl;
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
  double p_maxnodes = 10;
  unsigned long p_longestNode = 1;
  for (auto it : gfa_segments) {
    p_longestNode = max(p_longestNode, it.second->getLength());
  }
  settings.basesPerNode = p_longestNode / p_maxnodes;
}

VizNode* VizGraph::getNode(GfaSegment *seg) const {
  NodeMap::const_iterator it = nodes.find(seg);
  if(it != nodes.end())
    return it->second;
  else
    return NULL;
}

VizEdge* VizGraph::getEdge(GfaEdge *edge) const {
  EdgeMap::const_iterator it = edges.find(edge);
  if(it != edges.end())
    return it->second;
  else
    return NULL;
}

VizFragment* VizGraph::getFragment(GfaFragment* fragment) const {
  FragmentMap::const_iterator it = fragments.find(fragment);
  if(it != fragments.end())
    return it->second;
  else
    return NULL;
}

VizGap* VizGraph::getGap(GfaGap* gap) const {
  GapMap::const_iterator it = gaps.find(gap);
  if(it != gaps.end())
    return it->second;
  else
    return NULL;
}
QPointF VizGraph::getNodePos(node n) {
  return QPointF(GA.x(n), GA.y(n));
}

void VizGraph::renderToFile(QString filename, QString format) {
  /*QPixmap pixMap = view->grab();
  pixMap.save(filename);*/
  if (format.toUpper() == "SVG") {
    QSvgGenerator svgGen;
    svgGen.setFileName(filename + "." + format);
    svgGen.setSize(QSize(viewWidth, viewHeight));
    svgGen.setViewBox(QRect(0, 0, viewWidth, viewHeight));
    svgGen.setTitle(tr("SVG Generator Example Drawing"));
    svgGen.setDescription(tr("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt."));
    QPainter painter( &svgGen );
    scene->render( &painter );
  } else {
    QPixmap pixMap = view->grab();
    pixMap.save(filename + "." + format);
  }
}

void VizGraph::zoomIn() {
  view->scale(1.5,1.5);
}

void VizGraph::zoomOut() {
  view->scale(0.666,0.666);
}

void VizGraph::zoomDefault() {
  view->fitInView(GA.boundingBox().p1().m_x,
                  GA.boundingBox().p1().m_y,
                  GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x,
                  GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_y,
                  Qt::KeepAspectRatio);
}
