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


#ifndef AMELEMENTLISTEDIT_H
#define AMELEMENTLISTEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QValidator>

#include <QStandardItemModel>
#include <QMap>

class AMElementsModel: public QStandardItemModel {
	Q_OBJECT
public:
 	virtual ~AMElementsModel();
	explicit AMElementsModel(QObject* parent = 0);

	/// Re-implemented from QStandardItemModel to update the "indexing" on the columns
	void sort(int column, Qt::SortOrder order) {
		QStandardItemModel::sort(column, order);
		updateIndexes();
	}

	int indexOfElement(const QString& e) const {
		if(name2index_.contains(e)) return name2index_.value(e);
		else return -1;
	}

	int indexOfSymbol(const QString& e) const {
		if(symbol2index_.contains(e)) return symbol2index_.value(e);
		else return -1;
	}

	/// returns true if this string is an exact element name (non case-sensitive)
	bool validElement(const QString& element) {
		return indexOfElement(titleCase(element)) != -1;
	}

	/// returns true if this string is an exact symbol (non case-sensitive)
	bool validSymbol(const QString& symbol) {
		return indexOfSymbol(titleCase(symbol)) != -1;
	}

	/// returns true if this string is the beginning of an element name
	bool validElementStartsWith(const QString& string);
	/// returns true if this string is the beginning of an element symbol
	bool validSymbolStartsWith(const QString& string);

	static QString titleCase(const QString& name) {
		QString rv = name.toLower();
		if(!rv.isEmpty())
			rv[0] = rv.at(0).toUpper();
		return rv;
	}

protected:
	/// Fill the model
	void populate();
	/// Fill the name2index_ and symbol2index_ maps for fastest searching. Called automatically by populate() and sort().
	void updateIndexes();

	QMap<QString, int> name2index_;
	QMap<QString, int> symbol2index_;

};

/// This validator enforces entry on a text input that accepts element names and symbols.  Successive elements can be separated by a combination of commas and whitespaces.  Full element names are accepted, as well as symbols. If convertToSymbol() is turned on, complete element names are converted inside fixup() into their symbols.
class AMElementValidator : public QValidator {
	Q_OBJECT
public:
 	virtual ~AMElementValidator();
	explicit AMElementValidator(AMElementsModel* model, QObject* parent = 0);
	virtual void fixup(QString &) const;
	virtual QValidator::State validate(QString &, int &) const;

	/// Indicates whether automatic conversion from elements to symbols is turned on
	bool convertsToSymbol() const {
		return convertToSymbol_;
	}

	/// Set whether automatic conversion from element names to symbols is turned on
	void setConvertsToSymbol(bool convertToSymbol = true) {
		convertToSymbol_ = convertToSymbol;
	}


protected:
	bool convertToSymbol_;

	AMElementsModel* model_;
};

class AMElementListEdit : public QLineEdit
{
Q_OBJECT
public:
 	virtual ~AMElementListEdit();
	explicit AMElementListEdit(QWidget *parent = 0);

signals:

public slots:



protected slots:
	/// Called as the user edits text, this allows us to set the completion prefix on the completer
	void onTextEdited(const QString& text);
	/// Called when a result is chosen from the completer:
	void onCompleterActivated(const QString& text);

protected:
	QCompleter* completer_;
	AMElementsModel* elementsModel_;
	QString priorToPrefix_;
};

#endif // AMELEMENTLISTEDIT_H
