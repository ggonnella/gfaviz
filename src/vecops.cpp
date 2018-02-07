#include "vecops.h"

float length(const QPointF& p) {
  return sqrt(p.x()*p.x()+p.y()*p.y());
}
QPointF normalize(const QPointF& p) {
  return p / length(p);
}
QPointF Ogdf2Qt(const ogdf::GraphAttributes& GA, ogdf::node n) {
  return QPointF(GA.x(n), GA.y(n));
}
