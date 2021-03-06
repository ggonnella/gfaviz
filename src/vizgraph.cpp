#include "vizgraph.h"
#include <vizStressMinimization.h>
#include <vizComponentSplitterLayout.h>

#include <vizlayout.h>

#ifndef NOSVG
  #include <QSvgGenerator>
#endif
//#include <QSvg>

#include <QFileInfo>
#include <QJsonDocument>
#include <QFileDialog>

#include "ui/ui_layoutProgressOverlay.h"
//#include <ogdf/fileformats/GraphIO.h>

//todo: http://amber-v7.cs.tu-dortmund.de/doku.php/gsoc2013-ideas Node Overlap Removal (Noverlap in gephi)
//todo: https://forum.qt.io/topic/38631/dragging-a-tab-from-qtabwidget-into-a-separate-window/5

VizGraph::VizGraph(QWidget *parent) : QWidget(parent) {
  form.setupUi(this);
}
VizGraph::~VizGraph() {
  delete gfa;
}
void VizGraph::init(const QString& filename, const VizAppSettings& appSettings) {
  //GeneralMap map = edges;
  
  cout << "Opening " << filename.toStdString() << "..." << endl;
  //elements.resize(VIZ_ELEMENTUNKNOWN);
  //selectedElems.resize(VIZ_ELEMENTUNKNOWN);
  layouttimer.restart();
  scene = NULL;
  activeLayout = NULL;
  hasLayout = true;
  settings = appSettings.graphSettings;
  settings.filename = filename;
  viewWidth = appSettings.width;
  viewHeight = appSettings.height;
  setObjectName(QStringLiteral("tab"));
  connect(form.ButtonZoomIn, &QPushButton::clicked, this, &VizGraph::zoomIn);
  connect(form.ButtonZoomOut, &QPushButton::clicked, this, &VizGraph::zoomOut);
  connect(form.ButtonZoomDefault, &QPushButton::clicked, this,
      &VizGraph::zoomDefault);
  connect(form.SearchButton, &QPushButton::clicked, this, &VizGraph::search);
  connect(form.SearchName, SIGNAL(returnPressed()),form.SearchButton,
      SIGNAL(clicked()));
  connect(form.StyleLoadButton, &QPushButton::clicked, this,
      &VizGraph::loadStyleDialog);
  connect(form.StyleSaveButton, &QPushButton::clicked, this,
      &VizGraph::saveStyleDialog);
  connect(form.LayoutAlgorithmCombobox, static_cast<void (QComboBox::*)(int)>(
        &QComboBox::currentIndexChanged), this, &VizGraph::layoutAlgorithmChanged);
  connect(form.LayoutApplyButton, &QPushButton::clicked, this,
      &VizGraph::layoutApplyButtonPressed);
  connect(form.LayoutProgressCancel, &QPushButton::clicked, this,
      &VizGraph::cancelLayout);

  connect(form.styleApplyToSelectedCheckbox, &QCheckBox::stateChanged, this,
      &VizGraph::adaptStyleTabToSelection);

  #define addStyleLabelWidget(widget,type,prefix) \
    addStyleSetting(widget->form.Show, type, prefix ## LABELSHOW); \
    addStyleSetting(widget->form.Font, type, prefix ## LABELFONT); \
    addStyleSetting(widget->form.Size, type, prefix ## LABELFONTSIZE); \
    addStyleSetting(widget->form.Color, type, prefix ## LABELCOLOR); \
    addStyleSetting(widget->form.OutlineWidth, type, prefix ## LABELOUTLINEWIDTH); \
    addStyleSetting(widget->form.OutlineColor, type, prefix ## LABELOUTLINECOLOR); \

  addStyleSetting(form.styleSegShow, VIZ_SEGMENT, VIZ_DISABLESEGMENTS);
  addStyleSetting(form.styleSegWidth, VIZ_SEGMENT, VIZ_SEGMENTWIDTH);
  addStyleSetting(form.styleSegColor, VIZ_SEGMENT, VIZ_SEGMENTMAINCOLOR);
  addStyleSetting(form.styleSegOutlineWidth, VIZ_SEGMENT, VIZ_SEGMENTOUTLINEWIDTH);
  addStyleSetting(form.styleSegOutlineColor, VIZ_SEGMENT, VIZ_SEGMENTOUTLINECOLOR);
  addStyleSetting(form.styleSegArrowEnabled, VIZ_SEGMENT, VIZ_SEGMENTASARROW);
  addStyleLabelWidget(form.styleSegLabel, VIZ_SEGMENT, VIZ_SEG);
  
  addStyleSetting(form.styleEdgeShow, VIZ_EDGE, VIZ_DISABLEEDGES);
  addStyleSetting(form.styleEdgeWidth, VIZ_EDGE, VIZ_EDGEWIDTH);
  addStyleSetting(form.styleEdgeColor, VIZ_EDGE, VIZ_EDGECOLOR);
  addStyleSetting(form.styleEdgeHighlightsColor, VIZ_EDGE, VIZ_EDGEHIGHLIGHTCOLOR);
  addStyleSetting(form.styleEdgeHighlightsShow, VIZ_EDGE, VIZ_EDGEHIGHLIGHTSHOW);
  addStyleLabelWidget(form.styleEdgeLabel, VIZ_EDGE, VIZ_EDGE);
  
  addStyleSetting(form.styleGroupShow, VIZ_GROUP, VIZ_DISABLEGROUPS);
  addStyleSetting(form.styleGroupColor, VIZ_GROUP, VIZ_GROUPCOLOR);
  addStyleSetting(form.styleGroupWidth, VIZ_GROUP, VIZ_GROUPWIDTH);
  addStyleLabelWidget(form.styleGroupLabel, VIZ_GROUP, VIZ_GROUP);
  
  addStyleSetting(form.styleGapShow, VIZ_GAP, VIZ_DISABLEGAPS);
  addStyleSetting(form.stylePosGapColor, VIZ_GAP, VIZ_POSGAPCOLOR);
  addStyleSetting(form.styleNegGapColor, VIZ_GAP, VIZ_NEGGAPCOLOR);
  addStyleSetting(form.styleGapWidth, VIZ_GAP, VIZ_GAPWIDTH);
  addStyleLabelWidget(form.styleGapLabel, VIZ_GAP, VIZ_GAP);
  
  addStyleSetting(form.styleFragmentShow, VIZ_FRAGMENT, VIZ_DISABLEFRAGMENTS);
  addStyleSetting(form.styleFwdFragmentColor, VIZ_FRAGMENT, VIZ_FWDFRAGMENTCOLOR);
  addStyleSetting(form.styleRevFragmentColor, VIZ_FRAGMENT, VIZ_REVFRAGMENTCOLOR);
  addStyleSetting(form.styleFragmentWidth, VIZ_FRAGMENT, VIZ_FRAGMENTWIDTH);
  addStyleSetting(form.styleFragmentConnColor, VIZ_FRAGMENT, VIZ_FRAGMENTCONNCOLOR);
  addStyleSetting(form.styleFragmentConnWidth, VIZ_FRAGMENT, VIZ_FRAGMENTCONNWIDTH);
  addStyleSetting(form.styleFragmentHighlightsColor, VIZ_FRAGMENT, VIZ_FRAGMENTHIGHLIGHTCOLOR);
  addStyleSetting(form.styleFragmentHighlightsShow, VIZ_FRAGMENT, VIZ_FRAGMENTHIGHLIGHTSHOW);
  addStyleLabelWidget(form.styleFragmentLabel, VIZ_FRAGMENT, VIZ_FRAGMENT);
  
  
  addStyleLabelWidget(form.styleAllLabels, VIZ_ELEMENTUNKNOWN, VIZ_);
  
  addStyleSetting(form.LayoutParamLengthDovetail, VIZ_ELEMENTUNKNOWN, VIZ_DOVETAILLENGTH, true);
  addStyleSetting(form.LayoutParamLengthInternal, VIZ_ELEMENTUNKNOWN, VIZ_INTERNALLENGTH, true);
  addStyleSetting(form.LayoutParamGapsAsEdges, VIZ_ELEMENTUNKNOWN, VIZ_GAPSEDGES, true);
  addStyleSetting(form.LayoutParamLengthGaps, VIZ_ELEMENTUNKNOWN, VIZ_GAPLENGTH, true);
  addStyleSetting(form.LayoutParamFragmentDist, VIZ_ELEMENTUNKNOWN, VIZ_FRAGMENTDIST, true);
  addStyleSetting(form.LayoutParamMaxSubnodes, VIZ_ELEMENTUNKNOWN, VIZ_SEGMENTMAXSUB, true);
  addStyleSetting(form.LayoutParamMinWeight, VIZ_ELEMENTUNKNOWN, VIZ_MINWEIGHT, true);
  addStyleSetting(form.LayoutParamWeightFactor, VIZ_ELEMENTUNKNOWN, VIZ_WEIGHTFACTOR, true);
  
  
  view = form.vizCanvas;
  //view->setEnabled(false);
  //view->setObjectName(QStringLiteral("vizCanvas"));
  view->setGeometry(QRect(0, 0, viewWidth+2, viewHeight+2));
  view->setDragMode(QGraphicsView::RubberBandDrag); //QGraphicsView::ScrollHandDrag);
  //view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scene = new QGraphicsScene();
  connect(scene, &QGraphicsScene::selectionChanged, this, &VizGraph::selectionChanged);
  view->setRenderHints(QPainter::Antialiasing);
  
  
  QElapsedTimer timer;
  timer.start();
  
  gfa = new GfaGraph();
  gfa->open(qPrintable(filename));
  //gfa->print();
  if (gfa->hasTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)) {
    char* styledata = gfa->getTag(VIZ_OPTIONSTAG, GFA_TAG_JSON)->getStringValue();
    QJsonDocument jsondata = QJsonDocument::fromJson(styledata);
    if (!jsondata.isNull()) {
      settings.fromJson(jsondata.object());
    }
  }

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
  fillTreeView();
  initOgdf();
  scene->blockSignals(true);
  if (hasLayout) {
    draw();
    form.vizCanvasOverlay->hide();
  } else {
    calcLayout();
  }
  view->setScene(scene);
  hasLayout = true;
  scene->blockSignals(false);

  initLayouts();
  //GraphIO::writeGML(GA, QString(filename + ".gml").toStdString());

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
    renderToFile(outputFile, appSettings.outputFormat, appSettings.width,
                 appSettings.height, appSettings.transparency);
  }

  cout << "Finished after " << timer.elapsed() << " milliseconds." << endl;
}
void VizGraph::initOgdf() {
  G.clear();
  GA = GraphAttributes(G, GraphAttributes::nodeGraphics |
                          GraphAttributes::edgeGraphics |
                          GraphAttributes::edgeDoubleWeight );
  edgeLengths = EdgeArray<double>(G);
  determineParams();
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    for (auto it : elements[idx]) {
      it.second->initOgdf();
    }
  }
}

void VizGraph::setDisplaySize(unsigned int width, unsigned int height) {
  double p_xmul = (double)width / (double)viewWidth;
  double p_ymul = (double)height / (double)viewHeight;
  double scale = min(p_xmul, p_ymul);
  view->scale(scale,scale);
  view->setGeometry(QRect(0, 0, width+2, height+2));
}

void VizGraph::layoutProgress(double value) {
  form.LayoutProgressBar->setValue(value*10000);
  if (layouttimer.elapsed() > 100) {
    if (hasLayout)
      QCoreApplication::processEvents();
    layouttimer.restart();
  }
}
void VizGraph::calcLayout() {
  //cout << "Calculating Layout" << endl;
  VizLayout* layout;
  if (settings.get(VIZ_USEFMMM).toBool()) {
    layout = new VizLayoutFMMM(this);
  } else {
    layout = new VizLayoutSM(this);
  }
  applyLayout(layout,(double)viewWidth/(double)viewHeight);
  delete layout;
}

void VizGraph::draw() {
  /*GA.scale(2.0);*/
  view->resetMatrix();
  view->resetTransform();
  double p_xmul = view->size().width() /
                  (GA.boundingBox().p2().m_x - GA.boundingBox().p1().m_x);
  double p_ymul = view->size().height() /
                  (GA.boundingBox().p2().m_y - GA.boundingBox().p1().m_y);
  double scale = min(p_xmul, p_ymul) * 0.9;
  view->scale(scale,scale);
  //cout << GA.boundingBox().p2().m_x << " " << GA.boundingBox().p1().m_x << endl;

  scene->setBackgroundBrush(QBrush(
           settings.get(VIZ_BACKGROUNDCOLOR).value<QColor>()));

  //cout << "drawing gaps... " << endl;
  for (auto it : getGaps()) {
    it.second->draw();
  }
  //cout << "done!" << endl;

  //cout << "drawing edges... " << endl;
  for (auto it : getEdges()) {
    it.second->draw();
  }
  //cout << "done!" << endl;

  //cout << "drawing fragments... " << endl;
  for (auto it : getFragments()) {
    it.second->draw();
  }
  //cout << "done!" << endl;

  //cout << "drawing nodes... "  << endl;
  for (auto it : getNodes()) {
    it.second->draw();
    it.second->setZValue(1.0);
  }
  //cout << "done!" << endl;

  //cout << "drawing groups... " << endl;
  double epsilon = 0.01;
  int group_n=0;
  for (auto it : getGroups()) {
    it.second->setIndex(group_n);
    group_n++;
    it.second->draw();
    it.second->setZValue(-1.0 - epsilon);
    if (it.second->labelItem)
      it.second->labelItem->setZValue(-1.0 - epsilon);
    epsilon += 0.01;
  }
  //cout << "done!" << endl;
  selectionChanged(); //To disable non-needed style tabs

  scene->setSceneRect(scene->itemsBoundingRect());
  view->setSceneRect(QRectF());
}

/*VizGraph::~VizGraph() {

}*/

void VizGraph::addNode(GfaSegment* seg) {
  elements[VIZ_SEGMENT][seg] = new VizNode(seg, this);
}

void VizGraph::addEdge(GfaEdge* edge) {
  elements[VIZ_EDGE][edge] = new VizEdge(edge, this);
}

void VizGraph::addGap(GfaGap* gap) {
  elements[VIZ_GAP][gap] = new VizGap(gap, this);
}

void VizGraph::addGroup(GfaGroup* group) {
  elements[VIZ_GROUP][group] = new VizGroup(group, this);
}

void VizGraph::addFragment(GfaFragment* fragment) {
  elements[VIZ_FRAGMENT][fragment] = new VizFragment(fragment, this);
}

void VizGraph::determineParams() {
  const GfaSegmentMap& gfa_segments = gfa->getSegments();
  double p_maxsub = settings.get(VIZ_SEGMENTMAXSUB).toInt();
  unsigned long p_longestSegment = 1;
  for (auto it : gfa_segments) {
    p_longestSegment = max(p_longestSegment, it.second->getLength());
  }
  settings.basesPerSubseg = p_longestSegment / p_maxsub;
}

VizNode* VizGraph::getNode(GfaSegment *seg) const {
  NodeMap::const_iterator it = getNodes().find(seg);
  if(it != getNodes().end())
    return it->second;
  else
    return NULL;
}

VizEdge* VizGraph::getEdge(GfaEdge *edge) const {
  EdgeMap::const_iterator it = getEdges().find(edge);
  if(it != getEdges().end())
    return it->second;
  else
    return NULL;
}

VizFragment* VizGraph::getFragment(GfaFragment* fragment) const {
  FragmentMap::const_iterator it = getFragments().find(fragment);
  if(it != getFragments().end())
    return it->second;
  else
    return NULL;
}

VizGap* VizGraph::getGap(GfaGap* gap) const {
  GapMap::const_iterator it = getGaps().find(gap);
  if(it != getGaps().end())
    return it->second;
  else
    return NULL;
}

VizGroup* VizGraph::getGroup(GfaGroup* group) const{
  GroupMap::const_iterator it = getGroups().find(group);
  if(it != getGroups().end())
    return it->second;
  else
    return NULL;
}

VizElement* VizGraph::getElement(GfaLine* line) const {
  switch(line->getType()) {
    case GFA_SEGMENT:
      return getNode((GfaSegment*)line);
    case GFA_EDGE:
      return getEdge((GfaEdge*)line);
    case GFA_FRAGMENT:
      return getFragment((GfaFragment*)line);
    case GFA_GAP:
      return getGap((GfaGap*)line);
    case GFA_GROUP:
      return getGroup((GfaGroup*)line);
    default:
      return NULL;
  }
  return NULL;
}
const NodeMap& VizGraph::getNodes() const {
  const NodeMap* ret = (const NodeMap*)&elements[VIZ_SEGMENT];
  return *ret;
}
const EdgeMap& VizGraph::getEdges() const {
  const EdgeMap* ret = (const EdgeMap*)&elements[VIZ_EDGE];
  return *ret;
}
const GapMap& VizGraph::getGaps() const {
  const GapMap* ret = (const GapMap*)&elements[VIZ_GAP];
  return *ret;
}
const GroupMap& VizGraph::getGroups() const {
  const GroupMap* ret = (const GroupMap*)&elements[VIZ_GROUP];
  return *ret;
}
const FragmentMap& VizGraph::getFragments() const {
  const FragmentMap* ret = (const FragmentMap*)&elements[VIZ_FRAGMENT];
  return *ret;
}

QPointF VizGraph::getNodePos(node n) {
  return QPointF(GA.x(n), GA.y(n));
}

void VizGraph::renderToFile(QString filename, QString format,
                            int w, int h, bool transparency) {
#ifndef NOSVG
  if (format.toUpper() == "SVG") {
    setCacheMode(QGraphicsItem::NoCache);
    QSvgGenerator svgGen;
    svgGen.setFileName(filename + "." + format);
    svgGen.setSize(QSize(w, h));
    svgGen.setViewBox(QRect(0, 0, w, h));
    svgGen.setTitle(tr("SVG Generator Example Drawing"));
    svgGen.setDescription(tr("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt."));
    QPainter painter( &svgGen );
    scene->render( &painter );
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  } else {
#endif
    QPixmap pixMap(w,h);
    if (format.toUpper() == "PNG" && transparency)
      pixMap.fill(Qt::transparent);
    else
      pixMap.fill(Qt::white);
    QPainter painter( &pixMap );
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);
    scene->render( &painter );
    pixMap.save(filename + "." + format, qUtf8Printable(format));
#ifndef NOSVG
  }
#endif
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
  view->scale(0.9,0.9);
}

void VizGraph::search() {
  QStringList names = form.SearchName->text().split(" ");
  for (const auto& name : names)
  {
    GfaLine* line = gfa->getLine(name.toStdString());
    if (line == NULL) {
      showMessage("Element \"" + name + "\" could not be found!");
      continue;
    }
    VizElement* elem = getElement(line);
    if (elem->isSelected()) {
      showMessage("Element \"" + name + "\" was already selected!");
      continue;
    }
    elem->setSelected(true);
  }
}

void VizGraph::setCacheMode(QGraphicsItem::CacheMode mode) {
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    for (auto it : elements[idx]) {
      it.second->setCacheMode(mode);
      if (it.second->labelItem)
        it.second->labelItem->setCacheMode(mode);
    }
  }
  /*for (auto it : gaps) {
    it.second->setCacheMode(mode);
    if (it.second->labelItem)
      it.second->labelItem->setCacheMode(mode);
  } 
  for (auto it : edges) {
    it.second->setCacheMode(mode);
    if (it.second->labelItem)
      it.second->labelItem->setCacheMode(mode);
  }
  for (auto it : getFragments()) {
    it.second->setCacheMode(mode);
    if (it.second->labelItem)
      it.second->labelItem->setCacheMode(mode);
  }
  for (auto it : nodes) {
    it.second->setCacheMode(mode);
    if (it.second->labelItem)
      it.second->labelItem->setCacheMode(mode);
  }*/
}

void VizGraph::setStyleTabEnabled(VizElementType t, bool value) {
  if (t == VIZ_SEGMENT) {
    form.StyleTabSegments->setEnabled(value);
    form.StyleTabWidget->setTabEnabled(0,value);
  } else if (t == VIZ_EDGE) {
    form.StyleTabEdges->setEnabled(value);
    form.StyleTabWidget->setTabEnabled(1,value);
  } else if (t == VIZ_GROUP) {
    form.StyleTabGroups->setEnabled(value);
    form.StyleTabWidget->setTabEnabled(2,value);
  } else if (t == VIZ_GAP) {
    form.StyleTabGaps->setEnabled(value);
    form.StyleTabWidget->setTabEnabled(3,value);
  } else if (t == VIZ_FRAGMENT) {
    form.StyleTabFragments->setEnabled(value);
    form.StyleTabWidget->setTabEnabled(4,value);
  }
}

void VizGraph::adaptStyleTabToSelection() {
  QList<QGraphicsItem*> items = scene->selectedItems();

  if (items.size() == 0 || !form.styleApplyToSelectedCheckbox->isChecked()) {
    form.StyleLoadButton->setEnabled(true);
    form.StyleSaveButton->setEnabled(true);
    setStyleTabEnabled(VIZ_SEGMENT, (getNodes().size() > 0));
    setStyleTabEnabled(VIZ_EDGE, (getEdges().size() > 0));
    setStyleTabEnabled(VIZ_GROUP, (getGroups().size() > 0));
    setStyleTabEnabled(VIZ_GAP, (getGaps().size() > 0));
    setStyleTabEnabled(VIZ_FRAGMENT, (getFragments().size() > 0));
    return;
  }
  form.StyleLoadButton->setEnabled(false);
  form.StyleSaveButton->setEnabled(false);
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    if (selectedElems[idx].size() == 0) {
      setStyleTabEnabled((VizElementType)idx, false);
      continue;
    }
    setStyleTabEnabled((VizElementType)idx, true);
  }
}

