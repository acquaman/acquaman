#include "AMGraphicsTextItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QTextDocumentFragment>
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
	dontChangeSelection_ = false;
	selectAll_ = false;
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
	QTextCursor oldCursor = textCursor();
	QString textToSet = text;
	if(textToSet != toPlainText())
		document()->setPlainText(textToSet);
	if(selectAll())
	{
		selectAllText();
	}
	else
	{
		setTextCursor(oldCursor);
	}
}




bool AMGraphicsTextItem::selectAll()
{
	return selectAll_;
}

void AMGraphicsTextItem::selectAllText()
{
	QTextCursor newCursor = textCursor();
	newCursor.select(QTextCursor::Document);
	setTextCursor(newCursor);
}

void AMGraphicsTextItem::setSelectAll(const bool &selectAll)
{
	selectAll_ = selectAll;
}

void AMGraphicsTextItem::clearSelection()
{
	QTextCursor newCursor = textCursor();
	newCursor.setPosition(0);
	setTextCursor(newCursor);
}


void AMGraphicsTextItem::changingText()
{
	int initialPosition = textCursor().position();

	QTextCursor newPosition = textCursor();
	int anchorPositon = newPosition.anchor();

	if(textCursor().atStart())
		initialPosition = -2;
	emit textChanged(shapeIndex_);
	if(initialPosition == -1)
	{
		initialPosition++;
	}
	else if (initialPosition == -2)
	{
		newPosition.setPosition(0);
	}
	else
		newPosition.setPosition(initialPosition);
	newPosition.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,newPosition.position() - anchorPositon);
	setTextCursor(newPosition);
	setSelectAll(false);
}

void AMGraphicsTextItem::focusInEvent(QFocusEvent *event)
{
	QGraphicsTextItem::focusInEvent(event);
	dontChangeSelection_ = true;
	emit gotFocus(shapeIndex_);
	bool oldValue = blockSignals(true);
	selectAllText();
	blockSignals(oldValue);

}

void AMGraphicsTextItem::focusOutEvent(QFocusEvent *event)
{
	QGraphicsTextItem::focusOutEvent(event);
	setSelectAll(false);
	clearSelection();
}

#include <QKeyEvent>
void AMGraphicsTextItem::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
		emit returnPressed(shapeIndex_);
		clearFocus();
		event->accept();
	}
	else if(event->key() == Qt::Key_Delete)
	{
		QString docString = toPlainText();
		if(docString.count() > 0)
		{
			int cursorPosition = textCursor().position();
			if(cursorPosition >= 0)
			{
				docString.remove((cursorPosition), 1);
				setPlainText(docString);
				QTextCursor newCursor = textCursor();
				newCursor.setPosition(cursorPosition);
				setTextCursor(newCursor);
			}
		}
	}
	else
		QGraphicsTextItem::keyPressEvent(event);
}

void AMGraphicsTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QTextCursor oldCursor = textCursor();
	QGraphicsTextItem::mouseReleaseEvent(event);
	if(dontChangeSelection_)
	{
		setTextCursor(oldCursor);
		dontChangeSelection_ = false;
	}
}

