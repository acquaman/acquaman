#ifndef AMXRFDETAILEDDETECTORVIEW_H
#define AMXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFBaseDetectorView.h"

#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include "util/AMSelectablePeriodicTable.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "ui/util/AMSelectableElementView.h"
#include "util/AMNameAndRangeValidator.h"

class AMXRFDetailedDetectorView : public AMXRFBaseDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a more detailed view for AMXRFDetectors.
	AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

	/// Returns the energy range used for filtering.  If no range has been provided the range is null.
	const AMRange &energyRange() const { return emissionLineValidator_->range(); }
	/// Returns the minimum energy in the energy range filter.
	double minimumEnergy() const { return emissionLineValidator_->minimum(); }
	/// Returns the maximum energy in the energy range filter.
	double maximumEnergy() const { return emissionLineValidator_->maximum(); }

	/// Returns the list of emission line name filters that have provided to the element view.
	QStringList emissionLinenameFilters() const { return emissionLineValidator_->nameFilters(); }
	/// Returns the emission line name filter at a given index.
	const QString &emissionLineNameFilterAt(int index) const { return emissionLineValidator_->nameFilterAt(index); }
	/// Removes the emission line name filter at the given index.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(int index);
	/// Removes the emission line name filter with the given name fitler.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(const QString &filter);
	/// Adds a new emission line name filter to the list of name filters.
	void addEmissionLineNameFilter(const QString &newNameFilter);

public slots:
	/// Sets the energy range filter for this view.
	void setEnergyRange(const AMRange &newRange);
	/// Sets the energy range filter for this view.
	void setEnergyRange(double minimum, double maximum);
	/// Sets a new minimum value for the energy range.
	void setMinimumEnergy(double newMinimum);
	/// Sets a new maximum value for the energy range.
	void setMaximumEnergy(double newMaximum);

protected slots:
	/// Handles setting a new element to the AMSelectableElementView.
	void onElementClicked(AMElement *element);
	/// Handles changing the look of the periodic table view if an element is selected.
	void onElementSelected(AMElement *element);
	/// Handles changing the look of the periodic table view if an element is deselected.
	void onElementDeselected(AMElement *element);
	/// Updates the emission line list after the energy range or name filter has been changed.
	void updateEmissionLineMarkers();

protected:
	/// Helper method that returns whether the name of an emission line is valid or not based on the current filters.
	bool isEmissionLineValid(const QString &name) const;

	/// The selectable periodic table model.
	AMSelectablePeriodicTable *periodicTable_;
	/// The selectable periodic table view.
	AMSelectablePeriodicTableView *periodicTableView_;
	/// The selectable element view.
	AMSelectableElementView *elementView_;

	/// The validator for the range and emission line names.
	AMNameAndRangeValidator *emissionLineValidator_;
};

#endif // AMXRFDETAILEDDETECTORVIEW_H
