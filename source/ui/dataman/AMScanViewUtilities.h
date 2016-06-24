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


#ifndef AMSCANVIEWUTILITIES_H
#define AMSCANVIEWUTILITIES_H

#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QToolButton>

#include <QGraphicsView>

#include "AMQGraphics.h"

#include <QButtonGroup>
#include <QPushButton>

#include "dataman/AMScanSetModel.h"
#include "ui/dataman/AMCramBarHorizontal.h"
#include "util/AMRange.h"

/// This file contains some common classes that are used with AMScanView classes.

/// This menu subclass provides a context menu for AMScanViewScanBar
class AMScanViewScanBarContextMenu : public QMenu {
	Q_OBJECT
public:
	AMScanViewScanBarContextMenu(AMScanSetModel* model, int scanIndex, int dataSourceIndex, QWidget* parent = 0);
	virtual ~AMScanViewScanBarContextMenu();

protected:
	/// The model this context menu was created within
	AMScanSetModel* model_;
	/// index of the data source this context menu was created for. We need to use persistent indexes because the data sources might (very unlikely, but possibly) have been deleted while the right-click menu was open.
	QPersistentModelIndex pi_;


protected slots:
	/// Called when the "hide all data sources except this one" action is triggered.
	void hideAllExceptDataSource();
	/// Called when the "show all data sources like this one" action is triggered.
	void showAllDataSource();
	/// Called when the "show all data sources" action is triggered.  This only shows all the data sources for the scan that was selected.  If you want to show all data sources on all open scans then you need to do this for each scan.
	void showAll();
	/// Called when the "edit Color And Style" action is triggered.
	void editColorAndStyle();

};

/// This GUI class is a helper for AMScanViewSourceSelector.  It diplays the available data sources for a single Scan.
class AMScanViewScanBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent = 0);

public slots:
	/// The ScanBar has two behaviours.  When exclusiveMode is on, it only allows one data source to be "checked" or selected at a time, and tells the model to make this the exclusive data source.  Otherwise, it allows multiple data sources to be checked, and toggles their visibility in the model.
	void setExclusiveModeOn(bool exclusiveModeOn = true);

protected:
	/// ui components:
	QLabel* nameLabel_;
	QButtonGroup sourceButtons_;
	AMCramBarHorizontal* cramBar_;

	/// Index of "our" scan in the model:
	int scanIndex_;
	/// Connected model:
	AMScanSetModel* model_;
	/// whether in exclusiveMode (ie: only one data source allowed) or not:
	bool exclusiveModeOn_;

protected slots:
	/// after a scan or data source is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or data source is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or data source is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes:
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	/// when the model's exclusivedata source is changed
	void onExclusiveDataSourceChanged(const QString& exclusiveDataSourceName);
	/// when one of the data source toggles is clicked:
	void onSourceButtonClicked(int id);

	/// called when a right-click menu is requested on any of the buttons. Produces a context menu. (\c location is in source button coordinates)
	void onDataSourceButtonRightClicked(const QPoint& location);

	friend class AMScanViewSourceSelector;
};

/// This GUI class is a view on an AMScanSetModel.  It shows each scan in a horizontal bar, with checkable buttons for each data source.
class AMScanViewSourceSelector : public QWidget {
	Q_OBJECT

public:
	virtual ~AMScanViewSourceSelector();
	explicit AMScanViewSourceSelector(AMScanSetModel* model = 0, QWidget* parent = 0);
	void setModel(AMScanSetModel* model);
	/// Returns true if the exclusive view mode is on, false otherwise.
	bool exclusiveModeOn() const { return exclusiveModeOn_; }

public slots:
	/// ScanBars have two behaviours.  When exclusiveMode is on, they only allow one data source to be "checked" or selected at a time, and tell the model to make this the exclusive data source.  Otherwise, they allows multiple data sources within each Scan to be checked, and toggle the data sources' visibility in the model.
	void setExclusiveModeOn(bool exclusiveModeOn = true);

protected slots:
	/// after a scan or data source is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);

	/// before a scan or data source is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);

	/// after a scan or data source is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end) {
		Q_UNUSED(parent)
		Q_UNUSED(start)
		Q_UNUSED(end)
		/// \todo Anything needed here?
		}

	/// when data within the model changes. Possibilities we care about: nothing. (All handled within AMScanViewScanBars.)
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
		Q_UNUSED(topLeft)
		Q_UNUSED(bottomRight)
		}

protected:
	QList<AMScanViewScanBar*> scanBars_;
	QVBoxLayout* barLayout_;
	AMScanSetModel* model_;
	bool exclusiveModeOn_;
};

/// A GUI class that is a QGraphicsView, and provides a top-level QGraphicsWidget inside a scene.  It emits resized(const QSizeF& newSize) when the QGraphicsView widget is resized.
class AMGraphicsViewAndWidget : public QGraphicsView {
	Q_OBJECT
public:

	AMGraphicsViewAndWidget(QWidget* parent = 0) : QGraphicsView(parent) {

		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing /*| QPainter::HighQualityAntialiasing*/);

