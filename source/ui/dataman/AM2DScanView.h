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


#ifndef AM2DSCANVIEW_H
#define AM2DSCANVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotSeries.h"
#include "ui/dataman/AMScanViewUtilities.h"
#include "dataman/AMAxisInfo.h"
#include "dataman/AMnDIndex.h"
#include "util/AMRange.h"

class AM2DScanViewInternal;
class AM2DScanViewExclusiveView;
class AM2DScanViewMultiSourcesView;
class AMScanViewSingleSpectrumView;

#define AM2DSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_NULL_DATA_SOURCE 280201
#define AM2DSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_UNHANDLED_RANK 280202

/// This class is a small horizontal bar that holds some information for the 2D scan view, such as: current data position, whether to see the spectra or not, etc.
class AM2DScanBar : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	virtual ~AM2DScanBar();
	AM2DScanBar(QWidget *parent = 0);

	/// Returns the x axis units.
	QString xUnits() const { return xUnits_; }
	/// Returns the y axis units.
	QString yUnits() const { return yUnits_; }
	/// Returns the position.
	QPointF dataPosition() const { return position_; }
	/// Returns the value.
	double value() const { return value_; }
	/// Returns the minimum value of the range.
	double mininum() const { return range_.minimum(); }
	/// Returns the maximum value of the range.
	double maximum() const { return range_.maximum(); }
	/// Returns the range.
	AMRange range() const { return range_; }
	/// Returns the selected rectangle.
	QRectF selectedRect() const { return rect_; }
	/// Changes the enabled state of the show spectra check box.  If there is no source to view, then this option should not be available.
	void setShowSpectraEnabled(bool enable) { showSpectra_->setEnabled(enable); }
	/// Returns whether the spectrum view is enabled.
	bool showSpectraEnabled() const { return showSpectra_->isEnabled(); }

signals:
	/// Notifier that the state of whether or not the scan view should show individual spectra has been toggled.
	void showSpectra(bool);

public slots:
	/// Handles setting the label with the new data coordinates.
	void setDataPosition(const QPointF &point);
	/// Handles setting the label based on the selected rectangle.
	void setSelectedRect(const QRectF &rect);
	/// Sets the units for both x and y axes.
	void setUnits(const QString &xUnits, const QString &yUnits) { xUnits_ = xUnits; yUnits_ = yUnits; }
	/// Sets the x axis units.
	void setXAxisUnits(const QString &units) { xUnits_ = units; }
	/// Sets the y axis units.
	void setYAxisUnits(const QString &units) { yUnits_ = units; }
	/// Sets the value of the current position.
	void setValue(double value);
	/// Sets the range of data for the current data source.
	void setRange(const AMRange &range);
	/// Sets the minimum value of the range for the current data source.
	void setMinimum(double min);
	/// Sets the maximum value of the range for the current data source.
	void setMaximum(double max);

protected:
	/// Label holding the data position coordinates.
	QLabel *dataPosition_;
	/// Label holding the selected rectangle coordinates.
	QLabel *selectedRect_;
	/// Label holding the value at a given point.
	QLabel *valueLabel_;
	/// Label holding the data range of the current data source.
	QLabel *dataRange_;
	/// String holding the x axis units.
	QString xUnits_;
	/// String holding the y axis units.
	QString yUnits_;
	/// The current point.
	QPointF position_;
	/// The data value at the current position.
	double value_;
	/// The data range for the current data source.
	AMRange range_;
	/// The current selected rectangle.
	QRectF rect_;
	/// The show spectra check box.
	QCheckBox *showSpectra_;
};

/*! This class makes a scan view that is more suitable for 2D scans.  It has been built in the same spirit as AMScanView by having an
	AM2DScanViewInternal class.  This is primarily to reduce confusion when looking at either scan view.  This one is much more limited
	than AMScanView by having only two views: exclusive and multi-datasource-multi-view.  Unlike AMScanView, they are both visualized
	at the same time and the multi-view is not docked inside of the program.
*/
class AM2DScanView : public QWidget
{
	Q_OBJECT

public:
	/// Constructs a new AM2DScanView based on a set of scans given by \c model. If \c model is 0,  it creates its own model to use internally, otherwise it uses the supplied model.
	explicit AM2DScanView(AMScanSetModel* model = 0, QWidget *parent = 0);
	virtual ~AM2DScanView();

