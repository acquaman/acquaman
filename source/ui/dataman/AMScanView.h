/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSCANVIEW_H
#define AMSCANVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QLayout>
#include <QButtonGroup>
#include <QStringList>

#include "MPlot/MPlot.h"

#include "dataman/AMScanSetModel.h"

#include "ui/dataman/AMCramBarHorizontal.h"
#include "ui/dataman/AMScanViewUtilities.h"

#include <QMenu>

class QCheckBox;
class QDoubleSpinBox;

/// Contains the GUI buttons used by AMScanView to switch between different view modes
class AMScanViewModeBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QButtonGroup* modeButtons_;

	QCheckBox *logCheckBox_;
	QCheckBox* normalizationCheckBox_, *waterfallCheckBox_;
	QDoubleSpinBox* waterfallAmount_;

signals:
	void logScaleEnabled(bool);
	void normalizationEnabled(bool);
	void waterfallOffsetEnabled(bool);
	void waterfallOffsetChanged(double);


};

class AMScanView;

/// This class is the interface for different view options inside an AMScanView.  They must be able to handle changes from the AMScanSet model (scans or data sources added or removed).
class AMScanViewInternal : public QGraphicsWidget {
	Q_OBJECT
public:
	explicit AMScanViewInternal(AMScanView* masterView);

public slots:
	/// Must re-implement in subclasses: turn on log scale on the y-axis.
	virtual void enableLogScale(bool logScaleOn = true) { logScaleEnabled_ = logScaleOn; }
	/// Must re-implement in subclasses: turn on axis normalization on the y-axis
	virtual void enableNormalization(bool normalizationOn = true, double min = 0, double max = 1) { normalizationEnabled_ = normalizationOn; normMin_ = min, normMax_ = max;}
	/// Must re-implement in subclasses: set the waterfall amount
	virtual void setWaterfallOffset(double offset) { waterfallOffset_ = offset; }
	/// Must re-implement in subclasses: enable or disable the waterfall effect
	virtual void enableWaterfallOffset(bool waterfallOn = true) { waterfallEnabled_ = waterfallOn; }

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
	/// Helper function to look at a plot and configure the left and right axes depending on whether there are 1D and/or 2D data sources in the plot.
	/*! We plot 2D data sources on the right axis scale, and 1D data sources on the left axis scale.  We could therefore end up in one of 4 states for the axes configuration:
0: No data sources of any kind: hide both axis labels; assign both axes to the left axis scale.
1: Only 1D data sources: show the left axis labels, hide the right axis labels; assign both axes to the left axis scale.
2: Only 2D data sources: hide the left axis labels, show the right axis labels; assign both axes to the right axis scale.
3: Both 1D and 2D data sources: show the left and right axis labels; assign the left axis to the left axis scale and the right axis to the right axis scale.
	  */
	void reviewPlotAxesConfiguration(MPlotGW* plot);

	AMScanView* masterView_;

	AMScanSetModel* model() const;

	double waterfallOffset_, normMin_, normMax_;
	bool logScaleEnabled_;
	bool normalizationEnabled_, waterfallEnabled_;

	MPlotGW* createDefaultPlot();
};

#include <QPropertyAnimation>

/// A GUI class that provides a several different ways to view a set of scans.  It is based on the contents of an AMScanSetModel, and a variety of different AMScanViewInternal views can be shown within it.
class AMScanView : public QWidget
{
	Q_OBJECT
public:
	enum ViewMode { Invalid = -1, Tabs = 0, OverPlot, MultiScans, MultiSources };

	/// Constructs a new AMScanView based on a set of scans given by \c model. If \c model is 0,  it creates its own model to use internally; otherwise it uses the supplied model.
	explicit AMScanView(AMScanSetModel* model = 0, QWidget *parent = 0);
	virtual ~AMScanView();

	/// returns the AMScanSetModel used internally to hold the scans/data sources.
	AMScanSetModel* model() const { return scansModel_; }

signals:

public slots:

	/// change the view mode (newMode is a ViewMode enum: 0 for one data source at a time; 1 for all data sources overplotted; 2 for one plot per scan; 3 for one plot per data source.
	void changeViewMode(int newMode);

	/// add a scan to the view:
	void addScan(AMScan* scan);
	/// remove a scan from the view:
	void removeScan(AMScan* scan);

	/// Export the current view to a PDF file with \c outputFileName.
	bool exportGraphicsFile(const QString& outputFileName);

protected slots:
	void resizeViews();

protected:

	AMScanSetModel* scansModel_;

	/// List of the different views available
	QList<AMScanViewInternal*> views_;
	/// current view mode
	ViewMode mode_;

	// ui components:
	AMGraphicsViewAndWidget* gview_;
	QGraphicsLinearLayout* glayout_;

	AMScanViewModeBar* modeBar_;
	AMScanViewSourceSelector* scanBars_;

	QPropertyAnimation* modeAnim_;

	/// internal helper function to build the UI
	void setupUI();
	/// internal helper function to setup all UI event-handling connections
	void makeConnections();
};

