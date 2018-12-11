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
#include "ui/ui_graphwidget.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
//#include <ogdf/fileformats/GraphIO.h>

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QElapsedTimer>


using namespace ogdf;

typedef unordered_map<GfaLine*,VizNode*> NodeMap;
typedef unordered_map<GfaLine*,VizEdge*> EdgeMap;
typedef unordered_map<GfaLine*,VizGap*> GapMap;
typedef unordered_map<GfaLine*,VizGroup*> GroupMap;
typedef unordered_map<GfaLine*,VizFragment*> FragmentMap;
typedef unordered_map<GfaLine*,VizElement*> GeneralMap;

class VizLayout;

class VizStyleSetting {
  public:
    VizStyleSetting() {};
    VizStyleSetting(QObject *w, VizElementType t, VizGraphParam p, bool g=false) {
      widget = w;
      targetType = t;
      targetParam = p;
      global = g;
    }
    QObject* widget;
    VizElementType targetType;
    VizGraphParam targetParam;    
    bool global;
};
typedef unordered_map<QObject*, VizStyleSetting> StyleMap;


class VizGraph : public QWidget {
  //Q_OBJECT
  public:
    VizGraph(QWidget *parent = 0);
    ~VizGraph();
    void init(const QString& filename, const VizAppSettings& appSettings);
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
    void initOgdf();
    void saveGFA(QString filename, GfaVersion version, bool savestyle, bool savelayout);
    void renderToFile(QString filename, QString format, int w, int h, bool transparency=false);
    
    QPointF getNodePos(node n);
    
    void setHasLayout(bool value);
    
    void selectAll();
    void selectNone();
    void selectInvert();

  public slots:
    void cancelLayout();
    void layoutProgress(double value);
    void selectionChanged();
    void adaptStyleTabToSelection();
    void zoomIn();
    void zoomOut();
    void zoomDefault();
    void search();
    void loadStyleDialog();
    void saveStyleDialog();
    
    void layoutAlgorithmChanged(int index);
    void layoutApplyButtonPressed();
  
    void styleChanged();
    void treeViewItemExpanded(QTreeWidgetItem*);
    void treeViewItemDblClicked(QTreeWidgetItem*, int);
    
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
    /*NodeMap nodes;
    EdgeMap edges;
    GapMap gaps;
    GroupMap groups;
    FragmentMap fragments;*/
    GeneralMap elements[VIZ_ELEMENTUNKNOWN];
    
    set<VizElement*> selectedElems[VIZ_ELEMENTUNKNOWN];
    void setStyleTabEnabled(VizElementType t, bool value);

    
    Ui::GraphWidget form;
    VizGraphicsView* view;
    int viewWidth, viewHeight;
    
    void addStyleSetting(QObject *w, VizElementType t, VizGraphParam p, bool global=false);
    StyleMap styleSettings;
    
    void fillTreeView();
    void updateTreeViewToSelection();
    VizTreeItem* treeViewCurrentSelectionItem;
    
    
    void applyLayout(VizLayout* layout, double ratio, bool fromGui=false);
    VizLayout* currentLayout;
    VizLayout* activeLayout;
    vector<VizLayout*> layouts;
    void initLayouts();
    QElapsedTimer layouttimer;
    
    bool hasLayout;
};
