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
