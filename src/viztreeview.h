#pragma once

#include <QTreeWidgetItem>
class VizElement;

enum VizTreeItemType {
  VIZTREE_TAG,
  VIZTREE_INFO,
  VIZTREE_ELEMENT,
  VIZTREE_ITEM  
};

class VizTreeItem : public QTreeWidgetItem {
  public:
    VizTreeItem(VizTreeItemType _type, QTreeWidgetItem* parent = NULL);
    VizTreeItemType type;
    VizElement* element;
    bool filled;
    static VizTreeItem* fromElement(QTreeWidgetItem* parent, VizElement* element);
    static VizTreeItem* fromInfo(QTreeWidgetItem* parent, QString key, QString value);
};
