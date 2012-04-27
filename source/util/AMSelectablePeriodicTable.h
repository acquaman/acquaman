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


#ifndef AMSELECTABLEPERIODICTABLE_H
#define AMSELECTABLEPERIODICTABLE_H

#include <QObject>
#include "util/AMPeriodicTable.h"

///	This class encapsulates the periodic table and extends it's functionality slightly by offering a "memory" of selected elements.
class AMSelectablePeriodicTable : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit AMSelectablePeriodicTable(QObject *parent = 0);

	// Periodic table getters.
	/////////////////////////////////////////

	/// Returns the list of elements.
	QList<const AMElement *> elements() const { return AMPeriodicTable::table()->elements(); }
	/// Returns the number of elements in the periodic table.
	int numberOfElements() const { return AMPeriodicTable::table()->numberOfElements(); }
	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	const AMElement *elementByName(QString name) const { return AMPeriodicTable::table()->elementByName(name); }
	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	const AMElement *elementBySymbol(QString symbol) const { return AMPeriodicTable::table()->elementBySymbol(symbol); }
	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	const AMElement *elementByAtomicNumber(int number) const { return AMPeriodicTable::table()->elementByAtomicNumber(number); }

	// Added features.
	///////////////////////////////////////

	/// Adds a selected element to the end of the list.
	void selectElement(int atomicNumber);
	/// Removes a selected element from the list.
	void deselectElement(int atomicNumber);
	/// Returns the list of selected elements.
	QList<int> selectedElements() const { return savedElements_; }

	/// Clear the list.
	void clearList();

signals:
	/// Notifier that an element has been selected.  Passes the atomic number.
	void elementSelected(int);
	/// Notifier that an element has been deselected.  Passes the atomic number.
	void elementDeselected(int);
	/// Notifier that all of the elments have been removed.
	void listCleared();

public slots:

protected:
	/// List of atomic numbers that have been saved.
	QList<int> savedElements_;
};

#endif // AMSELECTABLEPERIODICTABLE_H