void VizGraph::selectionChanged() {
  QList<QGraphicsItem*> items = scene->selectedItems();

  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    selectedElems[idx].clear();
  }
  for (QGraphicsItem* item : items) {
    VizElement* elem = (VizElement*)item;
    selectedElems[elem->getType()].insert(elem);
    //
    // The following code selects an entire group if
    // more than one element of the group is selected.
    // This was disabled after feedback from the reviewers.
    //
    //if (items.size() > 1) {
    //  for (VizGroup* group : elem->getGroups()) {
    //    selectedElems[VIZ_GROUP].insert(group);
    //    group->setSelected(true);
    //  }
    //}
    //cout << elem->getGfaElement()->getName() << endl;
  }

  // set values in GUI
  updateTreeViewToSelection();
  adaptStyleTabToSelection();
  for (auto it : styleSettings) {
    VizStyleSetting option = it.second;
    if ((option.targetType == VIZ_ELEMENTUNKNOWN) ||
        (items.size() == 0 && elements[option.targetType].size() > 0) ||
        (selectedElems[option.targetType].size() > 0)) {
      VizGraphParamAttrib param = VizGraphSettings::params[option.targetParam];
      QVariant value;
      if (option.targetType == VIZ_ELEMENTUNKNOWN) {
        value = settings.get(option.targetParam);
      } else if (form.styleApplyToSelectedCheckbox->isChecked()
                  && items.size()>0) {
        value = (*(selectedElems[option.targetType].begin()))->getOption(
                  option.targetParam);
      } else {
        value = (*(elements[option.targetType].begin())).second->getOption(
                  option.targetParam);
      }
      option.widget->blockSignals(true);
      if (param.type == QMetaType::Bool) {
        ((QCheckBox*)option.widget)->setChecked(value.toBool());
      } else if (param.type == QMetaType::QColor) {
        ((ColorButton*)option.widget)->setColor(value.value<QColor>());
      } else if (param.type == QMetaType::Double) {
        ((QDoubleSpinBox*)option.widget)->setValue(value.toDouble());
      } else if (param.type == QMetaType::UInt) {
        ((QSpinBox*)option.widget)->setValue(value.toUInt());
      } else if (param.type == QMetaType::QFont) {
        ((QFontComboBox*)option.widget)->setCurrentFont(QFont(value.toString()));
      } else {
        qCritical("Not implemented");
        return;
      }
      option.widget->blockSignals(false);
    }
  }
  if (items.size() == 0) {
    form.selectionDisplay->setHtml(
        "<b>Current selection:</b><br>No items selected.");
    return;
  }
  QString text = "<b>Current selection:</b><br>";
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    if (selectedElems[idx].size() == 0) {
      continue;
    }
    int unnamed = 0;
    int named = 0;
    text += "<b>" + VizElement::getTypeName((VizElementType)idx) + "s</b>: ";
    for (VizElement* elem : selectedElems[idx]) {
      if (elem->getGfaElement()->hasName()) {
        if (named > 0)
          text += ", ";
        text += QString::fromStdString(elem->getGfaElement()->getName());
        named++;
      } else {
        unnamed++;
      }
    }
    if (unnamed > 0) {
      if (named > 0)
        text += ", ";
      text += QString::number(unnamed) + " unnamed element";
      if (unnamed > 1)
        text += "s";
    }
    text += "<br>";
  }
  form.selectionDisplay->setHtml(text);
}