		scene_ = new QGraphicsScene();
		setScene(scene_);
		/// \bug testing if disabling item indexing solves anything...
		scene_->setItemIndexMethod(QGraphicsScene::NoIndex);

		graphicsWidget_ = new QGraphicsWidget();
		graphicsWidget_->setGeometry(0,0,640,480);
		scene_->addItem(graphicsWidget_);

	}


	virtual ~AMGraphicsViewAndWidget() {
		graphicsWidget_->deleteLater();
		scene_->deleteLater();
	}

	QGraphicsWidget* graphicsWidget() const { return graphicsWidget_;}

signals:
	void resized(const QSizeF& newSize);

protected:
	QGraphicsScene* scene_;
	QGraphicsWidget* graphicsWidget_;

	// On resize events: notify the graphics widget to resize it.
	virtual void resizeEvent ( QResizeEvent * event ) {

		QGraphicsView::resizeEvent(event);
		emit resized(QSizeF(event->size()));
	}
};

#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"

#include "util/AMSelectablePeriodicTable.h"
#include "util/AMNameAndRangeValidator.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "ui/dataman/AMSpectrumAndPeriodicTableView.h"
#include "dataman/AMnDIndex.h"
#include "dataman/AMAxisInfo.h"

#include <QDoubleSpinBox>

/// This class holds a plot window and shows individual spectra when the mouse is clicked on image points.  It assumes that the spectrum is accessed by the last rank (eg: if the data source is rank 3, it assumes that the scan rank is 2).
class AMScanViewSingleSpectrumView : public AMSpectrumAndPeriodicTableView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a plot.
	AMScanViewSingleSpectrumView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewSingleSpectrumView();

	/// Sets the title for the view.
	void setTitle(const QString &title);
	/// Sets the scale for each point along the x-axis. This also calls setPlotRange to make the ranges match. Set \param propogateToPlotRange to false if you don't want the information to propogate.
	void setAxisInfo(AMAxisInfo info, bool propogateToPlotRange);
	/// This method looks for a data source named \param name and sets it as the only spectrum currently to be viewed.
	void setDataSourceByName(const QString &name);
	/// Sets the plot range used for placing markers inside the plot.
	void setEnergyRange(double low, double high);
	/// Sets a new minimum value for the energy range.
	void setMinimumEnergy(double newMinimum);
	/// Sets a new maximum value for the energy range.
	void setMaximumEnergy(double newMaximum);

//	/// Returns the list of emission line name filters that have provided to the element view.
//	QList<QRegExp> emissionLineNameFilters() const { return emissionLineValidator_->nameFilters(); }
//	/// Returns the emission line name filter at a given index.
//	const QRegExp &emissionLineNameFilterAt(int index) const { return emissionLineValidator_->nameFilterAt(index); }
//	/// Adds a new emission line name filter to the list of name filters.
//	void addEmissionLineNameFilter(const QRegExp &newNameFilter);
//	/// Removes the emission line name filter at the given index.  Returns whether the removal was successful.
//	bool removeEmissionLineNameFilter(int index);
//	/// Removes the emission line name filter with the given name fitler.  Returns whether the removal was successful.
//	bool removeEmissionLineNameFilter(const QRegExp &filter);

//	/// Returns the list of pile up peak name filters that have provided to the element view.
//	QList<QRegExp> pileUpPeakNameFilters() const { return pileUpPeakValidator_->nameFilters(); }
//	/// Returns the pile up peak name filter at a given index.
//	const QRegExp &pileUpPeakNameFilterAt(int index) const { return pileUpPeakValidator_->nameFilterAt(index); }
//	/// Adds a new pile up peak name filter to the list of name filters.
//	void addPileUpPeakNameFilter(const QRegExp &newNameFilter);
//	/// Removes the pile up peak name filter at the given index.  Returns whether the removal was successful.
//	bool removePileUpPeakNameFilter(int index);
//	/// Removes the pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
//	bool removePileUpPeakNameFilter(const QRegExp &filter);

//	/// Returns the list of combination pile up peak name filters that have provided to the element view.
//	QList<QRegExp> combinationPileUpPeakNameFilters() const { return combinationPileUpPeakValidator_->nameFilters(); }
//	/// Returns the combination pile up peak name filter at a given index.
//	const QRegExp &combinationPileUpPeakNameFilterAt(int index) const { return combinationPileUpPeakValidator_->nameFilterAt(index); }
//	/// Adds a new combination pile up peak name filter to the list of name filters.
//	void addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter);
//	/// Removes the combination pile up peak name filter at the given index.  Returns whether the removal was successful.
//	bool removeCombinationPileUpPeakNameFilter(int index);
//	/// Removes the combination pile up peak name filter with the given name fitler.  Returns whether the removal was successful.
//	bool removeCombinationPileUpPeakNameFilter(const QRegExp &filter);

	/// Sets the data source list that can be visualized.
	void setDataSources(const QList<AMDataSource *> &sources);

