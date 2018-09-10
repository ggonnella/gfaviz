#include "viznode.h"
#include "vizgraph.h"
#include "vecops.h"

#include <QJsonArray>
#include <QJsonDocument>

VizNode::VizNode(GfaSegment* _gfa_node, VizGraph* _vg) :
                   VizElement(VIZ_SEGMENT, _vg, _gfa_node) {
  gfa_node = _gfa_node;

  bool validPosData = false;
  QJsonArray posdata = readLayoutData("P");
  if (posdata.size() >= 4 && posdata.size() % 2 == 0) {
    validPosData = true;
    for (int idx=0; idx < posdata.size(); idx++) {
      if (!posdata[idx].isDouble()) {
        validPosData = false;
        break;
      }
    }
  }

  unsigned long length = gfa_node->getLength();
  unsigned long n_nodes;

  if (validPosData) {
    n_nodes = posdata.size() / 2;
  } else {
    // be sure there are at least 2 subnodes
    n_nodes = max((unsigned long)(
          (double)length / vg->settings.basesPerNode), 2UL);
    vg->setHasLayout(false);
  }

  double basesPerNodeLocal = (double)length/(double)n_nodes;
  double weight = getOption(VIZ_WEIGHTFACTOR).toDouble() * (basesPerNodeLocal /
                    (double)vg->settings.basesPerNode);
  weight = max(weight, getOption(VIZ_MINWEIGHT).toDouble() / (n_nodes-1));
  double node_dist = (basesPerNodeLocal / (double)vg->settings.basesPerNode);
  node_dist = max(node_dist, 0.2);

  node prev = NULL;
  for (unsigned long idx = 0; idx < n_nodes; idx++) {
    node n = vg->G.newNode();
    ogdf_nodes.push_back(n);
    vg->GA.width(n) = 0.25;
    vg->GA.height(n) = 0.25;

    // set initial positions to either the position data, if valid
    // or to a random value (which works better than setting the
    // positions all to 0,0)
    if (validPosData) {
      vg->GA.x(n) = posdata[idx*2].toDouble(0.0);
      vg->GA.y(n) = posdata[idx*2+1].toDouble(0.0);
    } else {
      vg->GA.x(n) = (rand() / (double)RAND_MAX) * 1000.0;
      vg->GA.y(n) = (rand() / (double)RAND_MAX) * 1000.0;
    }

    if (idx>0) {
      edge e = vg->G.newEdge(prev, n);
      ogdf_edges.push_back(e);
      vg->GA.doubleWeight(e) = weight;

      vg->edgeLengths[e] = node_dist;
    }
    prev = n;
  }

  setAcceptHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  vg->scene->addItem(this);
}

VizNode::~VizNode() {
  /* empty */
}

QJsonObject VizNode::getLayoutData() {
  QJsonObject data = VizElement::getLayoutData();
  QJsonArray posdata;
  for (node n : ogdf_nodes) {
    double px = (double)((int)(vg->GA.x(n)*10.0))/10.0;
    double py = (double)((int)(vg->GA.y(n)*10.0))/10.0;
    posdata.push_back(QJsonValue(px));
    posdata.push_back(QJsonValue(py));
  }
  data.insert("P",QJsonValue(posdata));
  return data;
}

node VizNode::getStart() {
  return ogdf_nodes[0];
}
node VizNode::getEnd() {
  return ogdf_nodes.back();
}

QPointF VizNode::getStartDir() {
  return normalize(vg->getNodePos(ogdf_nodes[0]) -
      vg->getNodePos(ogdf_nodes[1]));
}
QPointF VizNode::getEndDir() {
  size_t idx = ogdf_nodes.size()-1;
  return normalize(vg->getNodePos(ogdf_nodes[idx]) -
      vg->getNodePos(ogdf_nodes[idx-1]));
}

node VizNode::getNodeAtBase(unsigned long base) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  size_t idx = round((double)base/(double)(gfa_node->getLength()) *
      (ogdf_nodes.size()-1));
  return ogdf_nodes[idx];
}

