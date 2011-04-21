#ifndef XRFPERIODICTABLEVIEW_H
#define XRFPERIODICTABLEVIEW_H

#include <QWidget>

#include "ui/AMPeriodicTableView.h"

class XRFPeriodicTableView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Does not need any information.  Can optionally provide upper and lower limits.  These will disable elements based on those numbers.
	explicit XRFPeriodicTableView(double minEnergy = 0, double maxEnergy = 1e6, QWidget *parent = 0);

	/// Returns the minimum energy.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy.
	double maximumEnergy() const { return maximumEnergy_; }

signals:
	/// Passes on the signal that an element was selected.  Contains the element.
	void elementSelected(AMElement *);

public slots:
	/// Sets the minimum energy.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; disableElements(); }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; disableElements(); }

	/// Changes the color of the element button due to an ROI being added.
	void regionOfInterestAdded(AMElement *el, QPair<QString, QString> line);
	/// Reverts the color of the button back to its original color scheme due to an ROI being deleted.
	void regionOfInterestRemoved(AMElement *el, QPair<QString, QString> line);

protected:
	/// Helper function that disables the buttons based on the current values of maximumEnergy_ and minimumEnergy_.
	void disableElements();

	/// A generic periodic table view.
	AMPeriodicTableView *tableView_;

	/// Holds the minimum energy.  This is the lower limit and elements that don't have emission lines with energies higher then this are disabled.
	double minimumEnergy_;
	/// Holds the maximum energy.  This is the upper limit and elements that don't have emission lines with energies lower then this are disabled.
	double maximumEnergy_;

};

#endif // XRFPERIODICTABLEVIEW_H
