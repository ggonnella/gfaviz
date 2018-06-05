#include "vizsettings.h"
#include <assert.h>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>

std::vector<VizGraphParamAttrib> VizGraphSettings::params;
QMap<QString,VizGraphParam> VizGraphSettings::nameToParamMap;

void VizGraphSettings::initParams() {
  params.resize(VIZ_LASTPARAM);
  params[VIZ_BACKGROUNDCOLOR] = {"bg-color", "Background color.", QMetaType::QColor, QVariant("#00ffffff"), VIZ_NONE, true, true};
  params[VIZ_SHOWALLLABELS] = {"labels", "Add all labels to the graph.", QMetaType::Bool, QVariant(false), VIZ_NONE, true, true};
  params[VIZ_SHOWSEGMENTLABELS] = {"seg-labels", "Add segment labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS, true};
  params[VIZ_SHOWEDGELABELS] = {"edge-labels", "Add edge labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS, true};
  params[VIZ_SHOWGAPLABELS] = {"gap-labels", "Add gap labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS, true};
  params[VIZ_SEGMENTWIDTH] = {"seg-width", "Width of the segments.", QMetaType::Double, QVariant(4.0f), VIZ_NONE, true};
  params[VIZ_SEGMENTOUTLINEWIDTH] = {"seg-outline-width", "Width of the segment outline.", QMetaType::Double, QVariant(1.0f), VIZ_NONE, true};
  params[VIZ_SEGMENTMAINCOLOR] = {"seg-color", "Color of the segment.", QMetaType::QColor, QVariant("#0000ff"), VIZ_NONE, true};
  params[VIZ_SEGMENTOUTLINECOLOR] = {"seg-outline-color", "Color of the segment outline.", QMetaType::QColor, QVariant("#000000"), VIZ_NONE, true};
  params[VIZ_EDGEWIDTH] = {"edge-width", "Width of the links/edges.", QMetaType::Double, QVariant(2.0f), VIZ_NONE, true};
  params[VIZ_EDGECOLOR] = {"edge-color", "Color of the links/edges.", QMetaType::QColor, QVariant("#000000"), VIZ_NONE, true};
  params[VIZ_LABELFONT] = {"label-font", "Font family of the labels.", QMetaType::QString, QVariant("Arial"), VIZ_NONE, true};
  params[VIZ_LABELFONTSIZE] = {"label-size", "Font point size of the labels.", QMetaType::Double, QVariant(12.0f), VIZ_NONE, true};
  params[VIZ_LABELCOLOR] = {"label-color", "Font color of the labels.", QMetaType::QColor, QVariant("#000000"), VIZ_NONE, true};
  params[VIZ_LABELOUTLINECOLOR] = {"label-outline-color", "Font outline color of the labels.", QMetaType::QColor, QVariant("#ffffff"), VIZ_NONE, true};
  params[VIZ_LABELOUTLINEWIDTH] = {"label-outline-width", "Font outline width of the labels.", QMetaType::Double, QVariant(2.0f), VIZ_NONE, true};
  params[VIZ_MINWEIGHT] = {"minweight", "", QMetaType::Double, QVariant((double)12.0f), VIZ_NONE, false};
  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    nameToParamMap.insert(params[p].name, p);
  }
  
}

void VizGraphSettings::addOptions(QCommandLineParser* parser) {
  if (params.size() == 0)
    initParams();

  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    if (!params[p].userdefined)
      continue;
    if (params[p].type == QMetaType::Bool)
      params[p].cmdOption = new QCommandLineOption(QStringList() << params[p].name, params[p].description);
    else
      params[p].cmdOption = new QCommandLineOption(QStringList() << params[p].name, params[p].description, "value");
    parser->addOption(*(params[p].cmdOption));
  };
}

VizGraphSettings::VizGraphSettings() {
  if (params.size() == 0)
    initParams();
  //values.resize(VIZ_LASTPARAM);
}

void VizGraphSettings::setFromOptionParser(QCommandLineParser* parser) {
  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    if (!params[p].userdefined)
      continue;
    if (parser->isSet(*(params[p].cmdOption))) {
      if (params[p].type == QMetaType::Bool) {
        values[p] = QVariant(parser->isSet(*(params[p].cmdOption)));
      } else {
        QVariant val = QVariant(parser->value(*(params[p].cmdOption)));
        if (val.convert(params[p].type)) {
          values[p] = val;
        } else {
          qCritical("Could not convert value \"%s\" of option \"%s\" to type \"%s\".",
                    qUtf8Printable(parser->value(*(params[p].cmdOption))),
                    qUtf8Printable(params[p].name),
                    QMetaType::typeName(params[p].type));
        }
      }
    }      
  }
}

const QVariant VizGraphSettings::get(VizGraphParam p, VizGraphSettings* fallback) const {
  if (!values.contains(p))
    if (params[p].fallback != VIZ_NONE)
      return get(params[p].fallback, fallback);
    else if (fallback)
      return fallback->get(p);
    else
      return params[p].defaultvalue;
  else 
    return values[p];
}

QJsonObject VizGraphSettings::toJson() const {
  QJsonObject json;
  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    if (!params[p].saveable || values[p].isNull())
      continue;
    json.insert(params[p].name,QJsonValue::fromVariant(values[p]));
  }
  return json;
}

void VizGraphSettings::fromJson(const QJsonObject& json) {
  //for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
  for (auto it = json.begin(); it != json.end(); it++) {
    if (!nameToParamMap.contains(it.key())) {
      qWarning("Stylesheet contains unknown key \"%s\".", qUtf8Printable(it.key()));
      continue;
    }
    VizGraphParam p = nameToParamMap[it.key()];
    QVariant val = it.value().toVariant();
    if (!val.isNull()) {
      if (val.convert(params[p].type)) {
        values[p] = val;
      } else {
        qCritical("Could not convert value \"%s\" of stylesheet option \"%s\" to type \"%s\".",
                  qUtf8Printable(it.key()),
                  qUtf8Printable(params[p].name),
                  QMetaType::typeName(params[p].type));
      }
    }
  }
}

void VizGraphSettings::fromJsonFile(QString stylesheet) {
  QFile file;
  file.setFileName(stylesheet);
  if (!file.exists()) {
    qCritical("Could not find stylesheet file \"%s\".", qUtf8Printable(stylesheet));
  }
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical("Could not open stylesheet file \"%s\".", qUtf8Printable(stylesheet));
  }
  
  QString jsonstring = file.readAll();
  file.close();
  QJsonDocument sd = QJsonDocument::fromJson(jsonstring.toUtf8());
  if (sd.isNull()) {
    qCritical("File \"%s\" is not a valid stylesheet.", qUtf8Printable(stylesheet));
  }
  fromJson(sd.object());
}

/*VizGraphParamAttrib VizGraphSettings::getAttrib(VizGraphParam p) {
  switch(p) {
    case VIZ_SHOWSEGMENTLABELS:
      return {"seg-labels", "Add segment labels to the graph.", QVariant(true), true};
  }
  
  assert(1==0 && "Param undefined");  
}*/
