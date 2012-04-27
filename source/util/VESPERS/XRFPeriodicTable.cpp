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


#include "XRFPeriodicTable.h"

#include "util/AMPeriodicTable.h"

XRFPeriodicTable::XRFPeriodicTable(double minEnergy, double maxEnergy, QObject *parent)
	: QObject(parent)
{
	minimumEnergy_ = minEnergy;
	maximumEnergy_ = maxEnergy;
	current_ = 0;

	for (int i = 0; i < AMPeriodicTable::table()->elements().size(); i++)
		xrfTable_ << new XRFElement(AMPeriodicTable::table()->elementByAtomicNumber(i+1), this);
}

XRFPeriodicTable::~XRFPeriodicTable()
{
	for (int i = 0; i < xrfTable_.size(); i++)
		delete xrfTable_.at(i);
}

void XRFPeriodicTable::addLineToList(QString line)
{
	// Make sure that current is valid first.  Then, if the line was successfully added, emit the proper signal.  If the current element is not already in the selectedElements_ list, then add it.
	if (current_){

		bool alreadySelected = current_->hasLinesSelected();

		if (current_->addLine(line)){

			if (!alreadySelected)
				selectedElements_ << current_;

			emit addedRegionOfInterest(current_, line);
		}
	}
}

void XRFPeriodicTable::removeLineFromList(QString line)
{
	// Make sure the current pointer is valid.  Then, if the lien was successfully removed, emit the proper signal.  If the element doesn't have any more selected lines then it is removed from the list.
	if (current_){

		if (current_->removeLine(line)){

			if (!current_->hasLinesSelected())
				selectedElements_.removeOne(current_);

			emit removedRegionOfInterest(current_, line);
		}
	}
}

void XRFPeriodicTable::removeAll()
{
	// Don't do anything if current isn't valid.
	if (current_ == 0)
		return;

	// Save current element.
	XRFElement *current = current_;

	while (!selectedElements_.isEmpty()){

		current_ = selectedElements_.first();

		while (current_->hasLinesSelected())
			current_->removeLine(current_->linesSelected().first());

		selectedElements_.removeFirst();
	}

	// Used to notify that the old current element information may have changed.
	emit currentElementChanged(current);
	emit removedAllRegionsOfInterest();
}
