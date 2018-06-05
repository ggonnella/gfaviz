#pragma once
#include <QString>
#include <QMap>
#include <QVariant>
#include <QCommandLineParser>
#include <QJsonObject>
#include <vector>

enum VizGraphParam {
  //VIZ_BASESPERNODE,
  VIZ_NONE = -1,
  VIZ_BACKGROUNDCOLOR,
  VIZ_SHOWALLLABELS,
  VIZ_SHOWSEGMENTLABELS,
  VIZ_SHOWEDGELABELS,
  VIZ_SHOWGAPLABELS,
  VIZ_SEGMENTWIDTH,
  VIZ_SEGMENTOUTLINEWIDTH,
  VIZ_SEGMENTMAINCOLOR,
  VIZ_SEGMENTOUTLINECOLOR,
  VIZ_EDGEWIDTH,
  VIZ_EDGECOLOR,
  VIZ_LABELFONT,
  VIZ_LABELFONTSIZE,
  VIZ_LABELCOLOR,
  VIZ_LABELOUTLINEWIDTH,
  VIZ_LABELOUTLINECOLOR,
  /*VIZ_EDGEDOVETAILWIDTH,
  VIZ_EDGEDOVETAILCOLOR,
  VIZ_EDGEINTERNALWIDTH,
  VIZ_EDGEINTERNALCOLOR,*/
  VIZ_MINWEIGHT,
  VIZ_LASTPARAM
};
typedef struct {
  QString name;
  QString description;
  QMetaType::Type type;
  QVariant defaultvalue;
  VizGraphParam fallback;
  bool userdefined = false;
  bool saveable = true;
  QCommandLineOption* cmdOption = NULL;
} VizGraphParamAttrib;

class VizGraphSettings {
  public:
    VizGraphSettings();
    static std::vector<VizGraphParamAttrib> params;
    static QMap<QString,VizGraphParam> nameToParamMap; //QHash better?
    static void initParams();
    static void addOptions(QCommandLineParser* parser);
    //static VizGraphParamAttrib getAttrib(VizGraphParam p);
    
    void setFromOptionParser(QCommandLineParser* parser);
    const QVariant& get(VizGraphParam p) const;
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
    void fromJsonFile(QString stylesheet);
    
    QString filename;
    double basesPerNode = 1;
    
    
    /*bool showSegmentLabels = false;
    bool showEdgeLabels = false;
    bool showGapLabels = false;
    double minWeight = 12;*/
  private:
    std::vector<QVariant> values;
}; 

typedef struct {
  bool guiEnabled;
  bool renderEnabled;
  QString outputFile;
  QString outputFormat;
  unsigned int width;
  unsigned int height;
  VizGraphSettings graphSettings;
} VizAppSettings;