	/// returns the AMScanSetModel used internally to hold the scans/data sources.
	AMScanSetModel* model() const { return scansModel_; }
	/// Returns a pointer to the current scan the view is looking at.
	AMScan *currentScan() const { return currentScan_; }
	/// Returns the current position.  This holds the x and y coordinates from the last time the data position tool was moved.
	QPointF dataPosition() const { return exclusive2DScanBar_->dataPosition(); }
	/// Returns the current selected rectangle.
	QRectF selectedRect() const { return exclusive2DScanBar_->selectedRect(); }
	/// Sets the default axis information for the spectrum view. Set \param propogateToPlotRange to false if you don't want the information to propogate.
	void setAxisInfoForSpectrumView(const AMAxisInfo &info, bool propogateToPlotRange = true);
	/// Sets the plot range for the spectrum view.
	void setPlotRange(double low, double high);
	/// Sets the emission line name filter for the single spectrum view.
	void addSingleSpectrumEmissionLineNameFilter(const QRegExp &newNameFilter);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumEmissionLineNameFilter(int index);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumEmissionLineNameFilter(const QRegExp &filter);
	/// Sets the emission line name filter for the single spectrum view.
	void addSingleSpectrumPileUpPeakNameFilter(const QRegExp &newNameFilter);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumPileUpPeakNameFilter(int index);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumPileUpPeakNameFilter(const QRegExp &filter);
	/// Sets the emission line name filter for the single spectrum view.
	void addSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumCombinationPileUpPeakNameFilter(int index);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &filter);
	/// Sets the single spectrum view data source using the name given by \param name.
	void setSingleSpectrumDataSource(const QString &name);

public slots:
	/// add a scan to the view:
	void addScan(AMScan* scan);
	/// remove a scan from the view:
	void removeScan(AMScan* scan);
	/// Sets the current scan.  This is so that the scan view is looking at the same scan as all of the other pieces of the editor.
	void setCurrentScan(AMScan *scan);

	/// Export the current view to a PDF file with \c outputFileName. Overwrites \c outputFileName if it exists.
	void exportGraphicsFile(const QString& outputFileName);
	void printGraphics();



signals:
	/// Notifier that the data position tool has changed locations.  Passes the location of the mouse.
	void dataPositionChanged(const QPoint &);

protected slots:
	/// Slot that resizes the exclusive view as needed.
	void resizeExclusiveViews();
	/// Slot that resizes the multi view as needed.
	void resizeMultiViews();
	/// Slots that handles the visibility of the spectrum view based on the information from the scan bar.
	void setSpectrumViewVisibility(bool visible);
	/// Helper slot that makes sure all of the information that the spectrum fetcher needs is setup for a single point.
	void onDataPositionChanged(const QPointF &point);
	/// Helper slot that makes sure all the information that the spectrum fetcher needs when using the rect instead of a point.
	void onSelectedRectChanged(const QRectF &rect);
	/// Helper slot that computes the min and max value of the currently viewed data source.
	void onExclusiveDataSourceChanged(const QString &name);
	/// Helper slot that updates the range if the values change for the current exclusive data source.
	void onExclusiveDataSourceValueChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Helper slot that helps setup the single spectrum view after a scan has been added.
	void onScanAdded(AMScan *scan);

protected:
	/// Reimplements the show event to hide the multi view.
	virtual void showEvent(QShowEvent *e);
	/// Reimplements the hide event to hide the multi view.
	virtual void hideEvent(QHideEvent *e);
	/// Reimplementing the mouse release event so that it will emit a signal on right clicks to notify parent classes that the data position tool has changed.
	virtual void mousePressEvent(QMouseEvent *e);

	/// internal helper function to build the UI
	void setupUI();
	/// internal helper function to setup all UI event-handling connections
	void makeConnections();
	/// Helper method that returns the AMnDIndex for a given QPoint of data coordinates.
	AMnDIndex getIndex(const QPointF &point) const;
	/// Helper method that returns the minimum and maximum value for the current data source.  If no start and end values are given then the entire data source will be searched.
	AMRange getCurrentExclusiveDataSourceRange(const AMnDIndex &start = AMnDIndex(), const AMnDIndex &end = AMnDIndex()) const;

	/// The scans set model.
	AMScanSetModel* scansModel_;
	/// Pointer to the current scan.  Used for ensuring units are correct in the AMScanBar.
	AMScan *currentScan_;
	/// Pointer to the current data source in the exclusive mode.
	AMDataSource *currentExclusiveDataSource_;

	/// The exclusive view
	AM2DScanViewExclusiveView *exclusiveView_;
	/// The multi view.
	AM2DScanViewMultiSourcesView *multiView_;

	/// The individual spectrum view.
	AMScanViewSingleSpectrumView *spectrumView_;
	/// Flag used to determine whether the single spectrum view should be visible.
	bool spectrumViewIsVisible_;

	// ui components:
	AMGraphicsViewAndWidget* gExclusiveView_;
	QGraphicsLinearLayout* gExclusiveLayout_;
	AMScanViewSourceSelector* exclusiveScanBars_;
	AM2DScanBar *exclusive2DScanBar_;

	AMGraphicsViewAndWidget* gMultiView_;
	QGraphicsLinearLayout *gMultiViewLayout_;
	AMScanViewSourceSelector* multiScanBars_;

	QGroupBox *multiViewBox_;
	QGroupBox *spectrumViewBox_;
};

