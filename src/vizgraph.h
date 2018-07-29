#pragma once
#include "headers.h"

#include <set>

#include "gfa/graph.h"
#include "gfa/edge.h"

#include "vizedge.h"
#include "viznode.h"
#include "vizgap.h"
#include "vizgroup.h"
#include "vizfragment.h"
#include "vizsettings.h"
#include "ui_graphwidget.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QGraphicsSceneMouseEvent>


using namespace ogdf;

typedef unordered_map<GfaSegment*,VizNode*> NodeMap;
typedef unordered_map<GfaEdge*,VizEdge*> EdgeMap;
typedef unordered_map<GfaGap*,VizGap*> GapMap;
typedef unordered_map<GfaGroup*,VizGroup*> GroupMap;
typedef unordered_map<GfaFragment*,VizFragment*> FragmentMap;

class VizStyleSetting {
  public:
    VizStyleSetting() {};
    VizStyleSetting(QWidget *w, VizElementType t, VizGraphParam p) {
      widget = w;
      targetType = t;
      targetParam = p;
    }
    QWidget* widget;
    VizElementType targetType;
    VizGraphParam targetParam;    
};

class VizGraph : public QWidget {
  //Q_OBJECT
  public:
    VizGraph(const QString& filename, const VizAppSettings& appSettings, QWidget *parent = 0);
    void setDisplaySize(unsigned int width, unsigned int height);
    //~VizGraph();
    
    Graph G;
    GraphAttributes GA;
    EdgeArray<double> edgeLengths;
    
    VizGraphSettings settings;
    
    //QGraphicsScene *scene;
    QGraphicsScene* scene;
    
    VizElement* getElement(GfaLine* line) const;
    VizNode* getNode(GfaSegment* seg) const;
    VizEdge* getEdge(GfaEdge* edge) const;
    VizFragment* getFragment(GfaFragment* fragment) const;
    VizGap* getGap(GfaGap* gap) const;
    VizGroup* getGroup(GfaGroup* group) const;
    const NodeMap& getNodes() const;
    const EdgeMap& getEdges() const;
    const GapMap& getGaps() const;
    const GroupMap& getGroups() const;
    const FragmentMap& getFragments() const;
    
    void draw();
    void renderToFile(QString filename, QString format);
    
    QPointF getNodePos(node n);

  public slots:
    void selectionChanged();
    void zoomIn();
    void zoomOut();
    void zoomDefault();
    void search();
    void loadStyleDialog();
    void saveStyleDialog();
  
    void styleChanged();
    
  private:
    void addNode(GfaSegment* seg);
    void addEdge(GfaEdge* edge);
    void addGap(GfaGap* gap);
    void addGroup(GfaGroup* group);
    void addFragment(GfaFragment* fragment);
    void determineParams();
    void calcLayout();
    
    void setCacheMode(QGraphicsItem::CacheMode mode);
  
    GfaGraph* gfa;
    NodeMap nodes;
    EdgeMap edges;
    GapMap gaps;
    GroupMap groups;
    FragmentMap fragments;
    
    vector<set<VizElement*>> selectedElems;
    void setStyleTabEnabled(VizElementType t, bool value);
    
    Ui::GraphWidget form;
    QGraphicsView* view;
    int viewWidth, viewHeight;
    
    void addStyleSetting(QWidget *w, VizElementType t, VizGraphParam p);
    unordered_map<QWidget*, VizStyleSetting> styleSettings;
};