QPointF VizNode::getCoordForBase(unsigned long base, double offset) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  double fract = ((double)base/(double)(gfa_node->getLength())) *
    (ogdf_nodes.size()-1);
  size_t idx = floor(fract);
  double mult = fract - idx;

  if (idx == ogdf_nodes.size()-1)
    return getCoordForSubnode(idx, offset);

  QPointF p1 = getCoordForSubnode(idx, offset);
  QPointF p2 = getCoordForSubnode(idx+1, offset);
  return (p1 * (1.0-mult) + p2 * mult);
}

QPointF VizNode::getDirAtBase(unsigned long base) {
  base = min(base, gfa_node->getLength());
  double fract = ((double)base/(double)(gfa_node->getLength())) *
                   (ogdf_nodes.size()-1);
  size_t idx = floor(fract);

  if (idx == ogdf_nodes.size()-1)
    idx--;

  QPointF p1 = getCoordForSubnode(idx);
  QPointF p2 = getCoordForSubnode(idx+1);
  return normalize(p2-p1);
}

QPointF VizNode::getCoordForSubnode(size_t idx, double offset) {
  if (offset == 0.0f)
    return vg->getNodePos(ogdf_nodes[idx]);
  //TODO: should not happen
  if (idx >= ogdf_nodes.size())
    idx = ogdf_nodes.size()-1;

  QPointF res;

  QPointF p = vg->getNodePos(ogdf_nodes[idx]);
  QPointF p1 = (idx > 0 ?
                vg->getNodePos(ogdf_nodes[idx-1]) :
                vg->getNodePos(ogdf_nodes[0]));
  QPointF p2 = (idx < ogdf_nodes.size() - 1 ?
                vg->getNodePos(ogdf_nodes[idx+1]) :
                vg->getNodePos(ogdf_nodes[idx]));

  QPointF dir = p2 - p1;
  double len = sqrt(dir.x()*dir.x() + dir.y()*dir.y());
  dir /= len;

  res = QPointF(p.x() - offset * dir.y(), p.y() + offset * dir.x());

  //if (idx == 0 || idx == ogdf_nodes.size()-1) {
    res -= dir * fabs(offset);
  //}
  return res;
}

QPointF VizNode::getCenterCoord() {
  if (ogdf_nodes.size() % 2 == 0) {
    size_t idx = (ogdf_nodes.size()-2) / 2;
    QPointF p1 = vg->getNodePos(ogdf_nodes[idx]);
    QPointF p2 = vg->getNodePos(ogdf_nodes[idx+1]);
    return 0.5*p1 + 0.5*p2;
  } else {
    size_t idx = (ogdf_nodes.size()-1) / 2;
    return vg->getNodePos(ogdf_nodes[idx]);
  }
}

QPainterPath VizNode::getPath(VizGroup* group) {
  double width = getOption(VIZ_SEGMENTWIDTH).toDouble();
  QPainterPath path;

  if (group != NULL) {
    width += getOption(VIZ_SEGMENTOUTLINEWIDTH).toDouble() * 0.5;

    for (long unsigned int idx = 0; idx < groups.size(); idx++) {
      if (groups[idx] == group) {
        break;
      }
      width += groups[idx]->getOption(VIZ_GROUPWIDTH).toDouble();
    }
    width += group->getOption(VIZ_GROUPWIDTH).toDouble() * 0.5;
    width -= 0.01; //Epsilon
  }

  size_t node_n = ogdf_nodes.size();
  path.moveTo(vg->GA.x(ogdf_nodes[0]),vg->GA.y(ogdf_nodes[0]));
  for (size_t idx = 0; idx < node_n; idx++) {
    path.lineTo(getCoordForSubnode(idx,width));
  }
  path.lineTo(vg->GA.x(ogdf_nodes.back()),vg->GA.y(ogdf_nodes.back()));
  for (size_t idx = 0; idx < node_n; idx++) {
    path.lineTo(getCoordForSubnode(node_n-idx-1,-width));
  }
  path.lineTo(vg->GA.x(ogdf_nodes[0]),vg->GA.y(ogdf_nodes[0]));
  return path;
}

