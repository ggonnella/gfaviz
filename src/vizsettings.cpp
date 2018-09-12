#include "vizsettings.h"
#include <assert.h>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>

#define VIZ_GROUP_DEFAULTCOLORS "green,pink,red,orange"

std::vector<VizGraphParamAttrib> VizGraphSettings::params;
QMap<QString,VizGraphParam> VizGraphSettings::nameToParamMap;
QMap<VizGraphParam,VizGraphParam> VizGraphSettings::dependentParams;

VizGraphParamAttrib::VizGraphParamAttrib(const QString& _name, const QString& _description, QMetaType::Type _type, VizGraphParam _fallback, QVariant _defaultvalue, bool _userdefined, bool _saveable) { 
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
  params[VIZ_BACKGROUNDCOLOR] = VizGraphParamAttrib("bg-color", "Background color.", QMetaType::QColor, VIZ_NONE, QVariant("#00ffffff"));
  params[VIZ_LABELSHOW] = VizGraphParamAttrib("labels", "Add all labels to the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_SEGLABELSHOW] = VizGraphParamAttrib("seg-labels", "Add segment labels to the graph.", QMetaType::Bool, VIZ_LABELSHOW);
  params[VIZ_EDGELABELSHOW] = VizGraphParamAttrib("edge-labels", "Add edge labels to the graph.", QMetaType::Bool, VIZ_LABELSHOW);
  params[VIZ_GAPLABELSHOW] = VizGraphParamAttrib("gap-labels", "Add gap labels to the graph.", QMetaType::Bool, VIZ_LABELSHOW);
  params[VIZ_GROUPLABELSHOW] = VizGraphParamAttrib("group-labels", "Add group labels to the graph.", QMetaType::Bool, VIZ_LABELSHOW);
  params[VIZ_FRAGMENTLABELSHOW] = VizGraphParamAttrib("fragment-labels", "Add fragment labels to the graph.", QMetaType::Bool, VIZ_LABELSHOW);
  params[VIZ_DISABLESEGMENTS] = VizGraphParamAttrib("no-segments", "Do not show segments in the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false), false);
  params[VIZ_DISABLEEDGES] = VizGraphParamAttrib("no-edges", "Do not show edges in the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false), false);
  params[VIZ_DISABLEGROUPS] = VizGraphParamAttrib("no-groups", "Do not show groups in the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_DISABLEGAPS] = VizGraphParamAttrib("no-gaps", "Do not show gaps in the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_DISABLEFRAGMENTS] = VizGraphParamAttrib("no-fragments", "Do not show fragments in the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_DISABLEGROUPS] = VizGraphParamAttrib("no-groups", "Do not show groups in the graph.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_SEGMENTWIDTH] = VizGraphParamAttrib("seg-width", "Width of the segments.", QMetaType::Double, VIZ_NONE, QVariant(2.0f));
  params[VIZ_SEGMENTOUTLINEWIDTH] = VizGraphParamAttrib("seg-outline-width", "Width of the segment outline.", QMetaType::Double, VIZ_NONE, QVariant(0.5f));
  params[VIZ_SEGMENTMAINCOLOR] = VizGraphParamAttrib("seg-color", "Color of the segment.", QMetaType::QColor, VIZ_NONE, QVariant("#e9ffdd"));
  params[VIZ_SEGMENTMAXSUB] = VizGraphParamAttrib("seg-max-sub", "Maximum number of subsegments in segment representation.", QMetaType::UInt, VIZ_NONE, QVariant(10));
  params[VIZ_SEGMENTOUTLINECOLOR] = VizGraphParamAttrib("seg-outline-color", "Color of the segment outline.", QMetaType::QColor, VIZ_NONE, QVariant("#000000"));
  params[VIZ_EDGEWIDTH] = VizGraphParamAttrib("edge-width", "Width of the links/edges.", QMetaType::Double, VIZ_NONE, QVariant(1.0f));
  params[VIZ_EDGECOLOR] = VizGraphParamAttrib("edge-color", "Color of the links/edges.", QMetaType::QColor, VIZ_NONE, QVariant("#000000"));
  params[VIZ_EDGEHIGHLIGHTSHOW] = VizGraphParamAttrib("edge-highlights-show", "Always show highlight of the overlapped parts of edges on segments.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_EDGEHIGHLIGHTCOLOR] = VizGraphParamAttrib("edge-highlights-color", "Color of the edge-highlights on segments. Tip: use low alpha value.", QMetaType::QColor, VIZ_NONE, QVariant("#32ff0000"));
  params[VIZ_DOVETAILLENGTH] = VizGraphParamAttrib("dovetail-length", "Length of dovetail links.", QMetaType::Double, VIZ_NONE, QVariant(10.0));
  params[VIZ_DOVETAILWIDTH] = VizGraphParamAttrib("dovetail-width", "Width of dovetail links.", QMetaType::Double, VIZ_EDGEWIDTH);
  params[VIZ_DOVETAILCOLOR] = VizGraphParamAttrib("dovetail-color", "Color of dovetail links.", QMetaType::QColor, VIZ_EDGECOLOR);
  params[VIZ_INTERNALWIDTH] = VizGraphParamAttrib("internal-width", "Width of non-dovetail links.", QMetaType::Double, VIZ_EDGEWIDTH);
  params[VIZ_INTERNALLENGTH] = VizGraphParamAttrib("internal-length", "Length of internal links.", QMetaType::Double, VIZ_NONE, QVariant(80.0));
  params[VIZ_INTERNALCOLOR] = VizGraphParamAttrib("internal-color", "Color of non-dovetail links.", QMetaType::QColor, VIZ_EDGECOLOR);
  params[VIZ_GROUPWIDTH] = VizGraphParamAttrib("group-width", "Width of the groups.", QMetaType::Double, VIZ_NONE, QVariant(1.2f));
  params[VIZ_GROUPCOLORS] = VizGraphParamAttrib("group-colors", "Colors of the groups, separated by commas.", QMetaType::QString, VIZ_NONE, QVariant(VIZ_GROUP_DEFAULTCOLORS));
  params[VIZ_GROUPCOLOR] = VizGraphParamAttrib("group-color", "Color of the groups", QMetaType::QColor, VIZ_NONE, QVariant("red"), false);
  params[VIZ_GAPWIDTH] = VizGraphParamAttrib("gap-width", "Width of the gaps.", QMetaType::Double, VIZ_NONE, QVariant(1.0f));
  params[VIZ_GAPSEDGES] = VizGraphParamAttrib("gaps-as-edges", "Use gaps as edges in layout computation.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_GAPLENGTH] = VizGraphParamAttrib("gap-length", "Gap length factor if --gaps-as-edges is set.", QMetaType::Double, VIZ_DOVETAILLENGTH);
  params[VIZ_POSGAPCOLOR] = VizGraphParamAttrib("gap-color", "Color of positive-length gaps.", QMetaType::QColor, VIZ_NONE, QVariant("#668074"));
  params[VIZ_NEGGAPCOLOR] = VizGraphParamAttrib("neg-gap-color", "Color of negative-length gaps.", QMetaType::QColor, VIZ_NONE, QVariant("#c09090"));
  params[VIZ_FRAGMENTWIDTH] = VizGraphParamAttrib("fragment-width", "Width of the fragments.", QMetaType::Double, VIZ_NONE, QVariant(1.0f));
  params[VIZ_FRAGMENTCOLOR] = VizGraphParamAttrib("fragment-color", "Color of the fragments.", QMetaType::QColor, VIZ_NONE);
  params[VIZ_FRAGMENTMINLENGTH] = VizGraphParamAttrib("fragment-minlength", "Min length of fragment representation.", QMetaType::Double, VIZ_NONE, QVariant(0.1f));
  params[VIZ_FRAGMENTMULTLENGTH] = VizGraphParamAttrib("fragment-multlength", "Multiplier of fragment length.", QMetaType::Double, VIZ_NONE, QVariant(1.0f));
  params[VIZ_FWDFRAGMENTCOLOR] = VizGraphParamAttrib("fwd-fragment-color", "Color of the fragments in fwd orientation.", QMetaType::QColor, VIZ_FRAGMENTCOLOR, QVariant("#009000"));
  params[VIZ_REVFRAGMENTCOLOR] = VizGraphParamAttrib("rev-fragment-color", "Color of the fragments in rev orientation.", QMetaType::QColor, VIZ_FRAGMENTCOLOR, QVariant("#cf0000"));
  params[VIZ_FRAGMENTHIGHLIGHTSHOW] = VizGraphParamAttrib("fragment-highlights-show", "Always show highlight of the overlapped parts of fragments on segments.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  params[VIZ_FRAGMENTHIGHLIGHTCOLOR] = VizGraphParamAttrib("fragment-highlights-color", "Color of the fragment-highlights on segments. Tip: use low alpha value.", QMetaType::QColor, VIZ_NONE, QVariant("#32ff00ff"));
  params[VIZ_LABELFONT] = VizGraphParamAttrib("label-font", "Font family of all labels.", QMetaType::QFont, VIZ_NONE, QVariant("Arial"));
  params[VIZ_LABELFONTSIZE] = VizGraphParamAttrib("label-size", "Font point size of all labels.", QMetaType::Double, VIZ_NONE, QVariant(4.5f));
  params[VIZ_LABELCOLOR] = VizGraphParamAttrib("label-color", "Font color of all labels.", QMetaType::QColor, VIZ_NONE, QVariant("#000000"));
  params[VIZ_LABELOUTLINECOLOR] = VizGraphParamAttrib("label-outline-color", "Font outline color of all labels.", QMetaType::QColor, VIZ_NONE, QVariant("#ffffff"));
  params[VIZ_LABELOUTLINEWIDTH] = VizGraphParamAttrib("label-outline-width", "Font outline width of all labels.", QMetaType::Double, VIZ_NONE, QVariant(1.0f));
  
  params[VIZ_SEGLABELFONT] = VizGraphParamAttrib("seg-label-font", "Font family of the segment labels.", QMetaType::QFont, VIZ_LABELFONT);
  params[VIZ_SEGLABELFONTSIZE] = VizGraphParamAttrib("seg-label-size", "Font point size of the segment labels.", QMetaType::Double, VIZ_LABELFONTSIZE);
  params[VIZ_SEGLABELCOLOR] = VizGraphParamAttrib("seg-label-color", "Font color of the segment labels.", QMetaType::QColor, VIZ_LABELCOLOR);
  params[VIZ_SEGLABELOUTLINECOLOR] = VizGraphParamAttrib("seg-label-outline-color", "Font outline color of the segment labels.", QMetaType::QColor, VIZ_LABELOUTLINECOLOR);
  params[VIZ_SEGLABELOUTLINEWIDTH] = VizGraphParamAttrib("seg-label-outline-width", "Font outline width of the segment labels.", QMetaType::Double, VIZ_LABELOUTLINEWIDTH);
  
  params[VIZ_EDGELABELFONT] = VizGraphParamAttrib("edge-label-font", "Font family of the edge labels.", QMetaType::QFont, VIZ_LABELFONT);
  params[VIZ_EDGELABELFONTSIZE] = VizGraphParamAttrib("edge-label-size", "Font point size of the edge labels.", QMetaType::Double, VIZ_LABELFONTSIZE);
  params[VIZ_EDGELABELCOLOR] = VizGraphParamAttrib("edge-label-color", "Font color of the edge labels.", QMetaType::QColor, VIZ_LABELCOLOR);
  params[VIZ_EDGELABELOUTLINECOLOR] = VizGraphParamAttrib("edge-label-outline-color", "Font outline color of the edge labels.", QMetaType::QColor, VIZ_LABELOUTLINECOLOR);
  params[VIZ_EDGELABELOUTLINEWIDTH] = VizGraphParamAttrib("edge-label-outline-width", "Font outline width of the edge labels.", QMetaType::Double, VIZ_LABELOUTLINEWIDTH);
  
  params[VIZ_GROUPLABELFONT] = VizGraphParamAttrib("group-label-font", "Font family of the group labels.", QMetaType::QFont, VIZ_LABELFONT);
  params[VIZ_GROUPLABELFONTSIZE] = VizGraphParamAttrib("group-label-size", "Font point size of the group labels.", QMetaType::Double, VIZ_LABELFONTSIZE);
  params[VIZ_GROUPLABELCOLOR] = VizGraphParamAttrib("group-label-color", "Font color of the group labels.", QMetaType::QColor, VIZ_LABELCOLOR);
  params[VIZ_GROUPLABELOUTLINECOLOR] = VizGraphParamAttrib("group-label-outline-color", "Font outline color of the group labels.", QMetaType::QColor, VIZ_LABELOUTLINECOLOR);
  params[VIZ_GROUPLABELOUTLINEWIDTH] = VizGraphParamAttrib("group-label-outline-width", "Font outline width of the group labels.", QMetaType::Double, VIZ_LABELOUTLINEWIDTH);
  
  params[VIZ_GAPLABELFONT] = VizGraphParamAttrib("gap-label-font", "Font family of the gap labels.", QMetaType::QFont, VIZ_LABELFONT);
  params[VIZ_GAPLABELFONTSIZE] = VizGraphParamAttrib("gap-label-size", "Font point size of the gap labels.", QMetaType::Double, VIZ_LABELFONTSIZE);
  params[VIZ_GAPLABELCOLOR] = VizGraphParamAttrib("gap-label-color", "Font color of the gap labels.", QMetaType::QColor, VIZ_LABELCOLOR);
  params[VIZ_GAPLABELOUTLINECOLOR] = VizGraphParamAttrib("gap-label-outline-color", "Font outline color of the gap labels.", QMetaType::QColor, VIZ_LABELOUTLINECOLOR);
  params[VIZ_GAPLABELOUTLINEWIDTH] = VizGraphParamAttrib("gap-label-outline-width", "Font outline width of the gap labels.", QMetaType::Double, VIZ_LABELOUTLINEWIDTH);
  
  params[VIZ_FRAGMENTLABELFONT] = VizGraphParamAttrib("frag-label-font", "Font family of the fragment labels.", QMetaType::QFont, VIZ_LABELFONT);
  params[VIZ_FRAGMENTLABELFONTSIZE] = VizGraphParamAttrib("frag-label-size", "Font point size of the fragment labels.", QMetaType::Double, VIZ_LABELFONTSIZE);
  params[VIZ_FRAGMENTLABELCOLOR] = VizGraphParamAttrib("frag-label-color", "Font color of the fragment labels.", QMetaType::QColor, VIZ_LABELCOLOR);
  params[VIZ_FRAGMENTLABELOUTLINECOLOR] = VizGraphParamAttrib("frag-label-outline-color", "Font outline color of the fragment labels.", QMetaType::QColor, VIZ_LABELOUTLINECOLOR);
  params[VIZ_FRAGMENTLABELOUTLINEWIDTH] = VizGraphParamAttrib("frag-label-outline-width", "Font outline width of the fragment labels.", QMetaType::Double, VIZ_LABELOUTLINEWIDTH);
  
  params[VIZ_SEGLABELSHOWLENGTH] = VizGraphParamAttrib("seg-label-showlength", "Show segment length in label.", QMetaType::Bool, VIZ_NONE, QVariant(false));
  //params[VIZ_EDGELABELSHOWLENGTH] = VizGraphParamAttrib("edge-label-showlength", "Show edge length in label.", QMetaType::Bool, QVariant(false), VIZ_NONE, true, true};
  params[VIZ_MINWEIGHT] = VizGraphParamAttrib("minweight", "Minimum length of fragments and segments, expressed in fraction of the longest segment length divided by the max number of subsegments.", QMetaType::Double, VIZ_NONE, QVariant((double)0.05f), true, false);
  params[VIZ_WEIGHTFACTOR] = VizGraphParamAttrib("weight-factor", "Weight factor for the computation of segments and fragment lengths.", QMetaType::Double, VIZ_NONE, QVariant((double)1.0f), true, false);
  params[VIZ_SM_WEIGHTFACTOR] = VizGraphParamAttrib("sm-weight-factor", "Layout-specific multiplier of weight factor for the SM layout.", QMetaType::Double, VIZ_NONE, QVariant((double)15.0f), true, false);
  params[VIZ_FMMM_WEIGHTFACTOR] = VizGraphParamAttrib("fmmm-weight-factor", "Layout-specific multiplier of weight factor for the FMMM layout.", QMetaType::Double, VIZ_NONE, QVariant((double)0.8f), true, false);
  
  params[VIZ_USEFMMM] = VizGraphParamAttrib("fmmm", "Use the fmmm-graph layouting algorithm, which is faster than the standard one.", QMetaType::Bool, VIZ_NONE, QVariant(false),true,false);
  
  
  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    params[p].description += "\nDefault: " + getDefault(p).toString();
  }
  
  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    nameToParamMap.insert(params[p].name, p);
  }
  for (VizGraphParam p = (VizGraphParam)0; p < VIZ_LASTPARAM; p = (VizGraphParam)(p+1)) {
    if (params[p].fallback != VIZ_NONE) {
      dependentParams.insertMulti(params[p].fallback, p);
    }
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
        set(p, val, false);
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
  return getDefault(p);
}
const QVariant VizGraphSettings::getDefault(VizGraphParam p) {
  VizGraphParam param = p;
  while (param != VIZ_NONE) {
    if (!params[param].defaultvalue.isNull()) {
      return params[param].defaultvalue;
    }
    param = params[param].fallback;
  }
  qWarning("Could not find default value for parameter \"%s\".", qUtf8Printable(params[p].name));
  return QVariant();
}
void VizGraphSettings::set(VizGraphParam p, QVariant val, bool overwrite) {
  if (val.convert(params[p].type)) {
    if (overwrite) {
      unset(p);
    }
    values[p] = val;
  } else {
    qCritical("Could not convert value \"%s\" of option \"%s\" to type \"%s\".",
                    qUtf8Printable(val.toString()),
                    qUtf8Printable(params[p].name),
                    QMetaType::typeName(params[p].type));
  }
}
void VizGraphSettings::unset(VizGraphParam p) {
  values.remove(p);
  if (dependentParams.contains(p)) {
    QList<VizGraphParam> dependents = dependentParams.values(p);
    for (int i = 0; i < dependents.size(); i++) {
      unset(dependents[i]);
    }
  }
}
void VizGraphSettings::unsetAll() {
  values.clear();
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
  //unsetAll();
  for (auto it = json.begin(); it != json.end(); it++) {
    if (!nameToParamMap.contains(it.key())) {
      qWarning("Stylesheet contains unknown key \"%s\".", qUtf8Printable(it.key()));
      continue;
    }
    VizGraphParam p = nameToParamMap[it.key()];
    set(p, it.value().toVariant(),false);
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

int VizGraphSettings::size() {
  return values.size();
}
/*VizGraphParamAttrib VizGraphSettings::getAttrib(VizGraphParam p) {
  switch(p) {
    case VIZ_SHOWSEGMENTLABELS:
      return {"seg-labels", "Add segment labels to the graph.", QVariant(true), true};
  }
  
  assert(1==0 && "Param undefined");  
}*/
