#include "vizgap.h"

#include "vizgraph.h"
#include "vecops.h"

VizGap::VizGap(GfaGap* _gfa_gap, VizGraph* _vg) : VizElement(_vg) {
  gfa_gap = _gfa_gap;
  viz_nodes[0] = vg->getNode(gfa_gap->getSegment(0));
  viz_nodes[1] = vg->getNode(gfa_gap->getSegment(1));
  connected_subnodes[0] = (gfa_gap->isInedge(0) ? viz_nodes[0]->getStart() : viz_nodes[0]->getEnd());
  connected_subnodes[1] = (gfa_gap->isInedge(1) ? viz_nodes[1]->getStart() : viz_nodes[1]->getEnd());
}

VizGap::~VizGap() {
  
}

void VizGap::draw() {
  QPen pen(Qt::gray);
  pen.setStyle(Qt::DotLine);
  pen.setWidth(2);

  //graphicsItem = new VizGapGraphicsItem(this);
  QPainterPath path;
  QPointF p1 = Ogdf2Qt(vg->GA, connected_subnodes[0]);
  QPointF p2 = Ogdf2Qt(vg->GA, connected_subnodes[1]);
  QPointF d1 = (gfa_gap->isInedge(0) ? viz_nodes[0]->getStartDir() : viz_nodes[0]->getEndDir());
  QPointF d2 = (gfa_gap->isInedge(1) ? viz_nodes[1]->getStartDir() : viz_nodes[1]->getEndDir());
  path.moveTo(p1);
  path.cubicTo(p1+15*d1, p2+15*d2, p2);
  setPath(path);
  setPen(pen);
  
  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  vg->scene->addItem(this);
  
  if (vg->settings.showGapLabels) {
    drawLabel();
  }
}

QPointF VizGap::getCenterCoord() {
  QPointF p1 = Ogdf2Qt(vg->GA, connected_subnodes[0]);
  QPointF p2 = Ogdf2Qt(vg->GA, connected_subnodes[1]);
  return 0.5 * p1 + 0.5 * p2;
}
GfaLine* VizGap::getGfaElement() {
  return gfa_gap;
}

void VizGap::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  QPen pen(Qt::gray);
  pen.setStyle(Qt::DotLine);
  pen.setWidth(3);
  setPen(pen);
  update();
}
void VizGap::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  QPen pen(Qt::gray);
  pen.setStyle(Qt::DotLine);
  pen.setWidth(2);
  setPen(pen);
  update();
}
