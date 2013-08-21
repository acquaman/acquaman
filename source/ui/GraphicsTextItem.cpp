#include "GraphicsTextItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QCursor>
#include <QTextCursor>
#include <QDebug>

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
//    QCursor oldCursor = cursor();
    QTextCursor oldTextCursor = textCursor();
    emit textChanged(shapeIndex_);
    setTextCursor(oldTextCursor);
    //    setCursor(oldCursor);
}

void GraphicsTextItem::focusInEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusInEvent(event);
    qDebug()<<"GraphicsTextItem::focusInEvent------------------------------------------------------";
    emit gotFocus(shapeIndex_);
}
