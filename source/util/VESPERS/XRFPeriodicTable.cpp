#include "XRFPeriodicTable.h"

XRFPeriodicTable::XRFPeriodicTable(QObject *parent)
	: QObject(parent)
{
	minimumEnergy_ = minEnergy;
	maximumEnergy_ = maxEnergy;
}

void XRFPeriodicTable::addToList(AMElement *el, QString line)
{
	QPair<int, QString> temp;

	if (checkedList_.isEmpty()){

		checkedList_ << qMakePair(el->atomicNumber(), line.first);
		emit roiAdded(el, line);
	}

	for (int i = 0; i < checkedList_.size(); i++){

		temp = checkedList_.at(i);

		// If the region exists already in the list, then don't add it again.
		if (temp.first == el->atomicNumber() && temp.second.compare(line) == 0)
			return;

		// If the region doesn't exist yet, then add it to the list.
		if (temp.first != el->atomicNumber() || temp.second.compare(line) != 0){

			checkedList_ << qMakePair(el->atomicNumber(), line);
			emit roiAdded(el, line);
		}
	}
}

void XRFPeriodicTable::removeFromList(AMElement *el, QString line)
{
	QPair<int, QString> temp;

	for (int i = 0; i < checkedList_.size(); i++){

		temp = checkedList_.at(i);

		if (temp.first == el->atomicNumber() && temp.second.compare(line) == 0){

			checkedList_.removeAt(i);
			emit roiRemoved(el, line);
		}
	}
}

void XRFPeriodicTable::clearAll()
{
	while (!checkedList_.isEmpty())
		removeFromList(elementByAtomicNumber(checkedList_.last().first), line);
}
