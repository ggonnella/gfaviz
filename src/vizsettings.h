#pragma once
#include <QString>
#include <QMap>
#include <QVariant>
#include <QCommandLineParser>
#include <QJsonObject>
#include <vector>
#include "headers.h"

enum VizGraphParam {
  //VIZ_BASESPERNODE,
  VIZ_NONE = -1,
  VIZ_BACKGROUNDCOLOR,
  VIZ_SHOWALLLABELS,
  VIZ_SHOWSEGMENTLABELS,
  VIZ_SHOWEDGELABELS,
  VIZ_SHOWGAPLABELS,
  VIZ_SHOWGROUPLABELS,
  VIZ_DISABLEGAPS,
  VIZ_DISABLEFRAGMENTS,
  VIZ_DISABLEGROUPS,
  VIZ_SEGMENTWIDTH,
  VIZ_SEGMENTOUTLINEWIDTH,
  VIZ_SEGMENTMAINCOLOR,
  VIZ_SEGMENTOUTLINECOLOR,
  VIZ_EDGEWIDTH,
  VIZ_EDGECOLOR,
  VIZ_DOVETAILWIDTH,
  VIZ_DOVETAILCOLOR,
  VIZ_INTERNALWIDTH,
  VIZ_INTERNALCOLOR,
  VIZ_GROUPWIDTH,
  VIZ_GROUPCOLORS,
  VIZ_GROUPCOLOR,
  VIZ_GAPCOLOR,
  VIZ_LABELFONT,
  VIZ_LABELFONTSIZE,
  VIZ_LABELCOLOR,
  VIZ_LABELOUTLINEWIDTH,
  VIZ_LABELOUTLINECOLOR,
  VIZ_SEGLABELSHOWLENGTH,
  //VIZ_EDGELABELSHOWLENGTH,
  VIZ_MINWEIGHT,
  VIZ_USEFMMM,
  VIZ_LASTPARAM //Needs to be the last parameter
};
class VizGraphParamAttrib{
  public:
    VizGraphParamAttrib() {}
    VizGraphParamAttrib(const QString& _name, const QString& _description, QMetaType::Type _type, QVariant _defaultvalue = QVariant(), VizGraphParam _fallback=VIZ_NONE, bool _userdefined = true, bool _saveable = true);
    QString name;
    QString description;
    QMetaType::Type type;
    QVariant defaultvalue;
    VizGraphParam fallback;
    bool userdefined;
    bool saveable;
    QCommandLineOption* cmdOption;
};

class VizGraphSettings {
  public:
    VizGraphSettings();
    static std::vector<VizGraphParamAttrib> params;
    static QMap<QString,VizGraphParam> nameToParamMap; //QHash better?
    static void initParams();
    static void addOptions(QCommandLineParser* parser);
    //static VizGraphParamAttrib getAttrib(VizGraphParam p);
    
    void setFromOptionParser(QCommandLineParser* parser);
    const QVariant get(VizGraphParam p, VizGraphSettings* fallback = NULL) const;
    void set(VizGraphParam p, QVariant val, bool overwrite = true);
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
    QMap<VizGraphParam,QVariant> values;
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


