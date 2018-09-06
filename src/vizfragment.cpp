#include "vizfragment.h"

#include "vizgraph.h"
#include "vecops.h"

#include <QJsonArray>
#include <QJsonDocument>

VizFragment::VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg) : VizElement(VIZ_FRAGMENT, _vg, _gfa_fragment) {
  gfa_fragment = _gfa_fragment;
  highlight = NULL;
  base = (gfa_fragment->getSegmentBegin()+gfa_fragment->getSegmentEnd())/2;
  
  bool validPosData = false;
  QJsonArray posdata = readLayoutData("P");
  if (posdata.size() == 2 && posdata[0].isDouble() && posdata[1].isDouble()) {
    validPosData = true;
  }
  
  if (!validPosData) {
    vg->setHasLayout(false);
  }
  
  viz_node = vg->getNode(gfa_fragment->getSegment());
  connected_subnode = viz_node->getNodeAtBase((gfa_fragment->getSegmentBegin()+gfa_fragment->getSegmentEnd())/2);
  
  highlight = viz_node->registerHighlight(gfa_fragment->getSegmentBegin(), gfa_fragment->getSegmentEnd());
  highlight->setVisibility(false);
  
  ogdf_node = vg->G.newNode();
  vg->GA.width(ogdf_node) = 15; //10*5;
  vg->GA.height(ogdf_node) = 15; //10*5;
  if (validPosData) {
    vg->GA.x(ogdf_node) = posdata[0].toDouble(0.0);
    vg->GA.y(ogdf_node) = posdata[1].toDouble(0.0);
  } else {
    vg->GA.x(ogdf_node) = (rand() / (double)RAND_MAX) * 1000.0;
    vg->GA.y(ogdf_node) = (rand() / (double)RAND_MAX) * 1000.0;
  }
  ogdf_edge = vg->G.newEdge(connected_subnode, ogdf_node);
  vg->GA.doubleWeight(ogdf_edge) = 10;
  vg->edgeLengths[ogdf_edge] = 15;
  
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  vg->scene->addItem(this);
  line = new VizFragmentLine(this);
  vg->scene->addItem(line);
  //setParentItem(viz_node);
}

VizFragment::~VizFragment() {
  
}

QJsonObject VizFragment::getLayoutData() {
  QJsonObject data = VizElement::getLayoutData();
  QJsonArray posdata;
  double px = (double)((int)(vg->GA.x(ogdf_node)*10.0))/10.0;
  double py = (double)((int)(vg->GA.y(ogdf_node)*10.0))/10.0;
  posdata.push_back(QJsonValue(px));
  posdata.push_back(QJsonValue(py));
  data.insert("P",QJsonValue(posdata));
  return data;
}

void VizFragment::draw() {
  //if (scene())
  //  vg->scene->removeItem(this);
  //cout << "drawfrag" << endl;
  QPointF oldpos = pos();
  setPos(0,0);
  
  QPen pen(getOption(VIZ_FRAGMENTCOLOR).value<QColor>());
  pen.setWidthF(getOption(VIZ_FRAGMENTWIDTH).toDouble());
  setPen(pen);
  QBrush brush(Qt::black);
  setBrush(brush);
  QPointF p1 = viz_node->getCoordForBase(base);
  QPointF p2 = vg->getNodePos(ogdf_node);
  //graphicsItem = new VizFragmentGraphicsItem(this);
  
  line->setLine(QLineF(p1,p2));
  QPainterPath path;
  QPointF dir = viz_node->getDirAtBase(base);
  path.moveTo(p2 - dir * 2);
  path.lineTo(p2 + dir * 2);
  
  //path.addEllipse(p2, 2, 2);
  
  setPath(path);
  //graphicsItem->setLine(QLineF(p1*0.5+p2*0.5, p3*0.5+p4*0.5));
  
  //setTransformOriginPoint(p2);
  if (highlight) {
    highlight->setVisibility(getOption(VIZ_FRAGMENTHIGHLIGHTSHOW).toBool());
    highlight->setColor(getOption(VIZ_FRAGMENTHIGHLIGHTCOLOR).value<QColor>());
  }
  
  if (getOption(VIZ_FRAGMENTLABELSHOW).toBool()) {
    drawLabel(getOption(VIZ_FRAGMENTLABELFONT).toString(),
              getOption(VIZ_FRAGMENTLABELFONTSIZE).toDouble(),
              getOption(VIZ_FRAGMENTLABELCOLOR).value<QColor>(),
              getOption(VIZ_FRAGMENTLABELOUTLINEWIDTH).toDouble(),
              getOption(VIZ_FRAGMENTLABELOUTLINECOLOR).value<QColor>());
    setLabelVisible(true);
  } else {
    setLabelVisible(false);
  }
  
  setPos(oldpos);
  line->setVisible(!getOption(VIZ_DISABLEFRAGMENTS).toBool());
  setVisible(!getOption(VIZ_DISABLEFRAGMENTS).toBool());
}

