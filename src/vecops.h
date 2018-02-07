#pragma once
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <QPointF>

float length(const QPointF& p);
QPointF normalize(const QPointF& p);
QPointF Ogdf2Qt(const ogdf::GraphAttributes& GA, ogdf::node n);
