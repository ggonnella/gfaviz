#include "vizfragment.h"

#include "vizgraph.h"
#include "vecops.h"

#include <QJsonArray>
#include <QJsonDocument>

VizFragment::VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg) : VizElement(VIZ_FRAGMENT, _vg, _gfa_fragment) {
  gfa_fragment = _gfa_fragment;
  
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
  //TODO: Highlight
  
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
  
  //QPointF oldpos = pos();
  //setPos(0,0);
  QPen pen(Qt::black);
  pen.setWidthF(1);
  QBrush brush(Qt::black);
  QPointF p1 = viz_node->getCoordForBase((gfa_fragment->getSegmentBegin()+gfa_fragment->getSegmentEnd())/2) -pos();
  QPointF p2 = vg->getNodePos(ogdf_node) - pos();
  //graphicsItem = new VizFragmentGraphicsItem(this);
  QPainterPath path;
  path.moveTo(p2);
  path.lineTo(p1);
  path.addEllipse(p2, 2, 2);
  
  setPath(path);
  //graphicsItem->setLine(QLineF(p1*0.5+p2*0.5, p3*0.5+p4*0.5));
  setPen(pen);
  setBrush(brush);
  //setTransformOriginPoint(p2);
  //setPos(oldpos);
  
  setVisible(!getOption(VIZ_DISABLEFRAGMENTS).toBool());
}

QPointF VizFragment::getCenterCoord() {
  return QPointF(0.0,0.0);
}

GfaLine* VizFragment::getGfaElement() {
  return gfa_fragment;
}

void VizFragment::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(Qt::black);
  pen.setWidthF(2);
  QBrush brush(Qt::white);
  setPen(pen);
  setBrush(brush);
  update();
  VizElement::hoverEnterEvent(e);
}

void VizFragment::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(Qt::black);
  pen.setWidthF(1);
  QBrush brush(Qt::black);
  setPen(pen);
  setBrush(brush);
  update();
  VizElement::hoverLeaveEvent(e);
}

void VizFragment::move(double dx, double dy) {
  
}

QVariant VizFragment::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionChange && scene()) {
    double dx = value.toPointF().x() - pos().x();
    double dy = value.toPointF().y() - pos().y();
    vg->GA.x(ogdf_node) += dx;
    vg->GA.y(ogdf_node) += dy;
    draw();
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

