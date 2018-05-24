#include "vizfragment.h"

#include "vizgraph.h"
#include "vecops.h"

VizFragment::VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg) : VizElement(_vg) {
  gfa_fragment = _gfa_fragment;
  viz_node = vg->getNode(gfa_fragment->getSegment());
  connected_subnode = viz_node->getNodeAtBase((gfa_fragment->getSegmentBegin()+gfa_fragment->getSegmentEnd())/2);
  //TODO: Highlight
  
  ogdf_node = vg->G.newNode();
  vg->GA.width(ogdf_node) = 15; //10*5;
  vg->GA.height(ogdf_node) = 15; //10*5;
  vg->GA.x(ogdf_node) = (rand() / (double)RAND_MAX) * 1000.0;
  vg->GA.y(ogdf_node) = (rand() / (double)RAND_MAX) * 1000.0;
  ogdf_edge = vg->G.newEdge(connected_subnode, ogdf_node);
  vg->GA.doubleWeight(ogdf_edge) = 10;
  vg->edgeLengths[ogdf_edge] = 15;
}

VizFragment::~VizFragment() {
  
}

void VizFragment::draw() {
  QPen pen(Qt::black);
  pen.setWidth(1);
  QBrush brush(Qt::black);
  QPointF p1 = viz_node->getCoordForBase((gfa_fragment->getSegmentBegin()+gfa_fragment->getSegmentEnd())/2);
  QPointF p2 = Ogdf2Qt(vg->GA, ogdf_node);
  //graphicsItem = new VizFragmentGraphicsItem(this);
  QPainterPath path;
  path.moveTo(p1);
  path.lineTo(p2);
  path.addEllipse(p2, 5, 5);
  
  setPath(path);
  //graphicsItem->setLine(QLineF(p1*0.5+p2*0.5, p3*0.5+p4*0.5));
  setPen(pen);
  setBrush(brush);
  
  setAcceptHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  vg->scene->addItem(this);
}

QPointF VizFragment::getCenterCoord() {
  return QPointF(0.0,0.0);
}

GfaLine* VizFragment::getGfaElement() {
  return gfa_fragment;
}

void VizFragment::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  QPen pen(Qt::black);
  pen.setWidth(2);
  QBrush brush(Qt::white);
  setPen(pen);
  setBrush(brush);
  update();
}

void VizFragment::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  QPen pen(Qt::black);
  pen.setWidth(1);
  QBrush brush(Qt::black);
  setPen(pen);
  setBrush(brush);
  update();
}

