#ifndef AMSPECTRUMANDPERIODICTABLEVIEW_H
#define AMSPECTRUMANDPERIODICTABLEVIEW_H

#include <QWidget>

#include "beamline/AMDetector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "ui/AMTopFrame.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QButtonGroup>

#include "util/AMSelectablePeriodicTable.h"
#include "util/AMNameAndRangeValidator.h"
#include "util/AMRange.h"
#include "ui/util/AMSelectableElementView.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "dataman/AMScanSetModel.h"
#include "dataman/AMnDIndex.h"
#include "dataman/AMAxisInfo.h"

class AMSpectrumAndPeriodicTableView : public QWidget
{
	Q_OBJECT

public:
	AMSpectrumAndPeriodicTableView(QWidget *parent = 0);

	virtual ~AMSpectrumAndPeriodicTableView();


	/// Energy Settings

	void setEnergyRange(double low, double high);

	void setEnergyRange(const AMRange &newRange);

	void setMinimumEnergy(double newMinimum);

	void setMaximumEnergy(double newMaximum);

	/// Sets the scale for each point along the x-axis. This also calls setPlotRange to make the ranges match. Set \param propogateToPlotRange to false if you don't want the information to propogate.
	void setAxisInfo(AMAxisInfo info, bool propogateToPlotRange);

	/// Emission Lines
	/// Returns the list of emission line name filters that have provided to the element view.
	QList<QRegExp> emissionLineNameFilters() const { return emissionLineValidator_->nameFilters(); }
	/// Returns the emission line name filter at a given index.
	const QRegExp &emissionLineNameFilterAt(int index) const { return emissionLineValidator_->nameFilterAt(index); }
	/// Adds a new emission line name filter to the list of name filters.
	void addEmissionLineNameFilter(const QRegExp &newNameFilter);
	/// Removes the emission line name filter at the given index.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(int index);
	/// Removes the emission line name filter with the given name fitler.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(const QRegExp &filter);

	/// Peaks
	/// Returns the list of pile up peak name filters that have provided to the element view.
	QList<QRegExp> pileUpPeakNameFilters() const { return pileUpPeakValidator_->nameFilters(); }
	/// Returns the pile up peak name filter at a given index.
	const QRegExp &pileUpPeakNameFilterAt(int index) const { return pileUpPeakValidator_->nameFilterAt(index); }
	/// Adds a new pile up peak name filter to the list of name filters.
	void addPileUpPeakNameFilter(const QRegExp &newNameFilter);
	/// Removes the pile up peak name filter at the given index.  Returns whether the removal was successful.
	bool removePileUpPeakNameFilter(int index);
	/// Removes the pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
	bool removePileUpPeakNameFilter(const QRegExp &filter);


	/// Returns the list of combination pile up peak name filters that have provided to the element view.
	QList<QRegExp> combinationPileUpPeakNameFilters() const { return combinationPileUpPeakValidator_->nameFilters(); }
	/// Returns the combination pile up peak name filter at a given index.
	const QRegExp &combinationPileUpPeakNameFilterAt(int index) const { return combinationPileUpPeakValidator_->nameFilterAt(index); }
	/// Adds a new combination pile up peak name filter to the list of name filters.
	void addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter);
	/// Removes the combination pile up peak name filter at the given index.  Returns whether the removal was successful.
	bool removeCombinationPileUpPeakNameFilter(int index);
	/// Removes the combination pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
	bool removeCombinationPileUpPeakNameFilter(const QRegExp &filter);