QPointF VizFragment::getCenterCoord() {
  return QPointF(vg->GA.x(ogdf_node),vg->GA.y(ogdf_node));
}

GfaLine* VizFragment::getGfaElement() {
  return gfa_fragment;
}

void VizFragment::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(getOption(VIZ_FRAGMENTCOLOR).value<QColor>());
  pen.setWidthF(getOption(VIZ_FRAGMENTWIDTH).toDouble() * 1.5);
  setPen(pen);
  update();
  if (highlight) {
    highlight->setVisibility(true);
    highlight->draw();
  }
  VizElement::hoverEnterEvent(e);
}

void VizFragment::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(getOption(VIZ_FRAGMENTCOLOR).value<QColor>());
  pen.setWidthF(getOption(VIZ_FRAGMENTWIDTH).toDouble());
  setPen(pen);
  update();
  if (!getOption(VIZ_FRAGMENTHIGHLIGHTSHOW).toBool()) {
    if (highlight)
      highlight->setVisibility(false);
  }
  VizElement::hoverLeaveEvent(e);
}

void VizFragment::hover(bool enter, QGraphicsSceneHoverEvent *e) {
  if (enter)
    hoverEnterEvent(e);
  else
    hoverLeaveEvent(e);
}

void VizFragment::move(double dx, double dy) {
  vg->GA.x(ogdf_node) += dx;
  vg->GA.y(ogdf_node) += dy;
  QPointF p1 = viz_node->getCoordForBase(base);
  QPointF p2 = vg->getNodePos(ogdf_node);
  //graphicsItem = new VizFragmentGraphicsItem(this);
  line->setLine(QLineF(p1,p2));
}

QVariant VizFragment::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionChange && scene()) {
    double dx = value.toPointF().x() - pos().x();
    double dy = value.toPointF().y() - pos().y();
    move(dx,dy);
    //draw();
  }
  /*cout << change << endl;
  if (change == ItemPositionChange && scene()) {
    //QPointF newpos = mapToScene(value.toPointF());
    double dx = value.toPointF().x() - pos().x();
    double dy = value.toPointF().y() - pos().y();
    cout << value.toPointF().x() << " " << value.toPointF().y() << " - " << pos().x() << " " << pos().y() << endl;
    vg->GA.x(ogdf_node) += dx;
    vg->GA.y(ogdf_node) += dy;
  }
  if (change == ItemPositionHasChanged && scene()) {
    draw();
    // return QPointF(0,0);
  }*/
  return VizElement::itemChange(change, value);
}

VizFragmentLine::VizFragmentLine(VizFragment* parent) {
  frag=parent;
  setAcceptHoverEvents(true);
  QPen pen;
  pen.setWidthF(0.5);
  setPen(pen);
}
void VizFragmentLine::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  frag->hover(true,e);
}
void VizFragmentLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  frag->hover(false,e);
}
