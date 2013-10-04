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

	/// Sets the colors for each of the emission lines.
	void setLineColors(const QColor &kColor, const QColor &lColor, const QColor &mColor, const QColor &defaultColor);
	/// Sets the K emission line color.
	void setKEmissionLineColor(const QColor &color);
	/// Sets the L emission line color.
	void setLEmissionLineColor(const QColor &color);
	/// Sets the M emission line color.
	void setMEmissionLineColor(const QColor &color);
	/// Sets the default button color.
	void setDefaultEmissionLineColor(const QColor &color);
	/// Returns the map of names to colors for the emission lines.
	const QMap<QString, QColor> &lineColorMap() const { return emissionLineLegendColors_; }
	/// Returns the K emission line color.
	const QColor kEmissionLineColor() const { return emissionLineLegendColors_.value("K"); }
	/// Returns the L emission line color.
	const QColor lEmissionLineColor() const { return emissionLineLegendColors_.value("L"); }
	/// Returns the M emission line color.
	const QColor mEmissionLineColor() const { return emissionLineLegendColors_.value("M"); }
	/// Returns the default emission line color.
	const QColor defaultEmissionLineColor() const { return emissionLineLegendColors_.value("Default"); }

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
	/// Handles passing on the information when an emission line has been selected.
	void onEmissionLineSelected(const AMEmissionLine &emissionLine);
	/// Handles passing on the information when an emission line has been deselected.
	void onEmissionLineDeselected(const AMEmissionLine &emissionLine);
	/// Removes all of the emission line markers and deselects all of the elements.
	void removeAllEmissionLineMarkers();
	/// Removes all regions of interest.
	void removeAllRegionsOfInterest();

protected:
	/// Method that highlights the region of interest of the current element (if it has been selected).
	void highlightCurrentElementRegionOfInterest(AMElement *element);
	/// Updates the buttons in the periodic table view based on the selected emission lines.  Uses the emission line for appropriate lookups.
	void updatePeriodicTableButtonColors(const AMEmissionLine &line);
	/// Method that builds the style sheet for the regions of interest color.  The key is a string that can have any of the keys mashed together (eg: "KL").  If multiple lines exist then it will make a linear gradient of multiple colors.  Subclasses can re-implement for different stylesheets.
	virtual const QString buildStyleSheet(const QString &colorMapKey) const;

	/// The selectable periodic table model.
	AMSelectablePeriodicTable *periodicTable_;
	/// The selectable periodic table view.
	AMSelectablePeriodicTableView *periodicTableView_;
	/// The selectable element view.
	AMSelectableElementView *elementView_;

	/// The validator for the range and emission line names.
	AMNameAndRangeValidator *emissionLineValidator_;
	/// The list of emission line markers.
	QList<MPlotItem *> emissionLineMarkers_;
	/// A mapping of emission lines to region of interest markers.
	QMap<AMEmissionLine, MPlotMarkerTransparentVerticalRectangle *> regionOfInterestMarkers_;
	/// A simple map for the line colors.
	QMap<QString, QColor> emissionLineLegendColors_;
};

#endif // AMXRFDETAILEDDETECTORVIEW_H
