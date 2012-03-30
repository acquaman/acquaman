#ifndef AMSELECTABLEPERIODICTABLEVIEW_H
#define AMSELECTABLEPERIODICTABLEVIEW_H

#include "ui/util/AMPeriodicTableView.h"
#include "util/AMSelectablePeriodicTable.h"

/// This class extends the periodic table view to offer toggle-able buttons based on button clicks.
class AMSelectablePeriodicTableView : public AMPeriodicTableView
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
	void onClicked(int id);

protected:
	/// Helper method that returns whether a range is valid or not.
	bool rangeIsValid() const { return range_.first < 0 || range_.second < 0 ? false : true; }
	/// Holds a pointer to the selectable table model for this view.
	AMSelectablePeriodicTable *table_;

	/// Pair that holds the acceptable range that should be considered.
	QPair<double, double> range_;
};

#endif // AMSELECTABLEPERIODICTABLEVIEW_H