void VizGraph::loadStyleDialog() {
  QFileDialog dialog(parentWidget());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("GfaViz stylesheet (*.style)");
  if (dialog.exec()) {
    const QStringList& filenames = dialog.selectedFiles();
    if (filenames.size() > 0) {
      for (int idx = 0; idx < VIZ_ELEMENTUNKNOWN; idx++) {
        for (auto it : elements[idx]) {
          it.second->unsetAllOptions();
        }
      }
      settings.fromJsonFile(filenames[0]);
      draw();
    }
  }
}

void VizGraph::saveStyleDialog() {
  QFileDialog dialog(parentWidget());
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("style");
  dialog.setNameFilter("GfaViz stylesheet (*.style)");
  if (dialog.exec()) {
    const QStringList& filenames = dialog.selectedFiles();
    if (filenames.size() > 0) {
      QFile file(filenames[0]);
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Can't open file \"%s\" for writing!",
                 qUtf8Printable(filenames[0]));
        return;
      }
      QJsonDocument doc(settings.toJson());
      file.write(doc.toJson(QJsonDocument::Indented));
      file.close();
    }
  }
}

void VizGraph::addStyleSetting(QObject* w, VizElementType t, VizGraphParam p, bool global) {
  VizGraphParamAttrib param = VizGraphSettings::params[p];
  if (param.type == QMetaType::Bool) {
    connect((const QCheckBox*)w, &QCheckBox::stateChanged, this,
            &VizGraph::styleChanged);
  } else if (param.type == QMetaType::QColor) {
    connect((const ColorButton*)w, &ColorButton::valueChanged,
            this, &VizGraph::styleChanged);
  } else if (param.type == QMetaType::Double) {
    connect((const QDoubleSpinBox*)w,
            static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged), this, &VizGraph::styleChanged);
  } else if (param.type == QMetaType::UInt) {
    connect((const QSpinBox*)w, static_cast<void (QSpinBox::*)(int)>(
              &QSpinBox::valueChanged), this, &VizGraph::styleChanged);
  } else if (param.type == QMetaType::QFont) {
    connect((const QFontComboBox*)w, &QFontComboBox::currentFontChanged,
             this, &VizGraph::styleChanged);
  } else {
    qCritical("Not implemented");
    return;
  }
  styleSettings[w] = VizStyleSetting(w,t,p,global);
}

