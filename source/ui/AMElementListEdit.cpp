#include "AMElementListEdit.h"

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

void AMElementValidator::fixup(QString & text) const {
	// fixup is only used to convert full element names to symbols, IF this is turned on.
	if(!convertToSymbol_)
		return;

	QStringList split = text.split(QRegExp("\\s*,+\\s*|\\s+"), QString::SkipEmptyParts);

	for(int i=0; i<split.count(); i++) {
		// if element name matches, replace with symbol
		int ind = model_->indexOfElement(split.at(i));
		if(ind != -1)
			split[i] = model_->data(model_->index(ind,1)).toString();
	}

	text = split.join(", ");
}

QValidator::State AMElementValidator::validate(QString &text, int &pos) const {

	/// \bug temporary... implementation missing.
	return QValidator::Acceptable;

	/*

	QStringList split = text.split(QRegExp("\\s*,+\\s*|\\s+"), QString::SkipEmptyParts);

	// go through each potential "element?" name or symbol
	for(int i=0; i<split.count(); i++) {

		QString token = split.at(i);
		if(model_->)
	}

	*/
}


/* Learned from http://john.nachtimwald.com/2009/07/04/qcompleter-and-comma-separated-tags/

   Also, note that completer.setWidget(editor) was used not QLineEdit’s setCompleter() function. If setCompleter is used completion will only take place at the beginning of the QLineEdit. Meaning it will match everything before as one string and ignore the , delimiter separating the tags.

   */

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
}

#include <QDebug>
void AMElementListEdit::onTextEdited(const QString& text) {

	QStringList split = text.split(QRegExp("\\s*,+\\s*|\\s+"));
	// qDebug() << split;
	if(split.isEmpty())
		completer_->setCompletionPrefix(QString());
	else
		completer_->setCompletionPrefix(split.takeLast().trimmed());

	priorToPrefix_ = split.join(", ");

	// qDebug() << "completion prefix:" << completer_->completionPrefix();
	if(!completer_->completionPrefix().isEmpty())
		completer_->complete();
}

void AMElementListEdit::onCompleterActivated(const QString &text) {
	if(text.isEmpty())
		setText(priorToPrefix_);
	else if(priorToPrefix_.isEmpty())
		setText(text);
	else
		setText(priorToPrefix_ + ", " + text);
}
