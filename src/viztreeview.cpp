#include "viztreeview.h"
#include "vizelement.h"
#include "vizgraph.h"
#include "viznode.h"

VizTreeItem::VizTreeItem(VizTreeItemType _type, QTreeWidgetItem* parent) : QTreeWidgetItem(parent) {
  type = _type;
  element = NULL;
  filled = false;
}

VizTreeItem* VizTreeItem::fromElement(QTreeWidgetItem* parent, VizElement* element) {
  VizTreeItem *item = new VizTreeItem(VIZTREE_ELEMENT, parent);
  if (element->getGfaElement()->hasName()) {
    item->setText(0, QString::fromStdString(element->getGfaElement()->getName()));
  } else {
    item->setText(0, "[unnamed]");
  }
  item->setText(1, VizElement::getTypeName(element->getType()));
  QFont font = item->font(0);
  font.setBold(true);
  item->setFont(0,font);
  item->element = element;
  item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  return item;
  //it.second->addTreeViewInfo(item);
}
VizTreeItem* VizTreeItem::fromInfo(QTreeWidgetItem* parent, QString key, QString value) {
  VizTreeItem *item = new VizTreeItem(VIZTREE_INFO, parent);
  item->setText(0, key);
  item->setText(1, value);
  item->setToolTip(1, value);
  return item;
}

void VizGraph::fillTreeView() {
  VizTreeItem *header = new VizTreeItem(VIZTREE_ITEM, (QTreeWidgetItem*)NULL);
  header->setText(0, "headers");
  VizTreeItem *tagsitem = new VizTreeItem(VIZTREE_ITEM, header);
  tagsitem->setText(0, "tags");
  tagsitem->setText(1, QString::number(gfa->getTags().size()));
  for (GfaTag* tag : gfa->getTags()) {
    VizTreeItem *tagitem = new VizTreeItem(VIZTREE_TAG, tagsitem);
    tagitem->setText(0, QString::fromUtf8(tag->getKey(),2) + " (" + (char)tag->getType() + ")");
    tagitem->setText(1, QString::fromStdString(tag->asString()));
    tagitem->setToolTip(1, QString::fromStdString(tag->asString()));
  }
  form.treeWidget->addTopLevelItem(header);
  
  for (int idx = 0; idx < (int)VIZ_ELEMENTUNKNOWN; idx++) {
    VizTreeItem *parent = new VizTreeItem(VIZTREE_ITEM, (QTreeWidgetItem*)NULL);
    parent->setText(0, VizElement::getTypeName((VizElementType)idx) + "s");
    parent->setText(1, QString::number(elements[idx].size()) + " elements");
    for (auto it : elements[idx]) {
      VizTreeItem::fromElement(parent, it.second);
    }
    form.treeWidget->addTopLevelItem(parent);
  }
  connect(form.treeWidget, &QTreeWidget::itemExpanded, this, &VizGraph::treeViewItemExpanded);
  connect(form.treeWidget, &QTreeWidget::itemClicked, this, &VizGraph::treeViewItemClicked);

}

void VizGraph::treeViewItemExpanded(QTreeWidgetItem* _item) {
  VizTreeItem* item = (VizTreeItem*)_item;
  if (item->type == VIZTREE_ELEMENT && !item->filled) {
    item->element->addTreeViewInfo(item);
    VizTreeItem *tagsitem = new VizTreeItem(VIZTREE_ITEM, item);
    tagsitem->setText(0, "tags");
    tagsitem->setText(1, QString::number(item->element->getGfaElement()->getTags().size()));
    for (GfaTag* tag : item->element->getGfaElement()->getTags()) {
      VizTreeItem *tagitem = new VizTreeItem(VIZTREE_TAG, tagsitem);
      tagitem->setText(0, QString::fromUtf8(tag->getKey(),2) + " (" + (char)tag->getType() + ")");
      tagitem->setText(1, QString::fromStdString(tag->asString()));
      tagitem->setToolTip(1, QString::fromStdString(tag->asString()));
    }
    item->filled = true;
  }
}
void VizGraph::treeViewItemClicked(QTreeWidgetItem* _item, int column) {
  VizTreeItem* item = (VizTreeItem*)_item;
  if (item->type == VIZTREE_ELEMENT) {
    scene->clearSelection();
    item->element->setSelected(true);
  }
}

