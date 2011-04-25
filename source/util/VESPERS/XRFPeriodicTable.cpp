#include "XRFPeriodicTable.h"

XRFPeriodicTable::XRFPeriodicTable(QObject *parent)
	: QObject(parent)
{
}

bool XRFPeriodicTable::addToList(AMElement *el, QPair<QString, QString> line)
{
	QPair<int, QString> temp;

	if (checkedList_.isEmpty()){

		checkedList_ << qMakePair(el->atomicNumber(), line.first);
		return true;
	}

	for (int i = 0; i < checkedList_.size(); i++){

		temp = checkedList_.at(i);

		if (temp.first != el->atomicNumber() && temp.second.compare(line.first) != 0){

			checkedList_ << qMakePair(el->atomicNumber(), line.first);
			return true;
		}
	}

	return false;
}

bool XRFPeriodicTable::removeFromList(AMElement *el, QPair<QString, QString> line)
{
	QPair<int, QString> temp;

	for (int i = 0; i < checkedList_.size(); i++){

		temp = checkedList_.at(i);

		if (temp.first == el->atomicNumber() && temp.second.compare(line.first) == 0){

			checkedList_.removeAt(i);
			return true;
		}
	}

	return false;
}
