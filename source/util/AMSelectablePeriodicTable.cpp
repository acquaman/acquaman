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


#include "AMSelectablePeriodicTable.h"

#include "util/AMPeriodicTable.h"

AMSelectablePeriodicTable::AMSelectablePeriodicTable(QObject *parent) :
	QObject(parent)
{
	for (int i = 0, size = AMPeriodicTable::table()->numberOfElements(); i < size; i++)
		periodicTable_ << new AMSelectableElement(AMPeriodicTable::table()->elementByAtomicNumber(i+1)->name());
}

void AMSelectablePeriodicTable::selectElement(int atomicNumber)
{
	savedElements_.append(atomicNumber);
	emit elementSelected(atomicNumber);
}

void AMSelectablePeriodicTable::deselectElement(int atomicNumber)
{
	savedElements_.removeOne(atomicNumber);
	emit elementDeselected(atomicNumber);
}

void AMSelectablePeriodicTable::clearList()
{
	savedElements_.clear();
	emit listCleared();
}

AMSelectableElement *AMSelectablePeriodicTable::elementByName(QString name) const
{
	for (int i = 0, size = periodicTable_.size(); i < size; i++)
		if (periodicTable_.at(i)->name() == name)
			return periodicTable_.at(i);

	return 0;
}

AMSelectableElement *AMSelectablePeriodicTable::elementBySymbol(QString symbol) const
{
	for (int i = 0, size = periodicTable_.size(); i < size; i++)
		if (periodicTable_.at(i)->symbol() == symbol)
			return periodicTable_.at(i);

	return 0;
}

AMSelectableElement *AMSelectablePeriodicTable::elementByAtomicNumber(int number) const
{
	return periodicTable_.at(number-1);
}
