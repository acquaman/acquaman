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


#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QObject>
#include <QList>

#include "AMElement.h"

/*! This class is a container for AMElements that provides the convenience of retrieving an element in an intuitive way.  It also provides singleton-style access to an application-wide periodic table, via AMPeriodicTable::table().
  */
class AMPeriodicTable : public QObject
{
	Q_OBJECT

public:
	/// Access the application-wide periodic table object. It will be loaded for you if it hasn't been loaded yet.
	static AMPeriodicTable* table() {
		if(!instance_)
			instance_ = new AMPeriodicTable();
		return instance_;
	}

	/// Returns the list of elements.
	QList<const AMElement *> elements() const { return periodicTable_; }
	/// Returns the number of elements in the periodic table.
	int numberOfElements() const { return periodicTable_.size(); }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	const AMElement *elementByName(QString name) const
	{
		for (int i = 0; i < periodicTable_.size(); i++)
			if (QString::compare(periodicTable_.at(i)->name(), name) == 0)
				return periodicTable_.at(i);

		return 0;
	}

	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	const AMElement *elementBySymbol(QString symbol) const
	{
		for (int i = 0; i < periodicTable_.size(); i++)
			if (QString::compare(periodicTable_.at(i)->symbol(), symbol) == 0)
				return periodicTable_.at(i);

		return 0;
	}

	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	const AMElement *elementByAtomicNumber(int number) const { return periodicTable_.at(number-1); }

signals:
	/// Signal emitted if the the periodic table fails to load.  The parameters passed are the file name and error message.
	void unableToLoad(QString, QString);

public slots:

protected:
	// Member variables.
	// The periodic table.  In all its glory.
	QList<const AMElement *> periodicTable_;

	/// Singleton instance variable
	static AMPeriodicTable* instance_;

	/// Constructor: Builds the periodic table from a file and creates a list of elements. (Protected: access via table())
	explicit AMPeriodicTable(QObject *parent = 0);
};

#endif // PERIODICTABLE_H
