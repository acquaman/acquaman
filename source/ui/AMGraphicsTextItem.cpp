/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMGraphicsTextItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QCursor>
#include <QTextCursor>
#include <QDebug>

#include "AMQEvents.h"

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

void AMGraphicsTextItem::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
		emit returnPressed(shapeIndex_);
		clearFocus();
		event->accept();
	}// for no particular reason delete doesn't work, so reimplemented here
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