protected slots:
	/// Handles setting a new element to the AMSelectableElementView.
	void onElementClicked(AMElement *element);
	/// Handles changing the look of the periodic table view if an element is selected.
	void onElementSelected(AMElement *element);
	/// Handles changing the look of the periodic table view if an element is deselected.
	void onElementDeselected(AMElement *element);
	/// Updates the emission line list after the energy range or name filter has been changed.
	void updateEmissionLineMarkers();
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
	/// Removes all of the emission line markers and deselects all of the elements.
	void removeAllEmissionLineMarkers();
	/// Handles setting the minimum energy after a new input from the spin box.
	void onMinimumEnergyChanged();
	/// Handles setting the maximum energy after a new input from the spin box.
	void onMaximumEnergyChanged();
	/// Slot that handles when the log enabled button is toggled.
	void onLogScaleEnabled(bool enable);
	/// Slot that handles if the axis info for a data source changes.
	void onAxisInfoChanged();

	/// Method that takes two AMEmissionLines and adds them to the plot as a pile up peak if it would fit.
	void addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine);

protected:
	/// Sets up the plot.
	virtual void setupPlot();
	/// Sets up the energy range spin boxes and export option.
	virtual void buildEnergyRangeSpinBoxView();
	/// Builds pile up peak buttons.
	virtual void buildPileUpPeakButtons();
	/// Helper method that removes all of the plot items from the provided list.
	virtual void removeAllPlotItems(QList<MPlotItem *> &items);

	/// Holds the list of data sources that can be visualized.
	QList<AMDataSource *> sources_;
	/// Holds the button group that is associated with the current list of data sources.
	QButtonGroup *sourceButtons_;

	QVBoxLayout *sourceButtonsLayout_;

	QHBoxLayout *rowAbovePeriodicTableLayout_;

	/// Reference to the plot itself.
	MPlot *plot_;
	/// The plot widget that holds everything about the plot.
	MPlotWidget *plotView_;
	/// Holds the x-axis values so that they do not need to be recomputed everytime.
	QVector<double> x_;

	/// Flag that holds whether the spectrum view is viewing a single spectrum or adding many spectra together.
	bool addMultipleSpectra_;

	/// The title label.
	QLabel *title_;
	/// The periodic table model that holds all of the selected elements.
	AMSelectablePeriodicTable *periodicTable_;
	/// The view that looks at the selectable periodic table model.
	AMSelectablePeriodicTableView *periodicTableView_;
	/// Pair that holds the plot range that should be considered.
	AMRange range_;
	/// Double spin box that holds the minimum energy of the range.
	QDoubleSpinBox *minimum_;
	/// Double spin box that holds the maximum energy of the range.
	QDoubleSpinBox *maximum_;
	/// The push button that toggles whether the left axis is scaled logarithmically or not.
	QPushButton *logEnableButton_;


	/// The MPlot series that are visualized in the plot.
	QList<MPlotSeriesBasic *> series_;
	/// The list that holds all the MPlot data models.
	QList<MPlotVectorSeriesData *> models_;


	/// The export button.
	QPushButton *exportButton_;
	/// The button for choosing the second element for combination pile up peaks.
	QToolButton *combinationChoiceButton_;

	/// Holds the reference AMElement that was last clicked.
	AMElement *currentElement_;
	/// Holds the reference AMElement used for the combination pile up peaks.
	AMElement *combinationElement_;

	/// The list of pile up peaks markers.
	QList<MPlotItem *> pileUpPeakMarkers_;
	/// The list of the combination pile up peaks markers.
	QList<MPlotItem *> combinationPileUpPeakMarkers_;
	/// The list of emission line markers.
	QList<MPlotItem *> emissionLineMarkers_;

	/// The button for showing the pile up peaks.
	QPushButton *showPileUpPeaksButton_;
	/// The button for showing the combination pile up peaks.
	QPushButton *showCombinationPileUpPeaksButton_;

	AMNameAndRangeValidator *emissionLineValidator_;
	/// The validator for the range and emission line names for the pile up peaks.
	AMNameAndRangeValidator *pileUpPeakValidator_;
	/// The validator for the range and emission line names for the combination pile up peaks.
	AMNameAndRangeValidator *combinationPileUpPeakValidator_;



};

#endif // AMSPECTRUMANDPERIODICTABLEVIEW_H
