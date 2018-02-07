#pragma once
#include "headers.h"

#include "gfa/graph.h"
#include "gfa/edge.h"

#include "vizedge.h"
#include "viznode.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>

#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QGraphicsSceneMouseEvent>


using namespace ogdf;

typedef unordered_map<GfaSegment*,VizNode*> NodeMap;
typedef unordered_map<GfaEdge*,VizEdge*> EdgeMap;

typedef struct {
  double basesPerNode = 1;
} VizSettings;

class VizGraph {
  public:
    VizGraph(char *filename, QGraphicsView *view);
    ~VizGraph();
    
    Graph G;
    GraphAttributes GA;
    EdgeArray<double> edgeLengths;
    
    VizSettings settings;
    
    QGraphicsScene *scene;
    
    VizNode* getNode(GfaSegment *seg);
    void draw();

  private:
    void addNode(GfaSegment* seg);
    void addEdge(GfaEdge* edge);
    void determineParams();
    void calcLayout();
  
    GfaGraph* gfa;
    NodeMap nodes;
    EdgeMap edges;
    
    QGraphicsView *view;
};
