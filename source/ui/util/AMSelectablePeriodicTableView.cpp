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

 AMSelectablePeriodicTableView::~AMSelectablePeriodicTableView(){}
AMSelectablePeriodicTableView::AMSelectablePeriodicTableView(AMSelectablePeriodicTable *table, QWidget *parent)
	: AMCustomizablePeriodicTableView(table, parent)
{
	selectablePeriodicTable_ = table;
	connect(this, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));
	connect(selectablePeriodicTable_, SIGNAL(allElementsDeselected()), this, SLOT(onAllElementsDeselected()));

	energyRange_ = AMRange();
}

void AMSelectablePeriodicTableView::buildPeriodicTableView()
{
	AMCustomizablePeriodicTableView::buildPeriodicTableView();

	// Go through every button and make it checkable.
	for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++)
		button(i)->setCheckable(true);
}

void AMSelectablePeriodicTableView::onElementClicked(AMElement *element)
{
	QAbstractButton *clicked = button(element->atomicNumber());

	if (clicked->isChecked())
		selectablePeriodicTable_->selectElement(element);
	else
		selectablePeriodicTable_->deselectElement(element);
}

void AMSelectablePeriodicTableView::setEnergyRange(double low, double high)
{
	setEnergyRange(AMRange(low, high));
}

void AMSelectablePeriodicTableView::setEnergyRange(const AMRange &range)
{
	if (energyRange_ != range){

		energyRange_ = range;
		updateTableView();
	}
}

void AMSelectablePeriodicTableView::setMinimumEnergy(double newMinimum)
{
	if (energyRange_.minimum() != newMinimum){

		energyRange_.setMinimum(newMinimum);
		updateTableView();
	}
}

void AMSelectablePeriodicTableView::setMaximumEnergy(double newMaximum)
{
	if (energyRange_.maximum() != newMaximum){

		energyRange_.setMaximum(newMaximum);
		updateTableView();
	}
}

void AMSelectablePeriodicTableView::updateTableView()
{
	if (rangeIsValid()){

		for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++){

			AMElement *temp = periodicTable_->elementByAtomicNumber(i);
			button(i)->setDisabled((temp->Kalpha().energy() < energyRange_.minimum())
								   || ((temp->Kalpha().energy() > energyRange_.maximum() && temp->Lalpha().energy() < energyRange_.minimum()))
								   || (temp->emissionLines().isEmpty()));
		}
	}

	else {

		for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++)
			button(i)->setEnabled(true);
	}
}

void AMSelectablePeriodicTableView::onAllElementsDeselected()
{
	for (int i = 1, size = periodicTable_->numberOfElements(); i <= size; i++)
		button(i)->setChecked(false);
}
