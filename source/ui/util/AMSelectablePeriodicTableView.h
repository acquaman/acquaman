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


#ifndef AMSELECTABLEPERIODICTABLEVIEW_H
#define AMSELECTABLEPERIODICTABLEVIEW_H

#include "ui/util/AMCustomizablePeriodicTableView.h"
#include "util/AMSelectablePeriodicTable.h"

/// This class extends the periodic table view to offer toggle-able buttons based on button clicks.
class AMSelectablePeriodicTableView : public AMCustomizablePeriodicTableView
{
	Q_OBJECT

public:
	/// Constructor.
	AMSelectablePeriodicTableView(AMSelectablePeriodicTable *table, QWidget *parent = 0);

	/// Returns the energy range that is acceptable for the periodic table.
	QPair<double, double> range() const { return range_; }
	/// Returns the lower bound for the acceptable range.
	double lowerBound() const { return range_.first; }
	/// Returns the upper bound for the acceptable range.
	double upperBound() const { return range_.second; }

public slots:
	/// Sets the acceptable range used for elements to be enabled.  If either low or high are negative it assumes that all elements are valid.
	void setRange(double low, double high);

protected slots:
	/// Handles passing on information to the model that the button has been clicked.
	void onElementClicked(AMElement *element);

protected:
	/// Helper method that returns whether a range is valid or not.
	bool rangeIsValid() const { return !(range_.first < 0 || range_.second < range_.first); }

	/// Pair that holds the acceptable range that should be considered.
	QPair<double, double> range_;
	/// A pointer for the selectable periodic table.  To minimize having to use qobject_cast for routine operations.
	AMSelectablePeriodicTable *selectablePeriodicTable_;
};

#endif // AMSELECTABLEPERIODICTABLEVIEW_H
