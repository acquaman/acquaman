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


#ifndef AMSELECTABLEPERIODICTABLEVIEW_H
#define AMSELECTABLEPERIODICTABLEVIEW_H

#include "ui/util/AMCustomizablePeriodicTableView.h"
#include "util/AMSelectablePeriodicTable.h"
#include "util/AMRange.h"

/// This class extends the periodic table view to offer toggle-able buttons based on button clicks.
class AMSelectablePeriodicTableView : public AMCustomizablePeriodicTableView
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMSelectablePeriodicTableView();
	AMSelectablePeriodicTableView(AMSelectablePeriodicTable *table, QWidget *parent = 0);

	/// Re-implemented.  Uses AMCustomizeablePeriodicTableView, but then makes every button checkable.
	virtual void buildPeriodicTableView();

	/// Returns the energy range that is acceptable for the periodic table.
	const AMRange &range() const { return energyRange_; }
	/// Returns the lower bound for the acceptable range.
	double lowerBound() const { return energyRange_.minimum(); }
	/// Returns the upper bound for the acceptable range.
	double upperBound() const { return energyRange_.maximum(); }

public slots:
	/// Sets the acceptable range used for elements to be enabled.
	void setEnergyRange(double low, double high);
	/// Sets the acceptable range used for elements to be enabled.
	void setEnergyRange(const AMRange &range);
	/// Sets the minimum of the energy range.
	void setMinimumEnergy(double newMinimum);
	/// Sets the maximum of the energy range.
	void setMaximumEnergy(double newMaximum);

protected slots:
	/// Handles passing on information to the model that the button has been clicked.
	void onElementClicked(AMElement *element);
	/// Handles reseting the look of the table when all the elements are removed at once.
	void onAllElementsDeselected();

protected:
	/// Method that updates the disabled look of the buttons based on the energy range.
	void updateTableView();
	/// Helper method that returns whether a range is valid or not.  This is more strict than AMRange::isValid() because energies cannot be less than 0.
	bool rangeIsValid() const { return energyRange_.minimum() >= 0 || energyRange_.isValid(); }

	/// Pair that holds the acceptable range that should be considered.
	AMRange energyRange_;
	/// A pointer for the selectable periodic table.  To minimize having to use qobject_cast for routine operations.
	AMSelectablePeriodicTable *selectablePeriodicTable_;
};

#endif // AMSELECTABLEPERIODICTABLEVIEW_H