void VizNode::addTreeViewInfo(VizTreeItem* parent) {
  if (gfa_node->isLengthSet())
    VizTreeItem::fromInfo(parent, "length", QString::number(gfa_node->getLength()) + " bp");

  if (gfa_node->getSequence().isSet())
    VizTreeItem::fromInfo(parent, "sequence", QString::fromStdString(gfa_node->getSequence().getString()));
  
  int num;
  num = gfa_node->getInedges().size();
  if (num > 0) {
    VizTreeItem* item = VizTreeItem::fromInfo(parent, "inedges", QString::number(num));
    for (GfaEdge* e : gfa_node->getInedges()) {
      VizTreeItem::fromElement(item, vg->getEdge(e));
    }
  }
  num = gfa_node->getOutedges().size();
  if (num > 0) {
    VizTreeItem* item = VizTreeItem::fromInfo(parent, "outedges", QString::number(num));
    for (GfaEdge* e : gfa_node->getOutedges()) {
      VizTreeItem::fromElement(item, vg->getEdge(e));
    }
  }
  num = gfa_node->getGroups().size();
  if (num > 0) {
    VizTreeItem* item = VizTreeItem::fromInfo(parent, "groups", QString::number(num));
    for (GfaGroup* g : gfa_node->getGroups()) {
      VizTreeItem::fromElement(item, vg->getGroup(g));
    }
  }
  num = gfa_node->getGaps().size();
  if (num > 0) {
    VizTreeItem* item = VizTreeItem::fromInfo(parent, "gaps", QString::number(num));
    for (GfaGap* g : gfa_node->getGaps()) {
      VizTreeItem::fromElement(item, vg->getGap(g));
    }
  }
  num = gfa_node->getFragments().size();
  if (num > 0) {
    VizTreeItem* item = VizTreeItem::fromInfo(parent, "fragments", QString::number(num));
    for (GfaFragment* f : gfa_node->getFragments()) {
      VizTreeItem::fromElement(item, vg->getFragment(f));
    }
  }
}

void VizEdge::addTreeViewInfo(VizTreeItem* parent) {
  VizTreeItem::fromInfo(parent, "dovetail?", (gfa_edge->isDovetail() ? "yes" : "no"));

  if (gfa_edge->getAlignment().isSet())
    VizTreeItem::fromInfo(parent, "alignment length", QString::number(gfa_edge->getAlignment().getLength()));

  VizTreeItem* segitem = VizTreeItem::fromInfo(parent, "connected segments", "");
  VizTreeItem::fromElement(segitem, viz_nodes[0]);
  VizTreeItem::fromElement(segitem, viz_nodes[1]);
}

void VizGroup::addTreeViewInfo(VizTreeItem* parent) {
  VizTreeItem::fromInfo(parent, "ordered group?", (gfa_group->getTypeOriginal() == 'U' ? "no" : "yes"));
  int num = gfa_group->getMembers().size();
  VizTreeItem* item = VizTreeItem::fromInfo(parent, "members", QString::number(num));
  for (GfaLine* e : gfa_group->getMembers()) {
    VizTreeItem::fromElement(item, vg->getElement(e));
  }
}

void VizGap::addTreeViewInfo(VizTreeItem* parent) {
  VizTreeItem::fromInfo(parent, "distance", QString::number(gfa_gap->getDist()) + " bp");
  VizTreeItem* segitem = VizTreeItem::fromInfo(parent, "connected segments", "");
  VizTreeItem::fromElement(segitem, viz_nodes[0]);
  VizTreeItem::fromElement(segitem, viz_nodes[1]);
}

void VizFragment::addTreeViewInfo(VizTreeItem* parent) {
  VizTreeItem::fromInfo(parent, "name", QString::fromStdString(gfa_fragment->getFragment()));
  
  VizTreeItem* segitem = VizTreeItem::fromInfo(parent, "segment", "");
  VizTreeItem::fromElement(segitem, viz_node);
}
