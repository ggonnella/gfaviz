#include "viznode.h"
#include "vizgraph.h"
#include "vecops.h"

VizNode::VizNode(GfaSegment* _gfa_node, VizGraph* _vg) : VizElement(VIZ_SEGMENT, _vg, _gfa_node) {
  gfa_node = _gfa_node;
  //width = 6.5*0.8;
  
  unsigned long length = gfa_node->getLength();
  unsigned long n_nodes = max((unsigned long)((double)length / vg->settings.basesPerNode),2UL);
  unsigned long basesPerNodeLocal = length/n_nodes;
  
  node prev = NULL;
  for (unsigned long idx = 0; idx < n_nodes; idx++) {
    node n = vg->G.newNode();
    ogdf_nodes.push_back(n);
    vg->GA.width(n) = 0.25; //10*5;
    vg->GA.height(n) = 0.25; //10*5;
    vg->GA.x(n) = (rand() / (double)RAND_MAX) * 1000.0;
    vg->GA.y(n) = (rand() / (double)RAND_MAX) * 1000.0;
    if (idx>0) {
      edge e = vg->G.newEdge(prev, n);
      ogdf_edges.push_back(e);
      vg->GA.doubleWeight(e) = 2*max(5.0,((double)basesPerNodeLocal / (double)vg->settings.basesPerNode) * 5.0); // + (rand()%10);
      vg->GA.doubleWeight(e) = max(vg->GA.doubleWeight(e), getOption(VIZ_MINWEIGHT).toDouble() / (n_nodes-1));
      
      vg->edgeLengths[e] = 0.1; //node_dist;
      //cout << node_dist << endl;
    }
    prev = n;
  }
  
  
}
VizNode::~VizNode() {
  
}


node VizNode::getStart() {
  return ogdf_nodes[0];
}
node VizNode::getEnd() {
  return ogdf_nodes.back();
}

QPointF VizNode::getStartDir() {
  return normalize(vg->getNodePos(ogdf_nodes[0]) - vg->getNodePos(ogdf_nodes[1]));
}
QPointF VizNode::getEndDir() {
  size_t idx = ogdf_nodes.size()-1;
  return normalize(vg->getNodePos(ogdf_nodes[idx]) - vg->getNodePos(ogdf_nodes[idx-1]));
}

node VizNode::getNodeAtBase(unsigned long base) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  size_t idx = round((double)base/(double)(gfa_node->getLength()) * (ogdf_nodes.size()-1));
  return ogdf_nodes[idx];
}

QPointF VizNode::getCoordForBase(unsigned long base, double offset) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  double fract = ((double)base/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1);
  size_t idx = floor(fract);
  double mult = fract - idx;
  
  if (idx == ogdf_nodes.size()-1)
    return getCoordForSubnode(idx, offset);
  
  QPointF p1 = getCoordForSubnode(idx, offset);
  QPointF p2 = getCoordForSubnode(idx+1, offset);
  return (p1 * (1.0-mult) + p2 * mult);
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
  
  if (idx == 0 || idx == ogdf_nodes.size()-1) {
    res -= dir * fabs(offset);
  }
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
  
  setAcceptHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
  
  if (!scene())
    vg->scene->addItem(this);
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
  
  /*for (size_t idx = 0; idx < highlights.size(); idx++) {
    drawHighlight(highlights[idx]);
  }*/
  
  if (getOption(VIZ_SHOWSEGMENTLABELS).toBool()) {
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

void VizNode::drawHighlight(VizNodeHighlight* highlight) {
  QPainterPath intpath;
  double width = getOption(VIZ_SEGMENTWIDTH).toDouble();
  for (int above = 0; above <= 1; above++) {
    double curpos = (width + highlight->startheight) * (above == 0 ? 1.0 : -1.0);
    intpath.moveTo(getCoordForBase(highlight->begin, curpos));
    size_t idx_start = ceil(((double)highlight->begin/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1));
    size_t idx_end = floor(((double)highlight->end/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1));
    //TODO
    idx_start = min(idx_start, ogdf_nodes.size()-1);
    idx_end = min(idx_end, ogdf_nodes.size()-1);
    size_t n_idx = idx_start;
    size_t e_idx = 0;
    while (n_idx < idx_end || e_idx < highlight->events.size()) {
      unsigned long n_base = ((double)n_idx/(double)ogdf_nodes.size()) * gfa_node->getLength();
      if (e_idx >= highlight->events.size() || n_base < highlight->events[e_idx].pos) {
        intpath.lineTo(getCoordForSubnode(n_idx,curpos));
        n_idx++;
      } else {
        intpath.lineTo(getCoordForBase(highlight->events[e_idx].pos,curpos));
        curpos += highlight->events[e_idx].size * (highlight->events[e_idx].up ? 1.0 : -1.0) * (above == 0 ? 1.0 : -1.0);
        intpath.lineTo(getCoordForBase(highlight->events[e_idx].pos,curpos));
        e_idx++;
      }
    }
    intpath.lineTo(getCoordForBase(highlight->end, curpos));
  }
  QPen pen(Qt::red);
  pen.setWidthF(highlight->size*1.5);
  QGraphicsPathItem* item = vg->scene->addPath(intpath, pen);
  item->setParentItem(this);
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
    /*for (GfaFragment* fragment : gfa_node->getFragments()) {
      vg->getFragment(fragment)->draw();
    }*/
    for (GfaGap* gap : gfa_node->getGaps()) {
      vg->getGap(gap)->draw();
    }
  }
  
  if (change == ItemPositionHasChanged && scene()) {
    for (VizGroup* group : groups) {
      group->recenterLabel();
      group->setScale(1.0);
      group->update();
    }
  }
  return VizElement::itemChange(change, value);
}
QRectF VizNode::boundingRect() const {
  qreal margin = groups.size()*5;
  margin = 0;
  return VizElement::boundingRect().adjusted(-margin,-margin,margin,margin);
}


static bool compareHighlightEvents(const VizNodeHighlightEvent& i, const VizNodeHighlightEvent& j) { 
  return (i.pos<j.pos);
}
VizNodeHighlight* VizNode::registerHighlight(unsigned long begin, unsigned long end, double size) {
  VizNodeHighlight* highlight = new VizNodeHighlight(begin, end, size);
  for (size_t idx = 0; idx < highlights.size(); idx++) {
    VizNodeHighlight* h = highlights[idx];
    if (begin >= h->begin && begin <= h->end)
      highlight->startheight++;
    if (h->begin > begin && h->begin <= end)
      highlight->events.push_back((VizNodeHighlightEvent){h->begin, h->size, true});
    if (h->end > begin && h->end <= end)
      highlight->events.push_back((VizNodeHighlightEvent){h->end, h->size, false});
  }
  sort(highlight->events.begin(), highlight->events.end(), compareHighlightEvents);
  
  highlights.push_back(highlight);
  return highlight;
}

VizNodeHighlight::VizNodeHighlight(unsigned long _begin, unsigned long _end, double _size) {
  begin = _begin;
  end = _end;
  size = _size;
  startheight = 0.0;
}

