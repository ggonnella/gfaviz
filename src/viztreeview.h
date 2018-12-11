#pragma once
#include <QTreeWidgetItem>

class VizElement;

enum VizTreeItemType {
  VIZTREE_TAG,
  VIZTREE_INFO,
  VIZTREE_ELEMENT,
  VIZTREE_ITEM
};

/* VizTreeItem is an element of the tree navigation pane */

class VizTreeItem : public QTreeWidgetItem {

  public:

    VizTreeItemType type;
    VizElement* element;
    bool filled;

    VizTreeItem(VizTreeItemType _type, QTreeWidgetItem* parent = NULL);

    static VizTreeItem* fromElement(
        QTreeWidgetItem* parent, VizElement* element);
    static VizTreeItem* fromInfo(
        QTreeWidgetItem* parent, QString key, QString value);
};