void VizGraph::styleChanged() {
  QVariant value;
  StyleMap::const_iterator it = styleSettings.find(sender());
  if(it == styleSettings.end()) {
    qWarning("Unknown GUI element");
    return;
  }
  VizStyleSetting style = it->second;
  
  VizGraphParamAttrib param = VizGraphSettings::params[style.targetParam];
  if (param.type == QMetaType::Bool) {
    value = ((QCheckBox*)sender())->isChecked();
  } else if (param.type == QMetaType::QColor) {
    value = ((ColorButton*)sender())->getColor();
  } else if (param.type == QMetaType::Double) {
    value = ((QDoubleSpinBox*)sender())->value();
  } else if (param.type == QMetaType::UInt) {
    value = ((QSpinBox*)sender())->value();
  } else if (param.type == QMetaType::QFont) {
    value = ((QFontComboBox*)sender())->currentFont();
  } else {
    qCritical("Not implemented");
    return;
  }
  if (style.global) {
    settings.set(style.targetParam, value, true);
  } else {
    for (int idx = (style.targetType != VIZ_ELEMENTUNKNOWN ? style.targetType : 0);
         idx < (style.targetType != VIZ_ELEMENTUNKNOWN ? style.targetType+1 : VIZ_ELEMENTUNKNOWN);
         idx++) {
      if (form.styleApplyToSelectedCheckbox->isChecked() && scene->selectedItems().size() > 0) {
        set<VizElement*> selected = selectedElems[idx]; //Workaround because selectedElems is changed when an item is set to invisible
        for (auto it : selected) {
          it->setOption(style.targetParam, value, true);
        }
      } else {
        for (auto it : elements[idx]) {
          it.second->unsetOption(style.targetParam);
        }
        settings.set(style.targetParam, value, true);
        for (auto it : elements[idx]) {
          it.second->draw();
        }
      }
    }
  }
  //cout << value.toString().toStdString() << endl;
}