public slots:
	/// Gives a new coordinate to grab a new spectrum.
	void onDataPositionChanged(const AMnDIndex &index);
	/// Gives two new coordinates to grab a whole set of spectra and add them together.
	void onSelectedRectChanged(const AMnDIndex &start, const AMnDIndex &end);

protected slots:
	/// Slot that updates the plot at index \param index.  Updates the plot with every checked spectrum.  If no parameter is given then it uses the current index.
	void updatePlot(const AMnDIndex &index = AMnDIndex());
	/// Slot that updates the plot by adding all spectra from \param start to \param end.  Updates the plot with every checked spectrum.  If start is null then (0, 0) is assumed, and if end is null, (xSize-1, ySize-1) is assumed.
	void updatePlot(const AMnDIndex &start, const AMnDIndex &end);
	/// Overloaded.  Slot that updates the plot with the spectrum from datasource \param id.
	void updatePlot(int id);
	/// Handles setting a new element to the AMSelectableElementView.
	void onElementClicked(AMElement *element);
	/// Helper slot that adds lines to the plot based on elements being selected from the table.
	void onElementSelected(AMElement *element);
	/// Helper slot that removes lines from the plot based on elements being deselected fromm the table.
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
	/// Method that takes two AMEmissionLines and adds them to the plot as a pile up peak if it would fit.
	void addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine);
	/// Slot that helps handling adding and removing of MPlot items as check boxes are checked on and off.
	void onCheckBoxChanged(int id);
	/// Slot that handles getting the file name and then exporting the data sources to a file.
	void onExportClicked();
//	/// Slot that updates the view based on a change in the minimum range from the minimum spin box.
//	void onMinimumChanged();
//	/// Slot that updates the view vased on a change in the maximum range from the maximum spin box.
//	void onMaximumChanged();
	/// Slot that handles when the log enabled button is toggled.
	void onLogScaleEnabled(bool enable);
	/// Slot that handles if the axis info for a data source changes.
	void onAxisInfoChanged();

protected:
	/// Sets up the plot.
	void setupPlot();
	/// Helper method that removes all of the plot items from the provided list.
	void removeAllPlotItems(QList<MPlotItem *> &items);
	/// Exports the currently selected data sources to the file given by \param filename.
	bool exportToFile(const QString &filename) const;

//	/// The MPlot series that are visualized in the plot.
//	QList<MPlotSeriesBasic *> series_;
//	/// The list that holds all the MPlot data models.
//	QList<MPlotVectorSeriesData *> models_;
//	/// The plot widget that holds everything about the plot.
//	MPlotWidget *plot_;
//	/// Holds the x-axis values so that they do not need to be recomputed everytime.
//	QVector<double> x_;
	/// Holds the start AMnDIndex of where we will grab the spectrum.
	AMnDIndex startIndex_;
	/// Holds the end AMnDIndex of where we will grab the spectrum.
	AMnDIndex endIndex_;
//	/// Flag that holds whether the spectrum view is viewing a single spectrum or adding many spectra together.
//	bool addMultipleSpectra_;
//	/// Holds the list of data sources that can be visualized.
//	QList<AMDataSource *> sources_;
//	/// Holds the button group that is associated with the current list of data sources.
//	QButtonGroup *sourceButtons_;
//	/// The layout that holds the buttons associated with sourceButtons_.
//	QVBoxLayout *sourceButtonsLayout_;
//	/// The export button.
//	QPushButton *exportButton_;

//	/// The title label.
//	QLabel *title_;
//	/// The periodic table model that holds all of the selected elements.
//	AMSelectablePeriodicTable *table_;
//	/// The view that looks at the selectable periodic table model.
//	AMSelectablePeriodicTableView *tableView_;
//	/// Pair that holds the plot range that should be considered.
//	AMRange range_;
//	/// Double spin box that holds the minimum energy of the range.
//	QDoubleSpinBox *minimum_;
//	/// Double spin box that holds the maximum energy of the range.
//	QDoubleSpinBox *maximum_;
//	/// The push button that toggles whether the left axis is scaled logarithmically or not.
//	QPushButton *logEnableButton_;

	/// The pile up peak color.
	QColor pileUpPeakColor_;
	/// The combination pile up peak color.
	QColor combinationPileUpPeakColor_;
//	/// The validator for the range and emission line names.
//	AMNameAndRangeValidator *emissionLineValidator_;
//	/// The validator for the range and emission line names for the pile up peaks.
//	AMNameAndRangeValidator *pileUpPeakValidator_;
//	/// The validator for the range and emission line names for the combination pile up peaks.
//	AMNameAndRangeValidator *combinationPileUpPeakValidator_;
/*	/// The list of emission line markers.
	QList<MPlotItem *> emissionLineMarkers_*/;
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
//	/// Holds the reference AMElement that was last clicked.
//	AMElement *currentElement_;
//	/// Holds the reference AMElement used for the combination pile up peaks.
//	AMElement *combinationElement_;
};

#endif // AMSCANVIEWUTILITIES_H
