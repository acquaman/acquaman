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


#include "AMDictionaryLineEdit.h"

#include <QLabel>

#pragma clang diagnostic ignored "-Wunused-private-field"
#include <QKeyEvent>
#pragma clang diagnostic warning "-Wunused-private-field"

#include "dataman/AMScanParametersDictionary.h"

 AMDictionaryLineEdit::~AMDictionaryLineEdit(){}
AMDictionaryLineEdit::AMDictionaryLineEdit(AMOldScanParametersDictionary *dictionary, QLabel *exampleLabel, QWidget *parent) :
	QLineEdit(parent)
{
	dictionary_ = dictionary;
	exampleLabel_ = exampleLabel;
	connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
	if(exampleLabel_)
		connect(dictionary_, SIGNAL(parsed(QString)), exampleLabel_, SLOT(setText(QString)));
}

void AMDictionaryLineEdit::setTextAndOperate(const QString &text){
	setText(text);
	onTextEdited(text);
}

void AMDictionaryLineEdit::onTextEdited(const QString &text){
	dictionary_->parseKeywordStringAndOperate(text);
}

void AMDictionaryLineEdit::keyPressEvent(QKeyEvent *e){
	if(e->key() == Qt::Key_BracketLeft){
		int lastCursorPosition = cursorPosition();
		setText(text().insert(lastCursorPosition, "[]"));
		setCursorPosition(lastCursorPosition+1);
	}
	else if(e->key() == Qt::Key_BracketRight && cursorPosition() != text().length() && text().at(cursorPosition()) == ']')
		setCursorPosition(cursorPosition()+1);
	else if(e->key() == Qt::Key_Backspace){
		if(hasSelectedText())
			QLineEdit::keyPressEvent(e);
		else if(text().length() >= 2 && cursorPosition() != 0 && text().at(cursorPosition()-1) == ']' && text().at(cursorPosition()-2) == '[')
			setText(text().remove(cursorPosition()-2,2));
		else if(text().length() >= 2 && cursorPosition() != 0 && text().at(cursorPosition()-1) == '[' && cursorPosition() != text().length() && text().at(cursorPosition()) == ']')
			setText(text().remove(cursorPosition()-1,2));
		else if(text().length() >= 3 && cursorPosition() != 0 && text().at(cursorPosition()-1) == ']'){
			int otherBracket = text().lastIndexOf('[', cursorPosition()-1);
			int difference = cursorPosition()-otherBracket;
			if(otherBracket != -1)
				setSelection(otherBracket, difference);
		}
		else if(text().length() >= 3 && cursorPosition() != 0 && text().at(cursorPosition()-1) == '['){
			int otherBracket = text().indexOf(']', cursorPosition()-1);
			int difference = otherBracket-(cursorPosition()-1)+1;
			if(otherBracket != -1)
				setSelection(cursorPosition()-1, difference);
		}
		else
			QLineEdit::keyPressEvent(e);
	}
	else
		QLineEdit::keyPressEvent(e);
}
