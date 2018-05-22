#pragma once
#include <QString>

typedef struct {
  QString filename;
  double basesPerNode = 1;
  bool showSegmentLabels = false;
  bool showEdgeLabels = false;
  bool showGapLabels = false;
  
  double minWeight = 12;
  
} VizGraphSettings;

typedef struct {
  bool guiEnabled;
  bool renderEnabled;
  QString outputFile;
  QString outputFormat;
  unsigned int width;
  unsigned int height;
  VizGraphSettings graphSettings;
} VizAppSettings;
