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


#include "AMWrappingLineEdit.h"

 AMWrappingLineEdit::~AMWrappingLineEdit(){}
AMWrappingLineEdit::AMWrappingLineEdit(QWidget *parent) :
	QPlainTextEdit(parent)
{
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
	focusOutReason_ = UnknownReason;
}


void AMWrappingLineEdit::setText(const QString &t) {
	blockSignals(true);

	QTextCursor c = textCursor();
	c.beginEditBlock();
	setPlainText(t);
	c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	c.endEditBlock();
	setTextCursor(c);

	blockSignals(false);
	// bazinga!
}

void AMWrappingLineEdit::onBlockCountChanged(int numParagraphs) {
	if(numParagraphs > 1) {
		QString contents(text());
		contents.remove(QRegExp("[\\n\\r]"));
		setText(contents);
		focusOutReason_ = EnterPressedReason;
		// this will trigger our editingFinished() signal in the focusOutEvent(), with the custom EnterPressedReason reason
		clearFocus();
	}
}


#include <QFocusEvent>

void AMWrappingLineEdit::focusOutEvent(QFocusEvent *e) {

	if(e->reason() == Qt::OtherFocusReason) {
		emit editingFinished(focusOutReason_);
		focusOutReason_ = UnknownReason;
	}
	else
		emit editingFinished(e->reason());

	QPlainTextEdit::focusOutEvent(e);
}

void AMWrappingLineEdit::keyPressEvent(QKeyEvent *e) {
	if(e->key() == Qt::Key_Escape) {
		focusOutReason_ = EscapePressedReason;
		clearFocus();
	}
	else
		QPlainTextEdit::keyPressEvent(e);
}
