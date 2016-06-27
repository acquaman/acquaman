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


#ifndef AMXRFDETAILEDDETECTORVIEW_H
#define AMXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFBaseDetectorView.h"

#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include "util/AMSelectablePeriodicTable.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "ui/util/AMSelectableElementView.h"
#include "util/AMNameAndRangeValidator.h"

#include <QSignalMapper>

class QComboBox;
class AMHeaderButton;
class AMChooseScanDialog;
class AMExportController;
class AMExporterOption;

class AMXRFDetailedDetectorView : public AMXRFBaseDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a more detailed view for AMXRFDetectors.
	AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMXRFDetailedDetectorView();

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

	/// Returns the energy range used for filtering.  If no range has been provided the range is null.
	const AMRange &energyRange() const { return emissionLineValidator_->range(); }
	/// Returns the minimum energy in the energy range filter.
	double minimumEnergy() const { return emissionLineValidator_->minimum(); }
	/// Returns the maximum energy in the energy range filter.
	double maximumEnergy() const { return emissionLineValidator_->maximum(); }

//	/// Returns the list of emission line name filters that have provided to the element view.
//	QList<QRegExp> emissionLineNameFilters() const { return emissionLineValidator_->nameFilters(); }
//	/// Returns the emission line name filter at a given index.
//	const QRegExp &emissionLineNameFilterAt(int index) const { return emissionLineValidator_->nameFilterAt(index); }
//	/// Removes the emission line name filter at the given index.  Returns whether the removal was successful.
//	bool removeEmissionLineNameFilter(int index);
//	/// Removes the emission line name filter with the given name fitler.  Returns whether the removal was successful.
//	bool removeEmissionLineNameFilter(const QRegExp &filter);
//	/// Adds a new emission line name filter to the list of name filters.
//	void addEmissionLineNameFilter(const QRegExp &newNameFilter);

//	/// Returns the list of pile up peak name filters that have provided to the element view.
//	QList<QRegExp> pileUpPeakNameFilters() const { return pileUpPeakValidator_->nameFilters(); }
//	/// Returns the pile up peak name filter at a given index.
//	const QRegExp &pileUpPeakNameFilterAt(int index) const { return pileUpPeakValidator_->nameFilterAt(index); }
//	/// Removes the pile up peak name filter at the given index.  Returns whether the removal was successful.
//	bool removePileUpPeakNameFilter(int index);
//	/// Removes the pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
//	bool removePileUpPeakNameFilter(const QRegExp &filter);
//	/// Adds a new pile up peak name filter to the list of name filters.
//	void addPileUpPeakNameFilter(const QRegExp &newNameFilter);

//	/// Returns the list of combination pile up peak name filters that have provided to the element view.
//	QList<QRegExp> combinationPileUpPeakNameFilters() const { return combinationPileUpPeakValidator_->nameFilters(); }
//	/// Returns the combination pile up peak name filter at a given index.
//	const QRegExp &combinationPileUpPeakNameFilterAt(int index) const { return combinationPileUpPeakValidator_->nameFilterAt(index); }
//	/// Removes the combination pile up peak name filter at the given index.  Returns whether the removal was successful.
//	bool removeCombinationPileUpPeakNameFilter(int index);
//	/// Removes the combination pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
//	bool removeCombinationPileUpPeakNameFilter(const QRegExp &filter);
//	/// Adds a new combination pile up peak name filter to the list of name filters.
//	void addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter);

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

	/// Sets the pile up peak color.
	void setPileUpPeakColor(const QColor &color);
	/// Returns pile up peak color.
	const QColor pileUpPeakColor() const { return pileUpPeakColor_; }
	/// Sets the combination pile up peak color.
	void setCombinationPileUpPeakColor(const QColor &color);
	/// Returns combination pile up peak color.
	const QColor combinationPileUpPeakColor() const { return combinationPileUpPeakColor_; }

public slots:
	/// Sets the energy range filter for this view.
	void setEnergyRange(const AMRange &newRange);
	/// Sets the energy range filter for this view.
	void setEnergyRange(double minimum, double maximum);
	/// Sets a new minimum value for the energy range.
	void setMinimumEnergy(double newMinimum);
	/// Sets a new maximum value for the energy range.
	void setMaximumEnergy(double newMaximum);

	/// Expands (shows) the periodic table views
	void expandPeriodicTableViews();
	/// Collapses (hides) the periodic table views
	void collapsePeriodTableViews();

signals:
	/// Emitted when the periodic table views are hidden
	void resized();

