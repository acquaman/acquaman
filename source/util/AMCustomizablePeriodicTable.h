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


#ifndef AMCUSTOMIZABLEPERIODICTABLE_H
#define AMCUSTOMIZABLEPERIODICTABLE_H

#include <QObject>
#include <QList>

#include "AMElement.h"

#define AMCUSTOMIZABLEPERIODICTABLE_COULD_NOT_LOAD_ELEMENTAL_INFORMATION 323000

/// This class is a container for AMElements in a similar manner as AMPeriodicTable.  The primary difference being that this periodic table is setup to enable subclassing when such behaviour is desirable.
class AMCustomizablePeriodicTable : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds the periodic table from a file and creates a list of elements.
 	virtual ~AMCustomizablePeriodicTable();
	explicit AMCustomizablePeriodicTable(QObject *parent = 0);

	/// Builds the periodic table model.  Must be called after the table has been created.  Subclasses should reimplement if more sophisticated subclasses of AMElement are required.
	virtual void buildPeriodicTable();

	/// Returns the list of elements.
	QList<AMElement *> elements() const { return periodicTable_; }
	/// Returns the number of elements in the periodic table.
	int numberOfElements() const { return periodicTable_.size(); }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	AMElement *elementByName(QString name) const;
	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	AMElement *elementBySymbol(QString symbol) const;
	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	AMElement *elementByAtomicNumber(int number) const { return periodicTable_.at(number-1); }

signals:

public slots:

protected:
	/// The periodic table.  In all its glory.  Stores all AMElement *'s in a list.
	QList<AMElement *> periodicTable_;
};

#endif // AMCUSTOMIZABLEPERIODICTABLE_H
