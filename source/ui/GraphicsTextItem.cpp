#include "GraphicsTextItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>

GraphicsTextItem::GraphicsTextItem(QGraphicsItem* parent, QGraphicsScene* scene) :
    QGraphicsTextItem(parent,scene)
{
    document_ = document();
    connect(document_, SIGNAL(contentsChanged()), SLOT(changingText()));
    QFont font;
    font.setFamily("Helvitica");
    setFont(font);
}

void GraphicsTextItem::setShapeIndex(int index)
{
    shapeIndex_ = index;
}

int GraphicsTextItem::shapeIndex()
{
    return shapeIndex_;
}

int GraphicsTextItem::type() const
{
    return Type;
}


void GraphicsTextItem::changingText()
{
    emit textChanged(shapeIndex_);
}
