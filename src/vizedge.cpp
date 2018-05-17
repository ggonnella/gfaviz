#include "vizedge.h"

#include "vizgraph.h"
#include "vecops.h"


VizEdge::VizEdge(GfaEdge* _gfa_edge, VizGraph* _vg) {
  gfa_edge = _gfa_edge;
  isDovetail = gfa_edge->isDovetail();
  vg = _vg;
  viz_nodes[0] = vg->getNode(gfa_edge->getSegment(0));
  viz_nodes[1] = vg->getNode(gfa_edge->getSegment(1));
  double highlight_width = 1.0;
  if (isDovetail) {
    connected_subnodes[0] = (gfa_edge->isInedge(0) ? viz_nodes[0]->getStart() : viz_nodes[0]->getEnd());
    connected_subnodes[1] = (gfa_edge->isInedge(1) ? viz_nodes[1]->getStart() : viz_nodes[1]->getEnd());
  } else {
    connected_subnodes[0] = viz_nodes[0]->getNodeAtBase((gfa_edge->getBegin(0)+gfa_edge->getEnd(0))/2);
    connected_subnodes[1] = viz_nodes[1]->getNodeAtBase((gfa_edge->getBegin(1)+gfa_edge->getEnd(1))/2);
    highlight_width = 3.0;
  }
  if (gfa_edge->positionsSet() && !isDovetail) {
    highlights[0] = viz_nodes[0]->registerHighlight(gfa_edge->getBegin(0), gfa_edge->getEnd(0), highlight_width);
    highlights[1] = viz_nodes[1]->registerHighlight(gfa_edge->getBegin(1), gfa_edge->getEnd(1), highlight_width);
  }
  
  ogdf_edge = vg->G.newEdge(connected_subnodes[0], connected_subnodes[1]);
  vg->edgeLengths[ogdf_edge] = 20;
  if (isDovetail) {
    vg->GA.doubleWeight(ogdf_edge) = 10;
  } else {
    vg->GA.doubleWeight(ogdf_edge) = 40;
  }
}

VizEdge::~VizEdge() {
  
}

void VizEdge::draw() {
  QPen pen(Qt::black);
  pen.setWidth(2);
  
  if (!isDovetail) {
    pen.setColor(Qt::red);
    
    QPointF p1 = viz_nodes[0]->getCoordForBase((gfa_edge->getBegin(0)+gfa_edge->getEnd(0))/2);
    QPointF p2 = viz_nodes[1]->getCoordForBase((gfa_edge->getBegin(1)+gfa_edge->getEnd(1))/2);
    graphicsItem = new VizEdgeGraphicsItem(this);
    QPainterPath path;
    path.moveTo(p1);
    path.lineTo(p2);
    graphicsItem->setPath(path);
    //graphicsItem->setLine(QLineF(p1*0.5+p2*0.5, p3*0.5+p4*0.5));
    graphicsItem->setPen(pen);
    vg->scene->addItem(graphicsItem);
    graphicsItem->setAcceptHoverEvents(true);
  } else {
    graphicsItem = new VizEdgeGraphicsItem(this);
    //graphicsItem->setLine(vg->GA.x(connected_subnodes[0]),
    //                      vg->GA.y(connected_subnodes[0]),
    //                      vg->GA.x(connected_subnodes[1]),
    //                      vg->GA.y(connected_subnodes[1]));
    QPainterPath path;
    QPointF p1 = Ogdf2Qt(vg->GA, connected_subnodes[0]);
    QPointF p2 = Ogdf2Qt(vg->GA, connected_subnodes[1]);
    QPointF d1 = (gfa_edge->isInedge(0) ? viz_nodes[0]->getStartDir() : viz_nodes[0]->getEndDir());
    QPointF d2 = (gfa_edge->isInedge(1) ? viz_nodes[1]->getStartDir() : viz_nodes[1]->getEndDir());
    path.moveTo(p1);
    path.cubicTo(p1+10*d1, p2+10*d2, p2);
    //path.lineTo(Ogdf2Qt(vg->GA, connected_subnodes[1]));
    graphicsItem->setPath(path);
    
    graphicsItem->setPen(pen);
    vg->scene->addItem(graphicsItem);
    graphicsItem->setAcceptHoverEvents(true);
  }
}


VizEdgeGraphicsItem::VizEdgeGraphicsItem(VizEdge* _parent) {
  parent = _parent;
  setAcceptHoverEvents(true);
  //setFlag(QGraphicsItem::ItemIsMovable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
}
void VizEdgeGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  parent->setHighlight(true);
  QPen pen(Qt::black);
  if (!parent->isDovetail)
    pen.setColor(Qt::red);
  pen.setWidth(3);
  setPen(pen);
  update();
}
void VizEdgeGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  parent->setHighlight(false);
  QPen pen(Qt::black);
  if (!parent->isDovetail)
    pen.setColor(Qt::red);
  pen.setWidth(2);
  setPen(pen);
  update();
}

void VizEdge::setHighlight(bool _val) {
  
}
