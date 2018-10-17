#include "vizfragment.h"

#include "vizgraph.h"
#include "vecops.h"

#include <QJsonArray>
#include <QJsonDocument>

VizFragment::VizFragment(GfaFragment* _gfa_fragment, VizGraph* _vg) :
                          VizElement(VIZ_FRAGMENT, _vg, _gfa_fragment) {
  gfa_fragment = _gfa_fragment;
  highlight = NULL;
  base = (gfa_fragment->getSegmentBegin() + gfa_fragment->getSegmentEnd()) / 2;

  viz_node = vg->getNode(gfa_fragment->getSegment());

  highlight = viz_node->registerHighlight(gfa_fragment->getSegmentBegin(),
                                          gfa_fragment->getSegmentEnd());

  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  line = new VizFragmentLine(this);
  vg->scene->addItem(line);
  vg->scene->addItem(this);
  //setParentItem(viz_node);
}

VizFragment::~VizFragment() {
  /* nothing to do */
}

void VizFragment::initOgdf() {
  bool validPosData;
  QJsonArray posdata;
  
  validPosData = false;
  posdata = readLayoutData("P");
  if (posdata.size() == 2 && posdata[0].isDouble() && posdata[1].isDouble()) {
    validPosData = true;
  }

  if (!validPosData) {
    vg->setHasLayout(false);
  }

  connected_subnode =
    viz_node->getNodeAtBase((gfa_fragment->getSegmentBegin() +
                             gfa_fragment->getSegmentEnd()) / 2);

  ogdf_node = vg->G.newNode();
  vg->GA.width(ogdf_node) = 55;
  vg->GA.height(ogdf_node) = 55;

  if (validPosData) {
    vg->GA.x(ogdf_node) = posdata[0].toDouble() / 10.0;
    vg->GA.y(ogdf_node) = posdata[1].toDouble() / 10.0;
  } else {
    vg->GA.x(ogdf_node) = vg->G.numberOfNodes() % 100;
    vg->GA.y(ogdf_node) = (int)(vg->G.numberOfNodes() / 100);
  }
  ogdf_edge = vg->G.newEdge(connected_subnode, ogdf_node);
  vg->GA.doubleWeight(ogdf_edge) = getOption(VIZ_FRAGMENTDIST).toDouble() *
                                   getOption(VIZ_SM_FRAGMENTDIST).toDouble();
  vg->edgeLengths[ogdf_edge] = getOption(VIZ_FRAGMENTDIST).toDouble() *
                               getOption(VIZ_FMMM_FRAGMENTDIST).toDouble();
}

QColor VizFragment::getColor() {
  if (gfa_fragment->isReverse()) {
    return getOption(VIZ_FWDFRAGMENTCOLOR).value<QColor>();
  }
  else {
    return getOption(VIZ_REVFRAGMENTCOLOR).value<QColor>();
  }
}

QJsonObject VizFragment::getLayoutData() {
  QJsonObject data = VizElement::getLayoutData();
  QJsonArray posdata;
  int px = (int)(vg->GA.x(ogdf_node)*10.0);
  int py = (int)(vg->GA.y(ogdf_node)*10.0);
  posdata.push_back(QJsonValue(px));
  posdata.push_back(QJsonValue(py));
  data.insert("P",QJsonValue(posdata));
  return data;
}

void VizFragment::draw() {
  QPointF oldpos = pos(), p1, p2, dir;
  QPen pen(getColor());
  QBrush brush(Qt::black);
  QPainterPath path;
  double frac = ((double)gfa_fragment->getFragmentLength() /
                 (double)gfa_fragment->getSegment()->getLength());
  double length = frac * viz_node->getDrawingLength();
  length = max(length, getOption(VIZ_FRAGMENTMINLENGTH).toDouble());
  length = length * getOption(VIZ_FRAGMENTMULTLENGTH).toDouble();

  /* Fragment Information, turn on for debug:
  cout << "Fragment of segment: " << gfa_fragment->getSegment()->getName()
       << "; external: " << gfa_fragment->getFragment()
       << "; f_length: " << gfa_fragment->getFragmentLength()
       << "; s_length: " << gfa_fragment->getSegmentLength()
       << "; s_total_len: " << gfa_fragment->getSegment()->getLength()
       << "; s_drawing_len: " << viz_node->getDrawingLength()
       << "; f_frac: " << frac
       << "; f_drawing_len: " << length
       << "; n_subsegs: " << viz_node->get_n_subsegs()
       << "\n";
  */

  setPos(0,0);
  pen.setWidthF(getOption(VIZ_FRAGMENTWIDTH).toDouble());
  setPen(pen);
  setBrush(brush);
  p1 = viz_node->getCoordForBase(base);
  p2 = vg->getNodePos(ogdf_node);

  line->setLine(QLineF(p1,p2));
  QPen linepen;
  linepen.setWidthF(getOption(VIZ_FRAGMENTCONNWIDTH).toDouble());
  linepen.setColor(getOption(VIZ_FRAGMENTCONNCOLOR).value<QColor>());
  line->setPen(linepen);
  line->update();
  
  
  dir = viz_node->getDirAtBase(base);
  path.moveTo(p2 - dir * length/2);
  path.lineTo(p2 + dir * length/2);

  setPath(path);

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
  QPen pen(getColor());
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
  QPen pen(getColor());
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
    cout << value.toPointF().x() << " " << value.toPointF().y() <<
        " - " << pos().x() << " " << pos().y() << endl;
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
}

void VizFragmentLine::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  frag->hover(true,e);
}

void VizFragmentLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  frag->hover(false,e);
}
