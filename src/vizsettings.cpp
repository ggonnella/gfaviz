#include "vizsettings.h"
#include <assert.h>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>

#define VIZ_GROUP_DEFAULTCOLORS "green,pink,red,orange"

std::vector<VizGraphParamAttrib> VizGraphSettings::params;
QMap<QString,VizGraphParam> VizGraphSettings::nameToParamMap;

VizGraphParamAttrib::VizGraphParamAttrib(const QString& _name, const QString& _description, QMetaType::Type _type, QVariant _defaultvalue, VizGraphParam _fallback, bool _userdefined, bool _saveable) { 
  name=_name;
  description=_description;
  type=_type;
  defaultvalue=_defaultvalue;
  fallback=_fallback;
  userdefined=_userdefined;
  saveable=_saveable;
  cmdOption=NULL;
}

void VizGraphSettings::initParams() {
  params.resize(VIZ_LASTPARAM);
  params[VIZ_BACKGROUNDCOLOR] = VizGraphParamAttrib("bg-color", "Background color.", QMetaType::QColor, QVariant("#00ffffff"));
  params[VIZ_SHOWALLLABELS] = VizGraphParamAttrib("labels", "Add all labels to the graph.", QMetaType::Bool, QVariant(false));
  params[VIZ_SHOWSEGMENTLABELS] = VizGraphParamAttrib("seg-labels", "Add segment labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS);
  params[VIZ_SHOWEDGELABELS] = VizGraphParamAttrib("edge-labels", "Add edge labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS);
  params[VIZ_SHOWGAPLABELS] = VizGraphParamAttrib("gap-labels", "Add gap labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS);
  params[VIZ_SHOWGROUPLABELS] = VizGraphParamAttrib("group-labels", "Add group labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS);
  params[VIZ_DISABLEGAPS] = VizGraphParamAttrib("no-gaps", "Do not show gaps in the graph.", QMetaType::Bool);
  params[VIZ_DISABLEFRAGMENTS] = VizGraphParamAttrib("no-fragments", "Do not show fragments in the graph.", QMetaType::Bool);
  params[VIZ_DISABLEGROUPS] = VizGraphParamAttrib("no-groups", "Do not show groups in the graph.", QMetaType::Bool);
  params[VIZ_SEGMENTWIDTH] = VizGraphParamAttrib("seg-width", "Width of the segments.", QMetaType::Double, QVariant(4.0f));
  params[VIZ_SEGMENTOUTLINEWIDTH] = VizGraphParamAttrib("seg-outline-width", "Width of the segment outline.", QMetaType::Double, QVariant(1.0f));
  params[VIZ_SEGMENTMAINCOLOR] = VizGraphParamAttrib("seg-color", "Color of the segment.", QMetaType::QColor, QVariant("#0000ff"));
  params[VIZ_SEGMENTOUTLINECOLOR] = VizGraphParamAttrib("seg-outline-color", "Color of the segment outline.", QMetaType::QColor, QVariant("#000000"));
  params[VIZ_EDGEWIDTH] = VizGraphParamAttrib("edge-width", "Width of the links/edges.", QMetaType::Double, QVariant(2.0f));
  params[VIZ_EDGECOLOR] = VizGraphParamAttrib("edge-color", "Color of the links/edges.", QMetaType::QColor, QVariant("#000000"), VIZ_NONE);
  params[VIZ_DOVETAILWIDTH] = VizGraphParamAttrib("dovetail-width", "Width of dovetail links.", QMetaType::Double, QVariant(2.0f), VIZ_EDGEWIDTH);
  params[VIZ_DOVETAILCOLOR] = VizGraphParamAttrib("dovetail-color", "Color of dovetail links.", QMetaType::QColor, QVariant("#000000"), VIZ_EDGECOLOR);
  params[VIZ_INTERNALWIDTH] = VizGraphParamAttrib("internal-width", "Width of non-dovetail links.", QMetaType::Double, QVariant(2.0f), VIZ_EDGEWIDTH);
  params[VIZ_INTERNALCOLOR] = VizGraphParamAttrib("internal-color", "Color of non-dovetail links.", QMetaType::QColor, QVariant("#000000"), VIZ_EDGECOLOR);
  params[VIZ_GROUPWIDTH] = VizGraphParamAttrib("group-width", "Width of the groups.", QMetaType::Double, QVariant(2.5f));
  params[VIZ_GROUPCOLORS] = VizGraphParamAttrib("group-colors", "Colors of the groups, separated by commas.", QMetaType::QString, QVariant(VIZ_GROUP_DEFAULTCOLORS));
  params[VIZ_GROUPCOLOR] = VizGraphParamAttrib("group-color", "Color of the groups", QMetaType::QColor, QVariant("red"), VIZ_NONE, false);
  params[VIZ_GAPCOLOR] = VizGraphParamAttrib("gap-color", "Color of the gaps.", QMetaType::QColor, QVariant("gray"));
  params[VIZ_LABELFONT] = VizGraphParamAttrib("label-font", "Font family of the labels.", QMetaType::QString, QVariant("Arial"));
  params[VIZ_LABELFONTSIZE] = VizGraphParamAttrib("label-size", "Font point size of the labels.", QMetaType::Double, QVariant(12.0f));
  params[VIZ_LABELCOLOR] = VizGraphParamAttrib("label-color", "Font color of the labels.", QMetaType::QColor, QVariant("#000000"));
  params[VIZ_LABELOUTLINECOLOR] = VizGraphParamAttrib("label-outline-color", "Font outline color of the labels.", QMetaType::QColor, QVariant("#ffffff"));
  params[VIZ_LABELOUTLINEWIDTH] = VizGraphParamAttrib("label-outline-width", "Font outline width of the labels.", QMetaType::Double, QVariant(2.0f));
  params[VIZ_SEGLABELSHOWLENGTH] = VizGraphParamAttrib("seg-label-showlength", "Show segment length in label.", QMetaType::Bool, QVariant(false));
  //params[VIZ_EDGELABELSHOWLENGTH] = VizGraphParamAttrib("edge-label-showlength", "Show edge length in label.", QMetaType::Bool, QVariant(false), VIZ_NONE, true, true};
  params[VIZ_MINWEIGHT] = VizGraphParamAttrib("minweight", "", QMetaType::Double, QVariant((double)12.0f), VIZ_NONE, false);
  params[VIZ_USEFMMM] = VizGraphParamAttrib("fmmm", "Use the fmmm-graph layouting algorithm, which is faster than the standard one.", QMetaType::Bool, QVariant(false));
  
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
  /* First check in local settings */
  VizGraphParam param = p;
  while (param != VIZ_NONE) {
    if (values.contains(param)) {
      return values[param];
    }
    param = params[param].fallback;
  }
  /* Else, search in parent settings */
  if (fallback)
    return fallback->get(p);
  /* Else, return default value */
  return params[p].defaultvalue;
}
void VizGraphSettings::set(VizGraphParam p, QVariant val, bool overwrite) {
  if (val.convert(params[p].type)) {
    if (overwrite || !values.contains(p))
      values[p] = val;
  } else {
    qCritical("Could not convert value \"%s\" of option \"%s\" to type \"%s\".",
                    qUtf8Printable(val.toString()),
                    qUtf8Printable(params[p].name),
                    QMetaType::typeName(params[p].type));
  }
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
