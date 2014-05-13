/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMElementListEdit.h"

 AMElementsModel::~AMElementsModel(){}
AMElementsModel::AMElementsModel(QObject *parent)
	: QStandardItemModel(parent) {

	populate();
}

void AMElementsModel::populate() {
	// eventually, fill from database
	QStandardItem* name, *symbol, *atomicNumber;
	QList<QStandardItem*> row;
	row << name;
	row << symbol;
	row << atomicNumber;

	name = new QStandardItem("Hydrogen");
	symbol = new QStandardItem("H");
	atomicNumber = new QStandardItem("1");
	row.clear();
	row << name;
	row << symbol;
	row << atomicNumber;
	appendRow(row);

	name = new QStandardItem("Helium");
	symbol = new QStandardItem("He");
	atomicNumber = new QStandardItem("2");
	row.clear();
	row << name;
	row << symbol;
	row << atomicNumber;
	appendRow(row);

	name = new QStandardItem("Lithium");
	symbol = new QStandardItem("Li");
	atomicNumber = new QStandardItem("3");
	row.clear();
	row << name;
	row << symbol;
	row << atomicNumber;
	appendRow(row);

	name = new QStandardItem("Beryllium");
	symbol = new QStandardItem("Be");
	atomicNumber = new QStandardItem("4");
	row.clear();
	row << name;
	row << symbol;
	row << atomicNumber;
	appendRow(row);

	name = new QStandardItem("Boron");
	symbol = new QStandardItem("B");
	atomicNumber = new QStandardItem("5");
	row.clear();
	row << name;
	row << symbol;
	row << atomicNumber;
	appendRow(row);

	name = new QStandardItem("Carbon");
	symbol = new QStandardItem("C");
	atomicNumber = new QStandardItem("6");
	row.clear();
	row << name;
	row << symbol;
	row << atomicNumber;
	appendRow(row);

	updateIndexes();
}


void AMElementsModel::updateIndexes() {
	name2index_.clear();
	symbol2index_.clear();

	for(int i=0; i<rowCount(); i++) {
		name2index_.insert(data(index(i,0)).toString(), i);
		symbol2index_.insert(data(index(i,1)).toString(), i);
	}
}


#include <QRegExp>


AMElementValidator::AMElementValidator(AMElementsModel* model, QObject* parent)
	: QValidator(parent) {
	model_ = model;
	convertToSymbol_ = true;
}

AMElementValidator::~AMElementValidator(){}

void AMElementValidator::fixup(QString & text) const {

	Q_UNUSED(text)
}



QValidator::State AMElementValidator::validate(QString &text, int &pos) const {

	Q_UNUSED(pos)

	QValidator::State rv = QValidator::Acceptable;

	QStringList split = text.split(QRegExp("\\s*,+\\s*|\\s+"), QString::SkipEmptyParts);

	// go through each potential "element?" name or symbol
	for(int i=0; i<split.count(); i++) {

		QString token = split.at(i);
		// Either matching a symbol or matching an element is great... still acceptable
		if(model_->validElement(token) ) {
			if(convertToSymbol_) {
				int ind = model_->indexOfElement(model_->titleCase(split.at(i)));
				if(ind != -1)
					split[i] = model_->data(model_->index(ind,1)).toString();
			}
		}

		else if (model_->validSymbol(token)) {
			// do nothing...
		}

		else if(model_->validElementStartsWith(token) || model_->validSymbolStartsWith(token)) {
			// downgrade to "intermediate" and keep going... (this token could still become valid if they keep typing)
			rv = QValidator::Intermediate;
		}
		else {
			// just one invalid is enough. Book 'em, Deno.
			return QValidator::Invalid;
		}
	}

	// todo: figure out how to modify text (replacing names), and not mess up the cursor position
	/*

	text = split.join(", ");
	if(!split.isEmpty() && atEnd)
		text.append(",");
*/
	return rv;
}


bool AMElementsModel::validElementStartsWith(const QString &string) {

	QString s = titleCase(string);

	QMap<QString,int>::const_iterator lower = name2index_.lowerBound(s);

	if(lower != name2index_.constEnd() && lower.key().startsWith(s))
		return true;

	else
		return false;

}

bool AMElementsModel::validSymbolStartsWith(const QString &string) {
	QString s = titleCase(string);

	QMap<QString,int>::const_iterator lower = symbol2index_.lowerBound(s);

	if(lower != symbol2index_.constEnd() && lower.key().startsWith(s))
		return true;

	else
		return false;
}

/* Learned from http://john.nachtimwald.com/2009/07/04/qcompleter-and-comma-separated-tags/

   Also, note that completer.setWidget(editor) was used not QLineEdit’s setCompleter() function. If setCompleter is used completion will only take place at the beginning of the QLineEdit. Meaning it will match everything before as one string and ignore the , delimiter separating the tags.

   */

 AMElementListEdit::~AMElementListEdit(){}
AMElementListEdit::AMElementListEdit(QWidget *parent) :
		QLineEdit(parent)
{


	elementsModel_ = new AMElementsModel(this);
	elementsModel_->sort(0, Qt::AscendingOrder);

	completer_ = new QCompleter(this);
	completer_->setModel(elementsModel_);
	/// \todo: determine how to make sorting of the model case insensitive, and then completer_->setModelSorting(Qt::CaseInsensitive) for performance.
	completer_->setCaseSensitivity(Qt::CaseInsensitive);


	connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
	connect(completer_, SIGNAL(activated(QString)), this, SLOT(onCompleterActivated(QString)));
	completer_->setWidget(this);

	setValidator(new AMElementValidator(elementsModel_, this));
}

#include <QAbstractItemView>
void AMElementListEdit::onTextEdited(const QString& text) {

	QStringList split = text.split(QRegExp("\\s*,+\\s*|\\s+"));
	// qdebug() << split;
	if(split.isEmpty())
		completer_->setCompletionPrefix(QString());
	else
		completer_->setCompletionPrefix(split.takeLast().trimmed());

	priorToPrefix_ = split.join(", ");

	// qdebug() << "completion prefix:" << completer_->completionPrefix();
	if(!completer_->completionPrefix().isEmpty())
		completer_->complete();
	else {
		completer_->popup()->hide();
	}
}

void AMElementListEdit::onCompleterActivated(const QString &text) {
	QString result;

	if(text.isEmpty())
		result = priorToPrefix_;
	else if(priorToPrefix_.isEmpty())
		result = text;
	else
		result = priorToPrefix_ + ", " + text;

	setText(result);
}