QPainterPath VizNode::getSubPath(unsigned long start, unsigned long end) {
  double width = getOption(VIZ_SEGMENTWIDTH).toDouble();
  QPainterPath path;
  int node_n = (int)ogdf_nodes.size();

  int start_idx = ceil(((double)start/(double)gfa_node->getLength()) * (node_n-1));
  int end_idx = floor(((double)end/(double)gfa_node->getLength()) * (node_n-1));

  if (start == 0) {
    path.moveTo(getCoordForSubnode(0,width));
  } else {
    path.moveTo(getCoordForBase(start,width));
  }


  for (int idx = start_idx; idx <= end_idx; idx++) {
    path.lineTo(getCoordForSubnode(idx,width));
  }
  if (end >= gfa_node->getLength()) {
    path.lineTo(vg->GA.x(ogdf_nodes.back()),vg->GA.y(ogdf_nodes.back()));
  } else {
    path.lineTo(getCoordForBase(end,width));
    path.lineTo(getCoordForBase(end));
    path.lineTo(getCoordForBase(end,-width));
  }
  for (int idx = end_idx; idx >= start_idx; idx--) {
    path.lineTo(getCoordForSubnode(idx,-width));
  }
  if (start == 0) {
    path.lineTo(getCoordForSubnode(0,-width));
    path.lineTo(getCoordForSubnode(0));
    path.lineTo(getCoordForSubnode(0,width));
  } else {
    path.lineTo(getCoordForBase(start,-width));
    path.lineTo(getCoordForBase(start));
    path.lineTo(getCoordForBase(start,width));
  }

  return path;
}

void VizNode::draw() {
  /*for (size_t idx = 0; idx < ogdf_nodes.size(); idx++) {
    QPointF p = getCoordForSubnode(idx);
    cout << p.x() << " " << p.y() << endl;
  }*/

  QPointF oldpos = pos();
  setPos(0,0);
  QBrush brush(getOption(VIZ_SEGMENTMAINCOLOR).value<QColor>());
  QPen outlinePen(getOption(VIZ_SEGMENTOUTLINECOLOR).value<QColor>());
  outlinePen.setWidthF(getOption(VIZ_SEGMENTOUTLINEWIDTH).toDouble());
  //graphicsPathItem = new VizNodeSegItem(this);
  setPath(getPath());
  setPen(outlinePen);
  setBrush(brush);

  //graphicsPathItem = vg->scene->addPath(path,outlinePen,blueBrush);

  /*if (1 == 0) {
    //display internal nodes
    size_t node_n = ogdf_nodes.size();
    QPainterPath path2;
    for (size_t idx = 0; idx < node_n; idx++) {
      path2.addEllipse(getCoordForSubnode(idx),1.0,1.0);
    }
    QGraphicsPathItem *path2Item = new QGraphicsPathItem(path2);
    path2Item->setPen(QPen(Qt::red));
    path2Item->setBrush(QBrush(Qt::red));
    vg->scene->addItem(path2Item);
  }*/

  for (VizNodeHighlight* highlight : highlights) {
    highlight->draw();
  }

  if (getOption(VIZ_SEGLABELSHOW).toBool()) {
    drawLabel(getOption(VIZ_SEGLABELFONT).toString(),
              getOption(VIZ_SEGLABELFONTSIZE).toDouble(),
              getOption(VIZ_SEGLABELCOLOR).value<QColor>(),
              getOption(VIZ_SEGLABELOUTLINEWIDTH).toDouble(),
              getOption(VIZ_SEGLABELOUTLINECOLOR).value<QColor>());
    QString text = QString::fromStdString(gfa_node->getName());
    if (getOption(VIZ_SEGLABELSHOWLENGTH).toBool() && gfa_node->isLengthSet())
      text += "\n" + QString::number(gfa_node->getLength()) + "bp";
    setLabelText(text);
    setLabelVisible(true);
  } else {
    setLabelVisible(false);
  }
  setPos(oldpos);
  setVisible(!getOption(VIZ_DISABLESEGMENTS).toBool());
}