void VizGraph::initLayouts() {
  currentLayout = NULL;
  layouts.push_back(new VizLayoutSM(this));
  layouts.push_back(new VizLayoutFMMM(this));
  for (size_t idx=0; idx < layouts.size(); idx++) {
    VizLayout* layout = layouts[idx];
    form.LayoutAlgorithmParams->addWidget(layout->getWidget());
    form.LayoutAlgorithmCombobox->addItem(layout->getName(), QVariant((uint)idx));
  }

}

void VizGraph::layoutAlgorithmChanged(int index) {
  if (currentLayout != NULL) {
    QSizePolicy::Policy policy = QSizePolicy::Ignored;
    currentLayout->getWidget()->setSizePolicy(policy,policy);
    currentLayout->getWidget()->adjustSize();
  }


  uint l_idx = form.LayoutAlgorithmCombobox->itemData(index).toUInt();
  currentLayout = layouts[l_idx];

  QSizePolicy::Policy policy = QSizePolicy::Expanding;
  currentLayout->getWidget()->setSizePolicy(policy,policy);
  currentLayout->getWidget()->adjustSize();
  form.LayoutAlgorithmParamsBox->adjustSize();

  form.LayoutAlgorithmInfo->setHtml("<b>Algorithm info:</b><br>" +
                                    currentLayout->getDescription());
  form.LayoutAlgorithmParams->setCurrentWidget(currentLayout->getWidget());
  //currentLayout->getWidget()->adjustSize();
  //form.LayoutAlgorithmParams->adjustSize();
  //form.LayoutAlgorithmParamsBox->adjustSize();
  //form.scrollAreaWidgetContents_layout->adjustSize();
  //currentLayout->getWidget()->setParent(form.groupBox_2);
  //currentLayout->getWidget()->show();
}