/// This class handles changes in the scan set model and propogates it to both views inside of it.
class AM2DScanViewInternal : public QGraphicsWidget
{
	Q_OBJECT

public:
	/// Constructor.
	virtual ~AM2DScanViewInternal();
	explicit AM2DScanViewInternal(AM2DScanView *masterView);

protected slots:
	/// after a scan or data source is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end) = 0;
	/// before a scan or data source is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) = 0;
	/// after a scan or data source is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end) = 0;
	/// when data changes:
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) = 0;

protected:
	/// Helper function to create an appropriate MPlotItem and connect it to the \c dataSource, depending on the dimensionality of \c dataSource.  Returns 0 if we can't handle this dataSource and no item was created (ex: unsupported dimensionality, we only handle 1D or 2D data for now.)
	MPlotItem* createPlotItemForDataSource(const AMDataSource* dataSource, const AMDataSourcePlotSettings& plotSettings);

	/// Returns the scan set model this scan view visualizes.
	AMScanSetModel* model() const;
	/// Creates a standard plot and puts it into an MPlot graphics widget.
	MPlotGW* createDefaultPlot();

	/// Holds the pointer to the master view.
	AM2DScanView* masterView_;
};

class AM2DScanViewExclusiveView : public AM2DScanViewInternal
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AM2DScanViewExclusiveView(AM2DScanView *masterView);
	virtual ~AM2DScanViewExclusiveView();

signals:
	/// Notifier that the data position marker has changed.
	void dataPositionChanged(const QPointF &);
	/// Notifier that the selected rectangle coordinates have changed.
	void selectedRectChanged(const QRectF &);

protected slots:
	/// after a scan or data source is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or data source is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or data source is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes: (ex: line color, plot settings, etc.)
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

	/// when the model's "exclusive data source" changes. This is the one data source that we display for all of our scans (as long as they have it).
	void onExclusiveDataSourceChanged(const QString& exclusiveDataSource);

protected:
	/// Helper function to handle adding a scan (at row scanIndex in the model)
	void addScan(int scanIndex);
	/// Helper function to handle review a scan when a data source is added or the exclusive data source changes.
	void reviewScan(int scanIndex);
	/// Helper function to review how many scans are actually displayed (ie: how many have the exclusive data source), and update the plot legend title
	void refreshTitle();

	/// A list of MPlotItem*... one item for each scan. If we don't have a plot item shown for this scan, this list stores a null pointer.
	/*! A null pointer in plotItems_ means that the scan at that index doesn't have a data source matching the exclusive data source... or there is a data source, but we're unable to display it (ex: unsupported dimensionality, etc.).*/
	QList<MPlotItem*> plotItems_;
	/// This list is a parallel to plotItems_, but it holds a pointer to the data source that we're displaying for this plot.
	/*! A null pointer in plotItemDataSources_ means that the scan at that index doesn't have a data source matching the exclusive data source. */
	QList<AMDataSource*> plotItemDataSources_;

	/// Our plot.
	MPlotGW* plot_;
};

class AM2DScanViewMultiSourcesView : public AM2DScanViewInternal
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AM2DScanViewMultiSourcesView(AM2DScanView *masterView);
	/// Destructor.
	virtual ~AM2DScanViewMultiSourcesView();

protected slots:
	/// after a scan or data source is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or data source is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or data source is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes: (Things we care about: color, linePen, and visible)
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);


protected:
	/// Reimplemented mouse press event.  Determines which data source widget is being clicked on.
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	/// helper function: reviews the data sources that exist/are visible, and ensures plots correspond. Returns true if data source plots were created or deleted (which would require a reLayout()).
	bool reviewDataSources();

	/// re-do the layout of our plots
	void reLayout();

	/// Our plots (one for each data source), indexed by data source name
	QMap<QString, MPlotGW*> dataSource2Plot_;
	/// Our plot items, indexed by data source name and then by Scan pointer
	QMap<QString, QHash<AMScan*, MPlotItem*> > sourceAndScan2PlotItem_;


	/// A grid-layout within which to put our plots:
	QGraphicsGridLayout* layout_;

	/// true if the first plot (firstPlot_) exists already, but isn't used. (ie: free for the using)
	bool firstPlotEmpty_;
	/// When dataSource2Plot_ is empty, we keep a single plot here, to make sure that there's always at least one shown.
	MPlotGW* firstPlot_;
};

#endif // AM2DSCANVIEW_H
