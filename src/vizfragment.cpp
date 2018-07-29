#include "vizfragment.h"

#include "vizgraph.h"
#include "vecops.h"

VizFragment::VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg) : VizElement(VIZ_FRAGMENT, _vg, _gfa_fragment) {
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
  //if (scene())
  //  vg->scene->removeItem(this);
  
  setPos(0,0);
  QPen pen(Qt::black);
  pen.setWidth(1);
  QBrush brush(Qt::black);
  QPointF p1 = viz_node->getCoordForBase((gfa_fragment->getSegmentBegin()+gfa_fragment->getSegmentEnd())/2);
  QPointF p2 = vg->getNodePos(ogdf_node);
  //graphicsItem = new VizFragmentGraphicsItem(this);
  QPainterPath path;
  path.addEllipse(p2, 5, 5);
  path.moveTo(p2);
  path.lineTo(p1);
  
  setPath(path);
  //graphicsItem->setLine(QLineF(p1*0.5+p2*0.5, p3*0.5+p4*0.5));
  setPen(pen);
  setBrush(brush);
  //setTransformOriginPoint(p2);
  
  setAcceptHoverEvents(true);
  //setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  if (!scene())
    vg->scene->addItem(this);
  setParentItem(viz_node);
  
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
  pen.setWidth(2);
  QBrush brush(Qt::white);
  setPen(pen);
  setBrush(brush);
  update();
  VizElement::hoverEnterEvent(e);
}

void VizFragment::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  QPen pen(Qt::black);
  pen.setWidth(1);
  QBrush brush(Qt::black);
  setPen(pen);
  setBrush(brush);
  update();
  VizElement::hoverLeaveEvent(e);
}

QVariant VizFragment::itemChange(GraphicsItemChange change, const QVariant &value) {
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
