#include "AMGraphicsTextItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QCursor>
#include <QTextCursor>
#include <QDebug>

AMGraphicsTextItem::AMGraphicsTextItem(QGraphicsItem* parent, QGraphicsScene* scene) :
    QGraphicsTextItem(parent,scene)
{
    document_ = document();
    connect(document_, SIGNAL(contentsChanged()), SLOT(changingText()));
    QFont font;
    font.setFamily("Helvitica");
    setFont(font);
}

void AMGraphicsTextItem::setShapeIndex(int index)
{
    shapeIndex_ = index;
}

int AMGraphicsTextItem::shapeIndex()
{
    return shapeIndex_;
}

int AMGraphicsTextItem::type() const
{
    return Type;
}


void AMGraphicsTextItem::changingText()
{
    QTextCursor oldTextCursor = textCursor();
    int initialPosition = oldTextCursor.position();
    emit textChanged(shapeIndex_);
    if(!(initialPosition == 0 && textCursor().position() == 0))
    {
        setTextCursor(oldTextCursor);
    }
}

void AMGraphicsTextItem::focusInEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusInEvent(event);
    qDebug()<<"GraphicsTextItem::focusInEvent------------------------------------------------------";
    emit gotFocus(shapeIndex_);
}
