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
    // Pad text with a space at the end, so
    // that when it is empty there is no crash
    QString textToSet = text;
    if(textToSet.count() > 0 && !textToSet.endsWith(" "))
    {
        textToSet.append(" ");
    }
    else if(textToSet.count() == 0)
    {
        textToSet = " ";
    }
    QGraphicsTextItem::setPlainText(textToSet);
}


QString AMGraphicsTextItem::toPlainText() const
{
    // remove the space at the end for elsewhere
    QString result = QGraphicsTextItem::toPlainText();
    if(result.endsWith(" "))
    {
        result = result.remove(result.count()-1,1);
    }
    return result;
}


void AMGraphicsTextItem::changingText()
{
	int initialPosition = textCursor().position();
	emit textChanged(shapeIndex_);
	QTextCursor newPosition = textCursor();
    if(initialPosition == -1)
    {
        initialPosition++;
    }
	newPosition.setPosition(initialPosition);
	setTextCursor(newPosition);
}

void AMGraphicsTextItem::focusInEvent(QFocusEvent *event)
{
	QGraphicsTextItem::focusInEvent(event);
	qDebug()<<"GraphicsTextItem::focusInEvent------------------------------------------------------";
	emit gotFocus(shapeIndex_);
}

#include <QKeyEvent>
void AMGraphicsTextItem::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
		qDebug() << "Heard return pressed, so emit signal";
		emit returnPressed(shapeIndex_);
		clearFocus();
		event->accept();
	}
	else
		QGraphicsTextItem::keyPressEvent(event);
}
