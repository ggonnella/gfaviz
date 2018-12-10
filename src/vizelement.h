#pragma once
#include "headers.h"
#include "vizsettings.h"
#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>
#include <QTreeWidgetItem>

/* Generic classes to handle the graphical elements of the graph representation
   (nodes and edges) and their text labels.

   VizElement is a GFA element which has a graphical representation
   (segment, edge, group, gap or fragment).

   VizElementLabel is the (optional) text label of a VizElement.
*/

class VizTreeItem;
class VizGraph;
class VizElement;
class GfaLine;
class VizGroup;

enum VizElementType {
  VIZ_SEGMENT = 0,
  VIZ_EDGE,
  VIZ_GROUP,
  VIZ_GAP,
  VIZ_FRAGMENT,
  VIZ_ELEMENTUNKNOWN //last element
};

class VizElementLabel : public QGraphicsTextItem {

  public:
    QPointF offset;

    VizElementLabel(QString text, VizElement* _parent = 0);
    virtual void paint(QPainter * painter,
        const QStyleOptionGraphicsItem * option, QWidget * widget);
    void setHighlight(bool value);
    void setStyle(const QString& family, double size, const QColor& color,
        double outlineWidth, const QColor& outlineColor);
    void setText(QString _str);
    void setOffset(QPointF _offset);
    void setCenter(QPointF _center);

  protected:
    QString str;
    QPointF center;
    bool centerChanged;
    QFont font;
    QColor color;
    QPen outlinepen;
    VizElement* parent;

    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

    // Other possible events, currently not handled:
    //virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
};

class VizElement : public QGraphicsPathItem {
  public:
    VizElement(VizElementType _type, VizGraph* _vg, GfaLine* line);
    ~VizElement();

    VizGraph* vg; /* Graph to which the elements belongs */

    virtual void draw() = 0;
    virtual void initOgdf() = 0;
    virtual void addTreeViewInfo(VizTreeItem* parentItem) {};

    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget * widget);
    virtual QRectF boundingRect() const;
    virtual bool collidesWithPath(const QPainterPath &path,
                                  Qt::ItemSelectionMode mode =
                                    Qt::IntersectsItemShape) const;

    /* Options */
    const QVariant getOption(VizGraphParam p) const;
    bool hasOption(VizGraphParam p);
    void setOption(VizGraphParam p, QVariant val, bool overwrite = true);
    void unsetOption(VizGraphParam p);
    void unsetAllOptions();

    /* Groups */
    void addGroup(VizGroup* group);
    const vector<VizGroup*>& getGroups() const;
    long unsigned int getGroupIndex(VizGroup* group);

    /* Label */
    VizElementLabel* labelItem;
    void setLabelVisible(bool value);

    /* Underlying Gfa line */
    virtual GfaLine* getGfaElement() = 0;
    VizElementType getType();
    static const QString getTypeName(VizElementType t);

    /* Style */
    void saveStyle();

    /* Layout */
    virtual QJsonObject getLayoutData();
    void saveLayout();
    void resetLayout();
    QJsonArray readLayoutData(QString key);

  protected:
    VizElementType type;
    VizGraphSettings settings;
    vector<VizGroup*> groups;

    virtual QPointF getCenterCoord() = 0;
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    /* Label */
    void drawLabel(const QString& family, double size,
                   const QColor& color, double outlineWidth,
                   const QColor& outlineColor);
    void setLabelText(const QString& text);

    /* Mouse events */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

  private:

};

