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

#include "ui/AMCramBarHorizontal.h"

/// \todo move AMScrollArea and AMSizeSignallingWidget into a container widget AMCramBarHorizontal to simplify AMScanViewScanBar immensely.



class AMScanViewChannelSelector;

/// This GUI class is a helper for AMScanViewChannelSelector.  It diplays the available channels for a single Scan.
class AMScanViewScanBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent = 0);

public slots:
	/// The ScanBar has two behaviours.  When exclusiveMode is on, it only allows one channel to be "checked" or selected at a time, and tells the model to make this the exclusive channel.  Otherwise, it allows multiple channels to be checked, and toggles their visibility in the model.
	void setExclusiveModeOn(bool exclusiveModeOn = true);

protected:
	/// ui components:
	QLabel* nameLabel_;
	QButtonGroup chButtons_;
	// REMOVED: QToolButton* closeButton_;
	AMCramBarHorizontal* cramBar_;


	/// Index of "our" scan in the model:
	int scanIndex_;
	/// Connected model:
	AMScanSetModel* model_;
	/// whether in exclusiveMode (ie: only one channel allowed) or not:
	bool exclusiveModeOn_;


protected slots:
	/// after a scan or channel is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes:
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	/// when the model's exclusiveChannel is changed
	void onExclusiveChannelChanged(const QString& exclusiveChannelName);
	/// when one of the channel toggles is clicked:
	void onChannelButtonClicked(int id);

	// when the close (remove) button is clicked
	// REMOVED: void onCloseButtonClicked();




	friend class AMScanViewChannelSelector;
};

/// This GUI class is a view on an AMScanSetModel.  It shows each scan in a horizontal bar, with checkable buttons for each channel.
class AMScanViewChannelSelector : public QWidget {
	Q_OBJECT

public:
	explicit AMScanViewChannelSelector(AMScanSetModel* model = 0, QWidget* parent = 0);
	void setModel(AMScanSetModel* model);

public slots:
	/// ScanBars have two behaviours.  When exclusiveMode is on, they only allow one channel to be "checked" or selected at a time, and tell the model to make this the exclusive channel.  Otherwise, they allows multiple channels within each Scan to be checked, and toggle the channels' visibility in the model.
	void setExclusiveModeOn(bool exclusiveModeOn = true);

protected slots:
	/// after a scan or channel is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);

	/// before a scan or channel is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);

	/// after a scan or channel is deleted in the model:
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

/// Contains the GUI buttons used by AMScanView to switch between different view modes
class AMScanViewModeBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QToolButton* plusButton_, *subtractButton_;
	QButtonGroup* modeButtons_;

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
		delete graphicsWidget_;
		delete scene_;
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

class AMScanView;

/// This class is the interface for different view options inside an AMScanView.  They must be able to handle changes from the AMScanSet model (scans or channels added or removed).
class AMScanViewInternal : public QGraphicsWidget {
	Q_OBJECT
public:
	explicit AMScanViewInternal(AMScanView* masterView);

public slots:

protected slots:
	/// after a scan or channel is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end) = 0;
	/// before a scan or channel is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) = 0;
	/// after a scan or channel is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end) = 0;
	/// when data changes:
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) = 0;

protected:
	AMScanView* masterView_;

	AMScanSetModel* model() const;
};

#include <QPropertyAnimation>

/// A GUI class that provides a several different ways to view a set of scans.  It maintains an internal AMScanSetModel, and a variety of different AMScanViewInternal views can be shown within it.
class AMScanView : public QWidget
{
	Q_OBJECT
public:
	enum ViewMode { Invalid = -1, Tabs = 0, OverPlot, MultiScans, MultiChannels };

	explicit AMScanView(QWidget *parent = 0);
	virtual ~AMScanView();

	/// returns the AMScanSetModel used internally to hold the scans/channels.
	AMScanSetModel* model() const { return scansModel_; }

signals:

public slots:

	/// change the view mode (newMode is a ViewMode enum: 0 for one channel at a time; 1 for channels overplotted; 2 for one plot per scan; 2 for one plot per channel.
	void changeViewMode(int newMode);

	/// add a scan to the view:
	void addScan(AMScan* scan);
	/// remove a scan from the view:
	void removeScan(AMScan* scan);

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
	int width_, rc_, cc_;// layout locations: width (num cols), rowcounter, columncounter.

