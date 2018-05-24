#pragma once
#include "headers.h"

#include <QtWidgets/QGraphicsTextItem>
//#include <QtWidgets/QGraphicsItem>

class VizGraph;
class VizElementLabel;
class GfaLine;


class VizElement {
  public:
    VizElement(VizGraph* _vg);
    ~VizElement();
    
    virtual void setHighlight(bool _val) = 0;
    
  protected:
    VizGraph* vg;
    VizElementLabel* labelItem;
    void drawLabel();
    virtual QPointF getCenterCoord() = 0;
    virtual GfaLine* getGfaElement() = 0;
    virtual QGraphicsItem* getMainGraphicsItem();
    
    
  private:
    
};

class VizElementGraphicsItem {
  public:
    VizElementGraphicsItem(VizElement* _parent);
    virtual void setHighlight(bool val) = 0;
    
  private:
    
  protected:
    VizElement* parent;
    void setHover(bool val);    
};

class VizElementLabel : public VizElementGraphicsItem, public QGraphicsTextItem {
  public:
    VizElementLabel(QString text, VizElement* _parent);
    virtual void setHighlight(bool val);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    
  protected:
    //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) { setHover(true); };
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) { setHover(false); };  
};
