#ifndef XRFSELECTIONVIEW_H
#define XRFSELECTIONVIEW_H

#include <QWidget>

#include "util/AMElement.h"
#include "ui/VESPERS/XRFPeriodicTableView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSXRFElementView.h"

class XRFSelectionView : public QWidget
{
	Q_OBJECT
public:
	/// The constructor.  It takes in values for a maximum and minimum energy that should be enabled, if desired.
	explicit XRFSelectionView(double minimumEnergy = 0, double maximumEnergy = 1e6, QWidget *parent = 0);

	/// Returns the minimum energy.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy.
	double maximumEnergy() const { return maximumEnergy_; }

signals:
	/// Passes on the signal that an element was selected.  Contains the element.
	void elementSelected(AMElement *);
	/// Passes along the signal that an ROI should be added.
	void addRegionOfInterest(AMElement *, QPair<QString, QString>);
	/// Passes along the signal that an ROI should be removed.
	void removeRegionOfInterest(AMElement *, QPair<QString, QString>);

public slots:
	/// Sets the minimum energy.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; tableView_->setMinimumEnergy(energy); elView_->setMinimumEnergy(energy); }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; tableView_->setMaximumEnergy(energy); elView_->setMaximumEnergy(energy); }

protected slots:

protected:

	/// A pointer to the xrf periodic table view.
	XRFPeriodicTableView *tableView_;
	/// A pointer to the element view.
	VESPERSXRFElementView *elView_;

	/// Holds the minimum energy.  This is the lower limit and elements that don't have emission lines with energies higher then this are disabled.
	double minimumEnergy_;
	/// Holds the maximum energy.  This is the upper limit and elements that don't have emission lines with energies lower then this are disabled.
	double maximumEnergy_;
};

#endif // XRFSELECTIONVIEW_H
