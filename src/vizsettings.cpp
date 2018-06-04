#include "vizsettings.h"
#include <assert.h>

std::vector<VizGraphParamAttrib> VizGraphSettings::params;

void VizGraphSettings::initParams() {
  params.resize(VIZ_LASTPARAM);
  params[VIZ_SHOWALLLABELS] = {"labels", "Add all labels to the graph.", QMetaType::Bool, QVariant(false), VIZ_NONE, true, true};
  params[VIZ_SHOWSEGMENTLABELS] = {"seg-labels", "Add segment labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS, true};
  params[VIZ_SHOWEDGELABELS] = {"edge-labels", "Add edge labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS, true};
  params[VIZ_SHOWGAPLABELS] = {"gap-labels", "Add gap labels to the graph.", QMetaType::Bool, QVariant(), VIZ_SHOWALLLABELS, true};
  params[VIZ_SEGMENTWIDTH] = {"seg-width", "Width of the segments.", QMetaType::Double, QVariant(4.0f), VIZ_NONE, true};
  params[VIZ_SEGMENTOUTLINEWIDTH] = {"seg-outline-width", "Width of the segment outline.", QMetaType::Double, QVariant(1.0f), VIZ_NONE, true};
  params[VIZ_SEGMENTMAINCOLOR] = {"seg-color", "Color of the segment.", QMetaType::QColor, QVariant("#0000ff"), VIZ_NONE, true};
  params[VIZ_SEGMENTOUTLINECOLOR] = {"seg-outline-color", "Color of the segment outline.", QMetaType::QColor, QVariant("#000000"), VIZ_NONE, true};
  params[VIZ_MINWEIGHT] = {"minweight", "", QMetaType::Double, QVariant((double)12.0f), VIZ_NONE, false};
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
  values.resize(VIZ_LASTPARAM);
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

const QVariant& VizGraphSettings::get(VizGraphParam p) const {
  if (values[p].isNull())
    if (params[p].fallback != VIZ_NONE)
      return get(params[p].fallback);
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
    QJsonValue value;
    if (values[p].canConvert(QMetaType::QJsonValue))
      value = values[p].toJsonValue();
    else
      value = QJsonValue(values[p].toString());
    json.insert(params[p].name,value);
  }
  return json;
}

/*VizGraphParamAttrib VizGraphSettings::getAttrib(VizGraphParam p) {
  switch(p) {
    case VIZ_SHOWSEGMENTLABELS:
      return {"seg-labels", "Add segment labels to the graph.", QVariant(true), true};
  }
  
  assert(1==0 && "Param undefined");  
}*/