/// This class implements an internal view for AMScanView, which shows only a single data source at a time.
class AMScanViewExclusiveView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewExclusiveView(AMScanView* masterView);

	virtual ~AMScanViewExclusiveView();

public slots:

	/// Re-implementing enabling the log scale.
	virtual void enableLogScale(bool logScaleOn);
	virtual void enableNormalization(bool normalizationOn, double min = 0, double max = 1);
	virtual void setWaterfallOffset(double offset);
	virtual void enableWaterfallOffset(bool waterfallOn);

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

	/// Sets the preset data constraints for the given axis scale. \note This method currently only changes the dataRangeConstraint for MPlot::Left.  As other axis scales need calibration, they will be added as well.
	void setDataRangeConstraint(int id);

	/// A list of MPlotItem*... one item for each scan. If we don't have a plot item shown for this scan, this list stores a null pointer.
	/*! A null pointer in plotItems_ means that the scan at that index doesn't have a data source matching the exclusive data source... or there is a data source, but we're unable to display it (ex: unsupported dimensionality, etc.).*/
	QList<MPlotItem*> plotItems_;
	/// This list is a parallel to plotItems_, but it holds a pointer to the data source that we're displaying for this plot.
	/*! A null pointer in plotItemDataSources_ means that the scan at that index doesn't have a data source matching the exclusive data source. */
	QList<AMDataSource*> plotItemDataSources_;

	/// Our plot.
	MPlotGW* plot_;
};

/// This class implements an internal view for AMScanView, which shows all of the enabled data sources.
class AMScanViewMultiView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewMultiView(AMScanView* masterView);

	virtual ~AMScanViewMultiView();

public slots:
	/// Re-implementing enabling the log scale.
	virtual void enableLogScale(bool logScaleOn);
	virtual void enableNormalization(bool normalizationOn, double min = 0, double max = 1);
	virtual void setWaterfallOffset(double offset);
	virtual void enableWaterfallOffset(bool waterfallOn);


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
	/// helper function: adds the scan at \c scanIndex
	void addScan(int scanIndex);

	/// helper function: sets legend and title according to scans in the model
	void refreshTitles();

	/// Sets the preset data constraints for the given axis scale. \note This method currently only changes the dataRangeConstraint for MPlot::Left.  As other axis scales need calibration, they will be added as well.
	void setDataRangeConstraint(int id);

	/// A list of a list of MPlotItem*... In each scan, one for each data source
	QList<QList<MPlotItem*> > plotItems_;
	/// Our plot.
	MPlotGW* plot_;
};

/// This class implements an internal view for AMScanView, which shows every scan in its own plot.
class AMScanViewMultiScansView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewMultiScansView(AMScanView* masterView);

	virtual ~AMScanViewMultiScansView();

public slots:
	/// Re-implementing enabling the log scale.
	virtual void enableLogScale(bool logScaleOn);
	virtual void enableNormalization(bool normalizationOn, double min = 0, double max = 1);
	virtual void setWaterfallOffset(double offset);
	virtual void enableWaterfallOffset(bool waterfallOn);

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
	/// Sets the preset data constraints for the given axis scale. \note This method currently only changes the dataRangeConstraint for MPlot::Left.  As other axis scales need calibration, they will be added as well.
	void setDataRangeConstraint(int id);

	/// helper function: adds the scan at \c scanIndex
	void addScan(int scanIndex);

	/// Helper function: create the legend text on the plot at scan index \c si
	void refreshLegend(int si) {
		plots_.at(si)->plot()->legend()->setBodyText(plotLegendText_.at(si).join(QString()));
	}

	/// re-do the layout of our plots
	void reLayout();

	/// A list of a list of MPlotItem*... In each scan, one for each data source
	QList<QList<MPlotItem*> > plotItems_;
	/// A list of a list of legend strings... In each scan, one for each data source
	QList<QStringList> plotLegendText_;

	/// Our plots
	QList<MPlotGW*> plots_;
	/// A grid-layout within which to put our plots:
	QGraphicsGridLayout* layout_;

	/// true if the first plot in plots_ exists already, but isn't used:
	bool firstPlotEmpty_;
};


/// This class implements an internal view for AMScanView, which shows each data source in its own plot.
class AMScanViewMultiSourcesView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewMultiSourcesView(AMScanView* masterView);

	virtual ~AMScanViewMultiSourcesView();

public slots:
	/// Re-implementing enabling the log scale.
	virtual void enableLogScale(bool logScaleOn);
	virtual void enableNormalization(bool normalizationOn, double min = 0, double max = 1);
	virtual void setWaterfallOffset(double offset);
	virtual void enableWaterfallOffset(bool waterfallOn);

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
	/// Sets the preset data constraints for the given axis scale. \note This method currently only changes the dataRangeConstraint for MPlot::Left.  As other axis scales need calibration, they will be added as well.
	void setDataRangeConstraint(int id);

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


#endif // AMSCANVIEW_H
