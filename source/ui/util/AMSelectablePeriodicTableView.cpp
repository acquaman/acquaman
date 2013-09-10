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
	: AMPeriodicTableView(parent)
{
	table_ = table;
	connect(this, SIGNAL(clicked(int)), this, SLOT(onClicked(int)));

	// Go through every button and make it checkable.
	for (int i = 1; i <= table_->numberOfElements(); i++)
		button(i)->setCheckable(true);

	range_ = qMakePair(-1.0, -1.0);
}

void AMSelectablePeriodicTableView::onClicked(int id)
{
	QToolButton *clicked = button(id);

	if (clicked->isChecked())
		table_->selectElement(id);
	else
		table_->deselectElement(id);
}

void AMSelectablePeriodicTableView::setRange(double low, double high)
{
	range_ = qMakePair(low, high);
	const AMElement *temp;

	if (rangeIsValid()){

		for (int i = 1; i <= table_->numberOfElements(); i++){

			temp = table_->elementByAtomicNumber(i);
			button(i)->setDisabled((temp->Kalpha().energy() < low)
								   || ((temp->Kalpha().energy() > high && temp->Lalpha().energy() < low))
								   || (temp->emissionLines().isEmpty()));
		}
	}

	else {

		for (int i = 1; i <= table_->numberOfElements(); i++)
			button(i)->setEnabled(true);
	}
}
