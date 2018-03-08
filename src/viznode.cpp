#include "viznode.h"
#include "vizgraph.h"
#include "vecops.h"

VizNode::VizNode(GfaSegment* _gfa_node, VizGraph* _vg) {
  vg = _vg;
  gfa_node = _gfa_node;
  
  unsigned long n_nodes = max((unsigned long)((double)gfa_node->getLength() / vg->settings.basesPerNode),2UL);
  node prev = NULL;
  for (unsigned long idx = 0; idx < n_nodes; idx++) {
    node n = vg->G.newNode();
    ogdf_nodes.push_back(n);
    vg->GA.width(n) = 15; //10*5;
    vg->GA.height(n) = 15; //10*5;
    vg->GA.x(n) = (rand() / (double)RAND_MAX) * 1000.0;
    vg->GA.y(n) = (rand() / (double)RAND_MAX) * 1000.0;
    if (idx>0) {
      edge e = vg->G.newEdge(prev, n);
      vg->GA.doubleWeight(e) = 1;
      vg->edgeLengths[e] = 15; //node_dist;
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
  return normalize(Ogdf2Qt(vg->GA, ogdf_nodes[0]) - Ogdf2Qt(vg->GA, ogdf_nodes[1]));
}
QPointF VizNode::getEndDir() {
  size_t idx = ogdf_nodes.size()-1;
  return normalize(Ogdf2Qt(vg->GA, ogdf_nodes[idx]) - Ogdf2Qt(vg->GA, ogdf_nodes[idx-1]));
}

node VizNode::getNodeAtBase(unsigned long base) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  size_t idx = round((double)base/(double)(gfa_node->getLength()) * (ogdf_nodes.size()-1));
  return ogdf_nodes[idx];
}

QPointF VizNode::getCoordForBase(unsigned long base, bool above) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  double fract = ((double)base/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1);
  size_t idx = floor(fract);
  double mult = fract - idx;
  
  if (idx == ogdf_nodes.size()-1)
    return getCoordForSubnode(idx, above);
  
  QPointF p1 = getCoordForSubnode(idx, above);
  QPointF p2 = getCoordForSubnode(idx+1, above);
  return (p1 * (1.0-mult) + p2 * mult);
}
QPointF VizNode::getCoordForBase(unsigned long base) {
  //TODO what to do if base out of bounds?
  base = min(base, gfa_node->getLength());
  double fract = ((double)base/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1);
  size_t idx = floor(fract);
  double mult = fract - idx;
  
  if (idx == ogdf_nodes.size()-1)
    return getCoordForSubnode(idx);
  
  QPointF p1 = getCoordForSubnode(idx);
  QPointF p2 = getCoordForSubnode(idx+1);
  return (p1 * (1.0-mult) + p2 * mult); 
}

QPointF VizNode::getCoordForSubnode(size_t idx) {
  return Ogdf2Qt(vg->GA, ogdf_nodes[idx]);
}

QPointF VizNode::getCoordForSubnode(size_t idx, bool above) {
  double segwidth = 6.5*0.8;
  QPointF res;
  
  QPointF p = QPointF(vg->GA.x(ogdf_nodes[idx]),vg->GA.y(ogdf_nodes[idx]));
  QPointF p1 = (idx > 0 ?
                QPointF(vg->GA.x(ogdf_nodes[idx-1]),vg->GA.y(ogdf_nodes[idx-1])) :
                QPointF(vg->GA.x(ogdf_nodes[0]),vg->GA.y(ogdf_nodes[0])));
  QPointF p2 = (idx < ogdf_nodes.size() - 1 ?
                QPointF(vg->GA.x(ogdf_nodes[idx+1]),vg->GA.y(ogdf_nodes[idx+1])) :
                QPointF(vg->GA.x(ogdf_nodes[idx]),vg->GA.y(ogdf_nodes[idx])));
                
  QPointF dir = p2 - p1;
  double len = sqrt(dir.x()*dir.x() + dir.y()*dir.y());
  dir /= len;
  
  if (above) {
    res = QPointF(p.x() - segwidth * dir.y(), p.y() + segwidth * dir.x());
  } else {
    res = QPointF(p.x() + segwidth * dir.y(), p.y() - segwidth * dir.x());
  }
  if (idx == 0 || idx == ogdf_nodes.size()-1) {
    res -= dir * segwidth;
  }
  return res;
  
}

