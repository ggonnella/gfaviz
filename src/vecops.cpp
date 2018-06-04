#include "vecops.h"

float length(const QPointF& p) {
  return sqrt(p.x()*p.x()+p.y()*p.y());
}
QPointF normalize(const QPointF& p) {
  return p / length(p);
}

