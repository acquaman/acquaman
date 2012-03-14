#ifndef AM2DSCANVIEW_H
#define AM2DSCANVIEW_H

#include <QWidget>
#include <QPropertyAnimation>

#include "MPlot/MPlot.h"
#include "ui/dataman/AMScanViewUtilities.h"

class AM2DScanViewInternal;

/*! This class makes a scan view that is more suitable for 2D scans.  It has been built in the same spirit as AMScanView by having an
	AM2DScanViewInternal class.  This is primarily to reduce confusion when looking at either scan view.  This one is much more limited
	than AMScanView by having only two views: exclusive and multi-datasource-multi-view.  Unlike AMScanView, they are both visualized
	at the same time and the multi-view is not docked inside of the program.
*/
class AM2DScanView : public QWidget
{
	Q_OBJECT

public:
	/// Constructs a new AM2DScanView based on a set of scans given by \c model. If \c model is 0,  it creates its own model to use internally; otherwise it uses the supplied model.
	explicit AM2DScanView(AMScanSetModel* model = 0, QWidget *parent = 0);
	virtual ~AM2DScanView();

	/// returns the AMScanSetModel used internally to hold the scans/data sources.
	AMScanSetModel* model() const { return scansModel_; }

public slots:
	/// add a scan to the view:
	void addScan(AMScan* scan);
	/// remove a scan from the view:
	void removeScan(AMScan* scan);

protected slots:
	/// Slot that resizes the views as needed.
	void resizeViews();

protected:
	/// Reimplements the show event to hide the multi view.
	virtual void showEvent(QShowEvent *e);
	/// Reimplements the hide event to hide the multi view.
	virtual void hideEvent(QHideEvent *e);

	/// internal helper function to build the UI
	void setupUI();
	/// internal helper function to setup all UI event-handling connections
	void makeConnections();

	AMScanSetModel* scansModel_;

	/// List of the different views available
	QList<AM2DScanViewInternal*> views_;

	// ui components:
	AMGraphicsViewAndWidget* gview_;
	QGraphicsLinearLayout* glayout_;
	AMScanViewSourceSelector* scanBars_;

	QPropertyAnimation* modeAnim_;
};

/// This class handles changes in the scan set model and propogates it to both views inside of it.
class AM2DScanViewInternal : public QGraphicsWidget
{
	Q_OBJECT

public:
	/// Constructor.
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
	/// Helper function to create an appropriate MPlotItem and connect it to the \c dataSource, depending on the dimensionality of \c dataSource.  Returns 0 if we can't handle this dataSource and no item was created (ex: unsupported dimensionality; we only handle 1D or 2D data for now.)
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
