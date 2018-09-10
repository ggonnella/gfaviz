#include "vizgap.h"
#include "vizgraph.h"
#include "vecops.h"

VizGap::VizGap(GfaGap* _gfa_gap, VizGraph* _vg) :
                 VizElement(VIZ_GAP, _vg, _gfa_gap) {
  gfa_gap = _gfa_gap;
  viz_nodes[0] = vg->getNode(gfa_gap->getSegment(0));
  viz_nodes[1] = vg->getNode(gfa_gap->getSegment(1));
  connected_subnodes[0] =
    (gfa_gap->isInedge(0) ? viz_nodes[0]->getStart() : viz_nodes[0]->getEnd());
  connected_subnodes[1] =
    (gfa_gap->isInedge(1) ? viz_nodes[1]->getStart() : viz_nodes[1]->getEnd());

  if (getOption(VIZ_GAPSEDGES).toBool())
  {
    ogdf_edge = vg->G.searchEdge(connected_subnodes[0], connected_subnodes[1]);
    if (ogdf_edge == 0)
      ogdf_edge = vg->G.newEdge(connected_subnodes[0], connected_subnodes[1]);
    vg->edgeLengths[ogdf_edge] = getOption(VIZ_GAPLENGTH).toDouble() / 20.0;
    vg->GA.doubleWeight(ogdf_edge) = getOption(VIZ_GAPLENGTH).toDouble();
  }

  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  vg->scene->addItem(this);
}

VizGap::~VizGap() {
  /* empty */
}

QColor VizGap::getColor() {
  if (gfa_gap->getDist() >= 0) {
    return getOption(VIZ_POSGAPCOLOR).value<QColor>();
  }
  else {
    return getOption(VIZ_NEGGAPCOLOR).value<QColor>();
  }
}

void VizGap::draw() {
  QPainterPath path;
  QPointF p1, p2, d1, d2;
  QPen pen(getColor());

  pen.setStyle(Qt::DotLine);
  pen.setWidthF(getOption(VIZ_GAPWIDTH).toDouble());

  p1 = vg->getNodePos(connected_subnodes[0]);
  p2 = vg->getNodePos(connected_subnodes[1]);
  d1 = (gfa_gap->isInedge(0) ? viz_nodes[0]->getStartDir()
                             : viz_nodes[0]->getEndDir());
  d2 = (gfa_gap->isInedge(1) ? viz_nodes[1]->getStartDir()
                                     : viz_nodes[1]->getEndDir());
  path.moveTo(p1);
  path.cubicTo(p1+15*d1, p2+15*d2, p2);
  setPath(path);
  setPen(pen);

  if (getOption(VIZ_GAPLABELSHOW).toBool()) {
    drawLabel(getOption(VIZ_GAPLABELFONT).toString(),
              getOption(VIZ_GAPLABELFONTSIZE).toDouble(),
              getOption(VIZ_GAPLABELCOLOR).value<QColor>(),
              getOption(VIZ_GAPLABELOUTLINEWIDTH).toDouble(),
              getOption(VIZ_GAPLABELOUTLINECOLOR).value<QColor>());
    setLabelVisible(true);
  } else {
    setLabelVisible(false);
  }

  setVisible(!getOption(VIZ_DISABLEGAPS).toBool());
}

QPointF VizGap::getCenterCoord() {
  QPointF p1 = vg->getNodePos(connected_subnodes[0]);
  QPointF p2 = vg->getNodePos(connected_subnodes[1]);
  return 0.5 * p1 + 0.5 * p2;
}

GfaLine* VizGap::getGfaElement() {
  return gfa_gap;
}

void VizGap::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(getColor());
  pen.setStyle(Qt::DotLine);
  pen.setWidthF(getOption(VIZ_GAPWIDTH).toDouble() * 1.5);
  setPen(pen);
  update();
  VizElement::hoverEnterEvent(e);
}

void VizGap::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(getColor());
  pen.setStyle(Qt::DotLine);
  pen.setWidthF(getOption(VIZ_GAPWIDTH).toDouble());
  setPen(pen);
  update();
  VizElement::hoverLeaveEvent(e);
}