protected slots:
	/// Starts the acquisition.  Treats the acquisition like a scan and saves the result to the database.
	virtual void startAcquisition();
	/// Handles bringing up and exporting the given XRF scans.
	void onSaveButtonClicked();
	/// Handles grabbing the scan and exporting it.
	void exportScan();
	/// Handles deleting the export controller.
	void onExportControllerStateChanged(int state);

	/// Handles setting a new element to the AMSelectableElementView.
	void onElementClicked(AMElement *element);
//	/// Handles changing the look of the periodic table view if an element is selected.
//	void onElementSelected(AMElement *element);
//	/// Handles changing the look of the periodic table view if an element is deselected.
//	void onElementDeselected(AMElement *element);
//	/// Updates the emission line list after the energy range or name filter has been changed.
//	void updateEmissionLineMarkers();
	/// Handles passing on the information when an emission line has been selected.
	void onEmissionLineSelected(const AMEmissionLine &emissionLine);
	/// Handles passing on the information when an emission line has been deselected.
	void onEmissionLineDeselected(const AMEmissionLine &emissionLine);
	/// Handles adding the region of interest to the view with the provided region.
	void onRegionOfInterestAdded(AMRegionOfInterest *newRegion);
	/// Handles removing the region of interest from the view with the provided region.
	void onRegionOfInterestRemoved(AMRegionOfInterest *region);
//	/// Removes all of the emission line markers and deselects all of the elements.
//	void removeAllEmissionLineMarkers();
	/// Removes all regions of interest.
	void removeAllRegionsOfInterest();
	/// Handles switching which spectrum is displayed in the plot when the combo box index changes.
	void onSpectrumComboBoxIndexChanged(int index);
	/// Handles figuring out which spectra to plot in a waterfall.
	void onShowMultipleSpectraButtonClicked();
	/// Handles updating the waterfall offset.
	void onWaterfallUpdateRequired();
//	/// Handles updating the text for the pile up peaks button.
//	void updatePileUpPeaksButtonText();
//	/// Handles updating the text for the combination pile up peaks button.
//	void updateCombinationPileUpPeaksButtonText();
//	/// Handles showing the pile up peaks.
//	void updatePileUpPeaks();
//	/// Handles showing the combination pile up peaks.
//	void updateCombinationPileUpPeaks();
//	/// Handles updating the combination element by popping up a dialog.
//	void onCombinationChoiceButtonClicked();
	/// Handles updating the dead time label.
	void onDeadTimeChanged();
	/// Handles changing the data sources used for the corrected sum PV.
	void onDeadTimeButtonClicked(int deadTimeButtonId);
	/// Handles updating the look of a button if it is enabled or not.
	void onElementEnabledOrDisabled(int elementId);
	/// Handles updating the region of interest markers using the signal mapper.
	void onRegionOfInterestBoundsChanged(QObject *id);
	/// Handles changing the scale of the axis to logarithmic or linear.
	void onLogScaleClicked(bool logScale);
	/// Handles setting the minimum energy after a new input from the spin box.
//	void onMinimumEnergyChanged();
//	/// Handles setting the maximum energy after a new input from the spin box.
//	void onMaximumEnergyChanged();

	/// Handles showing or hiding the periodic table related views when the button is clicked
	void onPeriodicTableHeaderButtonClicked();
	/// Handles resizing the vertical height to a minimum size when the periodic table views are hidden
	void resizeToMinimumHeight();

protected:
	/// Builds the option that is going to be used by the XRF detector export.
	AMExporterOption *buildExporterOption() const;
	/// Method that highlights the region of interest of the current element (if it has been selected).
	void highlightCurrentElementRegionOfInterest();
	/// Updates the buttons in the periodic table view based on the selected emission lines.  Uses the element symbol for appropriate lookups.
	void updatePeriodicTableButtonColors(const QString &symbol);
	/// Method that builds the style sheet for the regions of interest color.  The key is a string that can have any of the keys mashed together (eg: "KL").  If multiple lines exist then it will make a linear gradient of multiple colors.  Subclasses can re-implement for different stylesheets.
	virtual const QString buildStyleSheet(const QString &colorMapKey) const;
	/// Method that builds the periodic table view and element view and adds it to the detector layout.
	virtual void buildPeriodicTableViewAndElementView();
	/// Method that builds the show spectra and show multiple spectra buttons.
	virtual void buildShowSpectraButtons();
	/// Method that builds the pile up peak buttons.
	virtual void buildPileUpPeakButtons();
	/// Method that builds the dead time GUI elements.
	virtual void buildDeadTimeView();
	/// Method that builds the region of interest views and associated things.
	virtual void buildRegionOfInterestViews();
	/// Method that builds two spin boxes that will change the energy range filter.
	virtual void buildEnergyRangeSpinBoxView();