	AMScanViewModeBar* modeBar_;
	AMScanViewChannelSelector* scanBars_;

	QPropertyAnimation* modeAnim_;

	// build UI
	void setupUI();
	// setup all UI event-handling connections
	void makeConnections();



};




class AMScanViewExclusiveView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewExclusiveView(AMScanView* masterView);

	virtual ~AMScanViewExclusiveView();

public slots:

protected slots:
	/// after a scan or channel is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes:
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

	/// when the model's "exclusive channel" changes. This is the one channel that we display for all of our scans (as long as they have it).
	void onExclusiveChannelChanged(const QString& exclusiveChannel);

protected:
	/// A list of MPlotSeries*... one series for each scan.
	QList<MPlotSeriesBasic*> plotSeries_;
	/// Our plot.
	MPlotGW* plot_;


	/// Helper function to handle adding a scan (at row scanIndex in the model)
	void addScan(int scanIndex);

	/// Helper function to handle review a scan when a channel is added or the exclusive channel changes.
	void reviewScan(int scanIndex);

	/// Helper function to review how many scans are actually displayed (ie: how many have the exclusive channel), and update the plot legend title
	void refreshTitle();

};


class AMScanViewMultiView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewMultiView(AMScanView* masterView);

	virtual ~AMScanViewMultiView();

public slots:


protected slots:
	/// after a scan or channel is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes: (Things we care about: color, linePen, and visible)
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);


protected:
	/// A list of a list of MPlotSeries*... In each scan, one for each channel
	QList<QList<MPlotSeriesBasic*> > plotSeries_;
	/// Our plot.
	MPlotGW* plot_;


	/// helper function: adds the scan at \c scanIndex
	void addScan(int scanIndex);

	/// helper function: sets legend and title according to scans in the model
	void refreshTitles();

};



class AMScanViewMultiScansView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewMultiScansView(AMScanView* masterView);

	virtual ~AMScanViewMultiScansView();

public slots:

protected slots:
	/// after a scan or channel is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes: (Things we care about: color, linePen, and visible)
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);


protected:
	/// A list of a list of MPlotSeries*... In each scan, one for each channel
	QList<QList<MPlotSeriesBasic*> > plotSeries_;
	/// A list of a list of legend strings... In each scan, one for each channel
	QList<QStringList> plotLegendText_;

	/// Our plots
	QList<MPlotGW*> plots_;
	/// A grid-layout within which to put our plots:
	QGraphicsGridLayout* layout_;

	/// true if the first plot in plots_ exists already, but isn't used:
	bool firstPlotEmpty_;

	/// helper function: adds the scan at \c scanIndex
	void addScan(int scanIndex);

	/// Helper function: create the legend text on the plot at scan index \c si
	void refreshLegend(int si) {
		plots_.at(si)->plot()->legend()->setBodyText(plotLegendText_.at(si).join(QString()));
	}

	/// re-do the layout of our plots
	void reLayout();

};


class AMScanViewMultiChannelsView : public AMScanViewInternal {
	Q_OBJECT

public:
	explicit AMScanViewMultiChannelsView(AMScanView* masterView);

	virtual ~AMScanViewMultiChannelsView();

public slots:

protected slots:
	/// after a scan or channel is added in the model
	virtual void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	virtual void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	virtual void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes: (Things we care about: color, linePen, and visible)
	virtual void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);


protected:


	/// Our plots (one for each channel), indexed by channel name
	QMap<QString, MPlotGW*> channel2Plot_;
	/// Our plot series, indexed by channel name and then by Scan pointer
	QMap<QString, QHash<AMScan*, MPlotSeriesBasic*> > channelAndScan2Series_;



	/// A grid-layout within which to put our plots:
	QGraphicsGridLayout* layout_;

	/// true if the first plot in plots_ exists already, but isn't used:
	MPlotGW* firstPlot_;
	bool firstPlotEmpty_;

	/// helper function: reviews the channels that exist/are visible, and ensures plots correspond. Returns true if channel plots were created or deleted (which would require a reLayout()).
	bool reviewChannels();

	/// re-do the layout of our plots
	void reLayout();

};


#endif // AMSCANVIEW_H
