#include "AMSelectablePeriodicTable.h"

AMSelectablePeriodicTable::AMSelectablePeriodicTable(QObject *parent) :
	QObject(parent)
{
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
