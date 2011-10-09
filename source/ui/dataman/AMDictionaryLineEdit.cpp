#include "AMDictionaryLineEdit.h"

#include <QLabel>
#include <QKeyEvent>

#include "dataman/AMScanParametersDictionary.h"

AMDictionaryLineEdit::AMDictionaryLineEdit(AMScanParametersDictionary *dictionary, QLabel *exampleLabel, QWidget *parent) :
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
