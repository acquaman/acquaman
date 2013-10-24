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
	QList<QRegExp> emissionLineNameFilters() const { return emissionLineValidator_->nameFilters(); }
	/// Returns the emission line name filter at a given index.
	const QRegExp &emissionLineNameFilterAt(int index) const { return emissionLineValidator_->nameFilterAt(index); }
	/// Removes the emission line name filter at the given index.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(int index);
	/// Removes the emission line name filter with the given name fitler.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(const QRegExp &filter);
	/// Adds a new emission line name filter to the list of name filters.
	void addEmissionLineNameFilter(const QRegExp &newNameFilter);

	/// Returns the list of pile up peak name filters that have provided to the element view.
	QList<QRegExp> pileUpPeakNameFilters() const { return pileUpPeakValidator_->nameFilters(); }
	/// Returns the pile up peak name filter at a given index.
	const QRegExp &pileUpPeakNameFilterAt(int index) const { return pileUpPeakValidator_->nameFilterAt(index); }
	/// Removes the pile up peak name filter at the given index.  Returns whether the removal was successful.
	bool removePileUpPeakNameFilter(int index);
	/// Removes the pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
	bool removePileUpPeakNameFilter(const QRegExp &filter);
	/// Adds a new pile up peak name filter to the list of name filters.
	void addPileUpPeakNameFilter(const QRegExp &newNameFilter);

	/// Returns the list of combination pile up peak name filters that have provided to the element view.
	QList<QRegExp> combinationPileUpPeakNameFilters() const { return combinationPileUpPeakValidator_->nameFilters(); }
	/// Returns the combination pile up peak name filter at a given index.
	const QRegExp &combinationPileUpPeakNameFilterAt(int index) const { return combinationPileUpPeakValidator_->nameFilterAt(index); }
	/// Removes the combination pile up peak name filter at the given index.  Returns whether the removal was successful.
	bool removeCombinationPileUpPeakNameFilter(int index);
	/// Removes the combination pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
	bool removeCombinationPileUpPeakNameFilter(const QRegExp &filter);
	/// Adds a new combination pile up peak name filter to the list of name filters.
	void addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter);

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
	/// Handles switching which spectrum is displayed in the plot when the combo box index changes.
	void onSpectrumComboBoxIndexChanged(int index);
	/// Handles figuring out which spectra to plot in a waterfall.
	void onShowMultipleSpectraButtonClicked();
	/// Handles updating the waterfall offset.
	void onWaterfallUpdateRequired();
	/// Handles updating the text for the pile up peaks button.
	void updatePileUpPeaksButtonText();
	/// Handles updating the text for the combination pile up peaks button.
	void updateCombinationPileUpPeaksButtonText();
	/// Handles showing the pile up peaks.
	void updatePileUpPeaks();
	/// Handles showing the combination pile up peaks.
	void updateCombinationPileUpPeaks();
	/// Handles updating the combination element by popping up a dialog.
	void onCombinationChoiceButtonClicked();
	/// Handles updating the dead time label.
	void onDeadTimeChanged();
	/// Handles changing the data sources used for the corrected sum PV.
	void onDeadTimeButtonClicked();
	/// Handles updating the region of interest markers using the signal mapper.
	void onRegionOfInterestBoundsChanged(int id);

protected:
	/// Method that highlights the region of interest of the current element (if it has been selected).
	void highlightCurrentElementRegionOfInterest();
	/// Updates the buttons in the periodic table view based on the selected emission lines.  Uses the emission line for appropriate lookups.
	void updatePeriodicTableButtonColors(const AMEmissionLine &line);
	/// Method that builds the style sheet for the regions of interest color.  The key is a string that can have any of the keys mashed together (eg: "KL").  If multiple lines exist then it will make a linear gradient of multiple colors.  Subclasses can re-implement for different stylesheets.
	virtual const QString buildStyleSheet(const QString &colorMapKey) const;
	/// Method that builds the periodic table view and element view and adds it to the detector layout.
	void buildPeriodicTableViewAndElementView();
	/// Method that builds the show spectra and show multiple spectra buttons.
	void buildShowSpectraButtons();
	/// Method that builds the pile up peak buttons.
	void buildPileUpPeakButtons();
	/// Method that builds the dead time GUI elements.
	void buildDeadTimeView();
	/// Method that builds the region of interest views and associated things.
	void buildRegionOfInterestViews();
	/// Helper method that removes all of the plot items from the provided list.
	void removeAllPlotItems(QList<MPlotItem *> &items);
	/// Method that takes two AMEmissionLines and adds them to the plot as a pile up peak if it would fit.
	void addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine);

	/// The selectable periodic table model.
	AMSelectablePeriodicTable *periodicTable_;
	/// The selectable periodic table view.
	AMSelectablePeriodicTableView *periodicTableView_;
	/// The selectable element view.
	AMSelectableElementView *elementView_;

	/// The validator for the range and emission line names.
	AMNameAndRangeValidator *emissionLineValidator_;
	/// The validator for the range and emission line names for the pile up peaks.
	AMNameAndRangeValidator *pileUpPeakValidator_;
	/// The validator for the range and emission line names for the combination pile up peaks.
	AMNameAndRangeValidator *combinationPileUpPeakValidator_;
	/// The list of emission line markers.
	QList<MPlotItem *> emissionLineMarkers_;
	/// A mapping of emission lines to region of interest markers.
	QMap<AMEmissionLine, MPlotMarkerTransparentVerticalRectangle *> regionOfInterestMarkers_;
	/// A signal mapper that maps the MPlotItems to the regions they represent.  Allows easy manipulation of the item's shape.
	QSignalMapper *regionOfInterestMapper_;
	/// A simple map for the line colors.
	QMap<QString, QColor> emissionLineLegendColors_;
	/// The pile up peak color.
	QColor pileUpPeakColor_;
	/// The combination pile up peak color.
	QColor combinationPileUpPeakColor_;

	/// Holds the reference AMElement that was last clicked.
	AMElement *currentElement_;
	/// Holds the reference AMElement used for the combination pile up peaks.
	AMElement *combinationElement_;

	/// The layout for the row above the periodic table view.
	QHBoxLayout *rowAbovePeriodicTableLayout_;
	/// The button for showing the pile up peaks.
	QPushButton *showPileUpPeaksButton_;
	/// The button for showing the combination pile up peaks.
	QPushButton *showCombinationPileUpPeaksButton_;
	/// The button for choosing the second element for combination pile up peaks.
	QToolButton *combinationChoiceButton_;
	/// The list of pile up peaks markers.
	QList<MPlotItem *> pileUpPeakMarkers_;
	/// The list of the combination pile up peaks markers.
	QList<MPlotItem *> combinationPileUpPeakMarkers_;

	/// The check box that holds whether multiple spectra should be shown as a waterfall plot or not.
	QCheckBox *showWaterfall_;

	/// The dead time label.
	QLabel *deadTimeLabel_;
	/// The button group that holds the dead time buttons.
	QButtonGroup *deadTimeButtons_;
	/// The button for showing the regions of interest edit widget.
	QPushButton *editRegionsOfInterestButton_;
};

#endif // AMXRFDETAILEDDETECTORVIEW_H
