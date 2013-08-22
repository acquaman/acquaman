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

void AMGraphicsTextItem::setPlainText(const QString &text)
{
    // Setting the text to an empty string may cause a crash
    // so, set it to a space instead
    if(text == "")
    {
        QGraphicsTextItem::setPlainText(" ");
    }
    else
        QGraphicsTextItem::setPlainText(text);
}


void AMGraphicsTextItem::changingText()
{
    int initialPosition = textCursor().position();
    emit textChanged(shapeIndex_);
    QTextCursor newPosition = textCursor();
    newPosition.setPosition(initialPosition);
    setTextCursor(newPosition);
}

void AMGraphicsTextItem::focusInEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusInEvent(event);
    qDebug()<<"GraphicsTextItem::focusInEvent------------------------------------------------------";
    emit gotFocus(shapeIndex_);
}
