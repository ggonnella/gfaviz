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




VizGraph::VizGraph(char *filename, QGraphicsView *_view) {
  QElapsedTimer timer;
  timer.start();
  
  view = _view;
  
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

  double p_xmul = view->viewport()->width() / (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  double p_ymul = view->viewport()->height() / (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_x);
  double scale = min(p_xmul, p_ymul);
  GA.scale(scale*1.0);
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
  
  cout << "drawing nodes... ";
  for (auto it : nodes) {
    it.second->draw();
  }
  cout << "done!" << endl;
  
}

VizGraph::~VizGraph() {
  
}

void VizGraph::addNode(GfaSegment* seg) {
  nodes[seg] = new VizNode(seg, this);
}

void VizGraph::addEdge(GfaEdge* edge) {
  edges[edge] = new VizEdge(edge, this);
}

void VizGraph::addGap(GfaGap* gap) {
  gaps[gap] = new VizGap(gap, this);
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

typedef struct {
  double x;
  double y;
} vec2d;

double dist(node n1, node n2, const GraphAttributes& GA) {
  double dx = GA.x(n1)-GA.x(n2);
  double dy = GA.y(n1)-GA.y(n2);
  return sqrt(dx*dx+dy*dy);
}

double length(vec2d v) {
  return sqrt(v.x*v.x+v.y*v.y);
}
vec2d normalize(vec2d v) {
  double l = length(v);
  return (vec2d){v.x/l, v.y/l};
}

void VizGraph::ownLayout() {
  NodeArray<vec2d> nodeForces(G);
  for (auto it : nodes) {
    for (node n : it.second->ogdf_nodes) {
      nodeForces[n].x = 0;
      nodeForces[n].y = 0;
    }
  }
  for (int iter = 0; iter < 200; iter++) {
    for (auto it : nodes) {
      for (size_t idx = 0; idx < it.second->ogdf_nodes.size(); idx++) {
        node n = it.second->ogdf_nodes[idx];
        if (idx > 0) {
          node n0 = it.second->ogdf_nodes[idx-1];
          vec2d d = (vec2d){GA.x(n0)-GA.x(n), GA.y(n0)-GA.y(n)};
          vec2d dn = normalize(d);          
          double ddist = 15.0 - length(d);
          if (fabs(ddist) > 0.5) {
            cout << ddist << endl;
            nodeForces[n].x += dn.x * ddist*ddist * 0.3;
            nodeForces[n].y += dn.y * ddist*ddist * 0.3;
          }
        }
        if (idx < it.second->ogdf_nodes.size() - 1) {
          node n0 = it.second->ogdf_nodes[idx+1];
          vec2d d = (vec2d){GA.x(n0)-GA.x(n), GA.y(n0)-GA.y(n)};
          vec2d dn = normalize(d);          
          double ddist = 15.0 - length(d);
          if (fabs(ddist) > 0.5) {
            cout << "xy: " << GA.x(n0) << " " << GA.x(n) << "; " << GA.y(n0)  << " " << GA.y(n) << endl;
            cout << d.x << " " << d.y << " " << ddist << endl;
            nodeForces[n].x += dn.x * ddist*ddist * 0.3;
            nodeForces[n].y += dn.y * ddist*ddist * 0.3;
          }
        }
      }
    }
    
    double mindist = 100;
    for (auto it : nodes) {
      for (node n : it.second->ogdf_nodes) {
        for (auto it2 : nodes) {
          if (it2.second == it.second) {
            //continue;
          }
          for (node n0 : it2.second->ogdf_nodes) {
            if (n == n0) {
              continue;
            }
            vec2d d = (vec2d){GA.x(n0)-GA.x(n), GA.y(n0)-GA.y(n)};
            double ddist = length(d);
            if (ddist < mindist) {
              double strength = sqrt(1.0-(ddist/mindist));
              vec2d dn = normalize(d);
              nodeForces[n].x -= dn.x * strength * 2.0;
              nodeForces[n].y -= dn.y * strength * 2.0;
              cout << nodeForces[n].x << endl;
            }
            
            
          }
        }
      }
    }
    
    
    for (auto it : nodes) {
      for (node n : it.second->ogdf_nodes) {
        double l = length(nodeForces[n]);
        if (l > 10.0) {
          nodeForces[n].x *= (10.0/l);
          nodeForces[n].y *= (10.0/l);
        }
        nodeForces[n].x *= 0.8;
        nodeForces[n].y *= 0.8;
        GA.x(n) += nodeForces[n].x;
        GA.y(n) += nodeForces[n].y;
      }
    }
  }
  
  
}
