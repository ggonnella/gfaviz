#pragma once
#include <QTreeWidgetItem>

#include "vizelement.h"

enum VizTreeItemType {
  VIZTREE_TAG,
  VIZTREE_INFO,
  VIZTREE_ELEMENT,
  VIZTREE_ITEM,
  VIZTREE_CATEGORY,
  VIZTREE_CURRENTSELECTION
};

/* VizTreeItem is an element of the tree navigation pane */

class VizTreeItem : public QTreeWidgetItem {

  public:

    VizTreeItemType type;
    union {
      VizElement* element; // Only for items of type VIZTREE_ELEMENT
      VizElementType elementtype; // Only for items of type VIZTREE_CATEGORY
                                  // and VIZTREE_CURRENTSELECTION
    };
    bool filled;

    VizTreeItem(VizTreeItemType _type, QTreeWidgetItem* parent = NULL);

    static VizTreeItem* fromElement(
        QTreeWidgetItem* parent, VizElement* element);
    static VizTreeItem* fromInfo(
        QTreeWidgetItem* parent, QString key, QString value);
};