void VizGraph::layoutApplyButtonPressed() {
  initOgdf();
  applyLayout(currentLayout, (double)viewWidth/(double)viewHeight, true);
}

class VizLayoutCancelException : public exception {
  public :
};

void VizGraph::cancelLayout() {
  if (activeLayout)
    throw VizLayoutCancelException();
}

void VizGraph::applyLayout(VizLayout* layout, double ratio, bool fromGui) {
  if (activeLayout)
    return;

  activeLayout = layout;
  form.vizCanvasOverlay->show();
  QCoreApplication::processEvents();

  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    for (auto it : elements[idx]) {
      it.second->resetLayout();
    }
  }

  //connect(form.LayoutProgressCancel,
  // &QPushButton::clicked, layout, &VizLayout::abort);
  connect(layout, &VizLayout::progress, this,
          &VizGraph::layoutProgress,Qt::UniqueConnection);
  try {
    layout->apply(ratio, fromGui);
  } catch (VizLayoutCancelException& e) {

  }
  form.vizCanvasOverlay->hide();
  activeLayout = NULL;

}

void VizGraph::saveGFA(QString filename, GfaVersion version,
                       bool savestyle, bool savelayout) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning("Can't open file \"%s\" for writing!", qUtf8Printable(filename));
    return;
  }

  gfa->removeTag(VIZ_OPTIONSTAG);
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    for (auto it : elements[idx]) {
      GfaLine* line = it.second->getGfaElement();
      line->removeTag(VIZ_OPTIONSTAG);
      line->removeTag(VIZ_LAYOUTTAG);
    }
  }
  if (savestyle) {
    if (settings.size() > 0) {
      QJsonDocument doc(settings.toJson());
      GfaTag* tag = new GfaTag(VIZ_OPTIONSTAG, GFA_TAG_JSON,
          doc.toJson(QJsonDocument::Compact).constData());
      gfa->addTag(tag);
    }
    for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
      for (auto it : elements[idx]) {
        it.second->saveStyle();
      }
    }
  }
  if (savelayout) {
    for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
      for (auto it : elements[idx]) {
        it.second->saveLayout();
      }
    }
  }
  stringstream ss;
  ss << *gfa;
  file.write(ss.str().c_str());
  file.close();
}

void VizGraph::setHasLayout(bool value) {
  hasLayout = value;
}

void VizGraph::selectAll() {
  scene->blockSignals(true);
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    for (auto it : elements[idx]) {
      it.second->setSelected(true);
    }
  }
  scene->blockSignals(false);
  selectionChanged();
}

void VizGraph::selectNone() {
  scene->clearSelection();
}

void VizGraph::selectInvert() {
  scene->blockSignals(true);
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    for (auto it : elements[idx]) {
      it.second->setSelected(!it.second->isSelected());
    }
  }
  scene->blockSignals(false);
  selectionChanged();
}