void VizNode::draw() {  
  QPainterPath path;
  
  size_t node_n = ogdf_nodes.size();
  path.moveTo(vg->GA.x(ogdf_nodes[0]),vg->GA.y(ogdf_nodes[0]));
  for (size_t idx = 0; idx < node_n; idx++) {
    path.lineTo(getCoordForSubnode(idx,true));
  }
  path.lineTo(vg->GA.x(ogdf_nodes.back()),vg->GA.y(ogdf_nodes.back()));
  for (size_t idx = 0; idx < node_n; idx++) {
    path.lineTo(getCoordForSubnode(node_n-idx-1,false));
  }
  path.lineTo(vg->GA.x(ogdf_nodes[0]),vg->GA.y(ogdf_nodes[0]));
  
  QBrush blueBrush(Qt::blue);
  QPen outlinePen(Qt::black);
  graphicsPathItem = new VizNodeSegItem();
  graphicsPathItem->setPath(path);
  graphicsPathItem->setPen(outlinePen);
  graphicsPathItem->setBrush(blueBrush);
  vg->scene->addItem(graphicsPathItem);
  graphicsPathItem->setAcceptHoverEvents(true);
  //graphicsPathItem = vg->scene->addPath(path,outlinePen,blueBrush);
  
  for (VizInternalEdge intEdge : internalEdges) {
    QPainterPath intpath;
    intpath.moveTo(getCoordForBase(intEdge.begin, false));
    size_t idx_start = ceil(((double)intEdge.begin/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1));
    size_t idx_end = floor(((double)intEdge.end/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1));
    for (size_t idx = idx_start; idx <= idx_end; idx++) {
      intpath.lineTo(getCoordForSubnode(idx,false));
    }
    intpath.lineTo(getCoordForBase(intEdge.end, false));
    QPen pen(Qt::red);
    pen.setWidth(3);
    vg->scene->addPath(intpath, pen);
  }
  for (VizInternalEdge intEdge : internalEdges) {
    QPainterPath intpath;
    intpath.moveTo(getCoordForBase(intEdge.begin, true));
    size_t idx_start = ceil(((double)intEdge.begin/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1));
    size_t idx_end = floor(((double)intEdge.end/(double)(gfa_node->getLength())) * (ogdf_nodes.size()-1));
    for (size_t idx = idx_start; idx <= idx_end; idx++) {
      intpath.lineTo(getCoordForSubnode(idx,true));
    }
    intpath.lineTo(getCoordForBase(intEdge.end, true));
    QPen pen(Qt::red);
    pen.setWidth(3);
    vg->scene->addPath(intpath, pen);
  }
}

VizNodeSegItem::VizNodeSegItem() {
  setAcceptHoverEvents(true);
  //setFlag(QGraphicsItem::ItemIsMovable);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlag(ItemAcceptsInputMethod, true);
}
void VizNodeSegItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  QBrush greenBrush(Qt::green);
  setBrush(greenBrush);
  update();
}
void VizNodeSegItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  QBrush blueBrush(Qt::blue);
  setBrush(blueBrush);
  update();
}

/*typedef struct { double x,y; } vec2d;
vec2d normalize(vec2d v) {
  double len = sqrt(v.x*v.x+v.y*v.y);
  return {v.x / len, v.y/len};
}

void VizNode::draw() {
  if (ogdf_nodes.size() == 1)
    return;
  
  cairo_t* ctx = gd->win->getCairoContext();
  cairo_set_source_rgb (ctx, 0, 0, 0);
  
  vec2d above[ogdf_nodes.size()];
  vec2d below[ogdf_nodes.size()];
  
  double sw = 5;
  for (size_t idx = 0; idx < ogdf_nodes.size(); idx++) {
    vec2d p = {gd->GA.x(ogdf_nodes[idx]),gd->GA.y(ogdf_nodes[idx])};
    vec2d p1 = (idx > 0 ?
                (vec2d){gd->GA.x(ogdf_nodes[idx-1]),gd->GA.y(ogdf_nodes[idx-1])} :
                (vec2d){gd->GA.x(ogdf_nodes[0]),gd->GA.y(ogdf_nodes[0])});
    vec2d p2 = (idx < ogdf_nodes.size() - 1 ?
                (vec2d){gd->GA.x(ogdf_nodes[idx+1]),gd->GA.y(ogdf_nodes[idx+1])} :
                (vec2d){gd->GA.x(ogdf_nodes[idx]),gd->GA.y(ogdf_nodes[idx])});
    double len = sqrt((p2.x-p1.x)*(p2.x-p1.x)+(p2.y - p1.y)*(p2.y - p1.y));
    
    above[idx] = {p.x - sw*(p2.y - p1.y)/len, p.y + sw*(p2.x-p1.x)/len};
    below[idx] = {p.x + sw*(p2.y - p1.y)/len, p.y - sw*(p2.x-p1.x)/len};
    if (idx == 0 || idx == ogdf_nodes.size() - 1) {
      above[idx].x -= sw*(p2.x - p1.x)/len;
      above[idx].y -= sw*(p2.y - p1.y)/len;
      below[idx].x -= sw*(p2.x - p1.x)/len;
      below[idx].y -= sw*(p2.y - p1.y)/len;
    }
  }
  
  cairo_set_source_rgb (ctx, 0,0,0);
  cairo_set_line_width(ctx, 2);
  
  cairo_move_to (ctx, gd->GA.x(ogdf_nodes[0]),gd->GA.y(ogdf_nodes[0]));
  size_t node_n = ogdf_nodes.size();
  for (size_t idx = 0; idx < node_n; idx++)
    cairo_line_to (ctx, above[idx].x, above[idx].y);
  cairo_line_to (ctx, gd->GA.x(ogdf_nodes.back()),gd->GA.y(ogdf_nodes.back()));
  for (size_t idx = 1; idx <= node_n; idx++)
    cairo_line_to (ctx, below[node_n-idx].x, below[node_n-idx].y);
  cairo_line_to (ctx, gd->GA.x(ogdf_nodes[0]),gd->GA.y(ogdf_nodes[0]));
  cairo_set_source_rgb (ctx, 0.5 + 0.5 * randf(), 0.5 + 0.5 * randf(), 0.5 + 0.5 * randf());
  cairo_fill (ctx);
  cairo_stroke(ctx);
}
*/

void VizNode::addInternalEdge(unsigned long begin, unsigned long end) {
  internalEdges.push_back((VizInternalEdge){begin,end});
}