//	/// Helper method that removes all of the plot items from the provided list.
//	void removeAllPlotItems(QList<MPlotItem *> &items);
	/// Method that takes two AMEmissionLines and adds them to the plot as a pile up peak if it would fit.
	void addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine);
	/// Removes all the region of interest pieces from the view.
	void removeRegionOfInterestItems(AMRegionOfInterest *region);

	/// Helper method to show or hide the periodic table related views and fix the header button
	void hidePeriodicTableViews(bool setHidden);

	/// Implementation method for setMinimumEnergy.  The view handles all the visuals, but if there is something specific that needs to be passed on to the detector or viewing subclass, you can implement that here.
	virtual void setMinimumEnergyImplementation(double energy){ Q_UNUSED(energy); }
	/// Implementation method for setMaximumEnergy.  The view handles all the visuals, but if there is something specific that needs to be passed on to the detector or viewing subclass, you can implement that here.
	virtual void setMaximumEnergyImplementation(double energy){ Q_UNUSED(energy); }

	/// The drop down button to show or hide the periodic table related information
	AMHeaderButton *periodicTableHeaderButton_;
	/// Widget to hold all of the periodic table related views, this way we can hide and show them all at once
	QWidget *bottomLayoutWidget_;
//	/// The selectable periodic table model.
//	AMSelectablePeriodicTable *periodicTable_;
//	/// The selectable periodic table view.
//	AMSelectablePeriodicTableView *periodicTableView_;
	/// The selectable element view.
	AMSelectableElementView *elementView_;

//	/// The validator for the range and emission line names.
//	AMNameAndRangeValidator *emissionLineValidator_;
//	/// The validator for the range and emission line names for the pile up peaks.
//	AMNameAndRangeValidator *pileUpPeakValidator_;
//	/// The validator for the range and emission line names for the combination pile up peaks.
//	AMNameAndRangeValidator *combinationPileUpPeakValidator_;
//	/// The list of emission line markers.
//	QList<MPlotItem *> emissionLineMarkers_;
	/// A mapping of emission lines to region of interest markers.
	QMap<AMRegionOfInterest *, MPlotMarkerTransparentVerticalRectangle *> regionOfInterestMarkers_;
	/// A signal mapper that maps the MPlotItems to the regions they represent.  Allows easy manipulation of the item's shape.
	QSignalMapper *regionOfInterestMapper_;
	/// A simple map for the line colors.
	QMap<QString, QColor> emissionLineLegendColors_;
//	/// The pile up peak color.
//	QColor pileUpPeakColor_;
//	/// The combination pile up peak color.
//	QColor combinationPileUpPeakColor_;

//	/// Holds the reference AMElement that was last clicked.
//	AMElement *currentElement_;
//	/// Holds the reference AMElement used for the combination pile up peaks.
//	AMElement *combinationElement_;

//	/// The layout for the row above the periodic table view.
//	QHBoxLayout *rowAbovePeriodicTableLayout_;
//	/// The button for showing the pile up peaks.
//	QPushButton *showPileUpPeaksButton_;
//	/// The button for showing the combination pile up peaks.
//	QPushButton *showCombinationPileUpPeaksButton_;
//	/// The button for choosing the second element for combination pile up peaks.
//	QToolButton *combinationChoiceButton_;
//	/// The list of pile up peaks markers.
//	QList<MPlotItem *> pileUpPeakMarkers_;
//	/// The list of the combination pile up peaks markers.
//	QList<MPlotItem *> combinationPileUpPeakMarkers_;

	/// The combo box that chooses which spectra to look at
	QComboBox *spectraComboBox_;
	/// The check box that holds whether multiple spectra should be shown as a waterfall plot or not.
	QCheckBox *showWaterfall_;
//	/// The button that controls whether the plot is log scale or not.
//	QPushButton *logScaleButton_;

	/// The dead time label.
	QLabel *deadTimeLabel_;
	/// The button group that holds the dead time buttons.
	QButtonGroup *deadTimeButtons_;
	/// The button for showing the regions of interest edit widget.
	QPushButton *editRegionsOfInterestButton_;

	/// The layout for the energy range ui elements.
	QVBoxLayout *energyRangeLayout_;
	/// The button that shows or hides the energy range spin boxes.
	QPushButton *showEnergyRangeSpinBoxes_;
//	/// The minimum energy spin box.
//	QDoubleSpinBox *minimum_;
//	/// The maximum energy spin box.
//	QDoubleSpinBox *maximum_;
//	/// The export button.
//	QPushButton *exportButton_;

	/// A dead time view factor.  Should be customized in the constructor.
	int deadTimeViewFactor_;

	/// Choose scan dialog.
	AMChooseScanDialog *chooseScanDialog_;
	/// The export controller.
	AMExportController *exportController_;
};

#endif // AMXRFDETAILEDDETECTORVIEW_H
