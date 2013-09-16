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


#include "AMSelectablePeriodicTableView.h"

AMSelectablePeriodicTableView::AMSelectablePeriodicTableView(AMSelectablePeriodicTable *table, QWidget *parent)
	: AMCustomizablePeriodicTableView(table, parent)
{
	selectablePeriodicTable_ = table;
	connect(this, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));

	// Go through every button and make it checkable.
	for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++)
		button(i)->setCheckable(true);

	range_ = AMRange(-1.0, -1.0);
}

void AMSelectablePeriodicTableView::onElementClicked(AMElement *element)
{
	QAbstractButton *clicked = button(element->atomicNumber());

	if (clicked->isChecked())
		selectablePeriodicTable_->selectElement(element->atomicNumber());
	else
		selectablePeriodicTable_->deselectElement(element->atomicNumber());
}

void AMSelectablePeriodicTableView::setRange(double low, double high)
{
	range_ = AMRange(low, high);
	AMElement *temp;

	if (rangeIsValid()){

		for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++){

			temp = periodicTable_->elementByAtomicNumber(i);
			button(i)->setDisabled((temp->Kalpha().energy() < range_.minimum())
								   || ((temp->Kalpha().energy() > range_.maximum() && temp->Lalpha().energy() < range_.minimum()))
								   || (temp->emissionLines().isEmpty()));
		}
	}

	else {

		for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++)
			button(i)->setEnabled(true);
	}
}
