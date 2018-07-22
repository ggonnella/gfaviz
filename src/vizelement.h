#pragma once
#include "headers.h"
#include "vizsettings.h"
#include <QGraphicsTextItem>
#include <QPen>
#include <QFont>

//#include <QtWidgets/QGraphicsItem>

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
    VizElementLabel(QString text, VizElement* _parent = 0);
    //virtual void setHighlight(bool val);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    void setHighlight(bool value);  
    void setStyle(const QString& family, double size, const QColor& color, double outlineWidth, const QColor& outlinecolor);
    
  protected:
    QFont font;
    QColor color;
    QPen outlinepen;
  
    VizElement* parent;
    
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    //virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    //virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
};

class VizElement : public QGraphicsPathItem {
  public:
    VizElement(VizElementType _type, VizGraph* _vg, GfaLine* line);
    ~VizElement();
    const QVariant getOption(VizGraphParam p) const;
    void setOption(VizGraphParam p, QVariant val, bool overwrite = true);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    void addGroup(VizGroup* group);
    const vector<VizGroup*>& getGroups() const;
    long unsigned int getGroupIndex(VizGroup* group);
    VizElementLabel* labelItem;
    
    virtual GfaLine* getGfaElement() = 0;
    VizElementType getType();
    static const QString getTypeName(VizElementType t);
    
    VizGraph* vg;
  protected:
    VizElementType type;
    VizGraphSettings settings;
    vector<VizGroup*> groups;
    
    void drawLabel();
    void setLabelText(const QString& text);
    
    virtual QPointF getCenterCoord() = 0;
    
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);  
    
  private:
    

};