GfaLine* VizNode::getGfaElement() {
  return gfa_node;
}

void VizNode::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  QBrush greenBrush(Qt::green);
  setBrush(greenBrush);
  for (VizGroup* group : groups) {
    group->update();
  }
  update();
  VizElement::hoverEnterEvent(e);
}
void VizNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  QBrush brush(getOption(VIZ_SEGMENTMAINCOLOR).value<QColor>());
  setBrush(brush);
  for (VizGroup* group : groups) {
    group->update();
  }
  update();
  VizElement::hoverLeaveEvent(e);
}
QVariant VizNode::itemChange(GraphicsItemChange change, const QVariant &value) {
  //cout << (int)change << endl;
  if (change == ItemPositionChange && scene()) {
    double dx = value.toPointF().x() - pos().x();
    double dy = value.toPointF().y() - pos().y();
    for (node n : ogdf_nodes) {
      vg->GA.x(n) += dx;
      vg->GA.y(n) += dy;
    }
    for (GfaEdge* edge : gfa_node->getEdges()) {
      vg->getEdge(edge)->draw();
    }
    /*for (VizGroup* group : groups) {
      group->draw();
    }*/
    for (GfaFragment* gfa_fragment : gfa_node->getFragments()) {
      VizFragment* fragment = vg->getFragment(gfa_fragment);
      if (!fragment->isSelected()) {
        fragment->moveBy(dx,dy);
      }
      //vg->getFragment(fragment)->draw();
    }
    for (GfaGap* gap : gfa_node->getGaps()) {
      vg->getGap(gap)->draw();
    }
    for (GfaGroup* group : gfa_node->getGroups()) {
      vg->getGroup(group)->draw();
    }
  }

  if (change == ItemPositionHasChanged && scene()) {
    for (VizGroup* group : groups) {
      group->setScale(1.0);
      group->update();
    }
  }
  return VizElement::itemChange(change, value);
}

QRectF VizNode::boundingRect() const {
  qreal margin = 0;
  for (VizGroup* group : groups) {
    margin += group->getOption(VIZ_GROUPWIDTH).toDouble();
  }
  return VizElement::boundingRect().adjusted(-margin,-margin,margin,margin);
}

VizNodeHighlight* VizNode::registerHighlight(unsigned long start,
                                             unsigned long end) {
  start = min(start, gfa_node->getLength());
  end = min(end, gfa_node->getLength());
  VizNodeHighlight* highlight = new VizNodeHighlight(this, start, end);
  highlights.push_back(highlight);
  return highlight;
}


VizNodeHighlight::VizNodeHighlight(VizNode* _parent, unsigned long _base_start,
                                   unsigned long _base_end) :
                                     QGraphicsPathItem(_parent) {
  parent = _parent;
  start = _base_start;
  end = _base_end;
}
void VizNodeHighlight::draw() {
  if (!isVisible())
    return;
  //QPainterPath path;
  //path.moveTo(parent->getCoordForBase(start));
  //path.lineTo(parent->getCoordForBase(end));
  //QPen outlinePen(getOption(VIZ_SEGMENTOUTLINECOLOR).value<QColor>());
  //outlinePen.setWidthF(getOption(VIZ_SEGMENTOUTLINEWIDTH).toDouble());
  //graphicsPathItem = new VizNodeSegItem(this);
  QPen pen(Qt::transparent);
  QBrush brush(color);
  setPen(pen);
  setBrush(brush);
  setPath(mapFromScene(parent->getSubPath(start,end)));
}

void VizNodeHighlight::setColor(QColor c) {
  if (color != c) {
    color = c;
    draw();
  }
}

void VizNodeHighlight::setVisibility(bool val) {
  bool prev = isVisible();
  setVisible(val);
  if (val && !prev) {
    draw();
  }
}
