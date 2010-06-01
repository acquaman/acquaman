#ifndef AMSCANVIEW_H
#define AMSCANVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QLayout>
#include <QButtonGroup>
#include <QStringList>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotSceneAndView.h"

#include "dataman/AMScanSetModel.h"

class AMScanViewChannelSelector;

/// This GUI class is a helper for AMScanViewChannelSelector.  It handles a channel-viewer for a single Scan.
class AMScanViewScanBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent = 0);

protected:
	/// ui components:
	QLabel* nameLabel_;
	QButtonGroup chButtons_;
	QToolButton* closeButton_;
	QHBoxLayout* chButtonLayout_;

	/// Index of "our" scan in the model:
	int scanIndex_;
	/// Connected model:
	AMScanSetModel* model_;


protected slots:
	/// after a scan or channel is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes:
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	/// when one of the channel toggles is clicked:
	void onChannelButtonClicked(int id);
	/// when the close (remove) button is clicked
	void onCloseButtonClicked();


	friend class AMScanViewChannelSelector;
};

/// This GUI class is a view on an AMScanSetModel.  It shows each scan in a horizontal bar, with checkable buttons for each channel.
class AMScanViewChannelSelector : public QWidget {
	Q_OBJECT

public:
	explicit AMScanViewChannelSelector(AMScanSetModel* model = 0, QWidget* parent = 0);
	void setModel(AMScanSetModel* model);

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


};

/// Contains the GUI buttons used by AMScanView to switch between different view modes
class AMScanViewModeBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QToolButton* plusButton_, *subtractButton_;
	QButtonGroup* modeButtons_;

};


/// A GUI class that provides a QGraphicsWidget inside the scene, that scales with the size of the scene/view
class AMScanViewMainWidget : public MPlotSceneAndView {
	Q_OBJECT
public:

	AMScanViewMainWidget(QWidget* parent = 0) : MPlotSceneAndView(parent) {
		graphicsWidget_ = new QGraphicsWidget();
		this->scene()->addItem(graphicsWidget_);
		//QPalette p = graphicsWidget_->palette();
		//p.setColor(QPalette::Window, QColor(Qt::red));
		//graphicsWidget_->setPalette(p);
	}


	virtual ~AMScanViewMainWidget() {
		delete graphicsWidget_;
	}

	QGraphicsWidget* graphicsWidget() const { return graphicsWidget_;}

protected:
	QGraphicsWidget* graphicsWidget_;

	// On resize events: notify the graphics widget to resize it.
	virtual void resizeEvent ( QResizeEvent * event ) {
		MPlotSceneAndView::resizeEvent(event);

		graphicsWidget_->resize(event->size());
		fitInView(graphicsWidget_, Qt::KeepAspectRatioByExpanding);
	}
};

class AMScanViewInternal;

/// A GUI class that provides a several different ways to view a set of scans.  It maintains an internal AMScanSetModel, and a variety of different AMScanViewInternal views can be shown within it.
class AMScanView : public QWidget
{
Q_OBJECT
public:
	enum ViewMode { Invalid = -1, Tabs = 0, OverPlot, MultiScans, MultiChannels };

    explicit AMScanView(QWidget *parent = 0);

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

protected:

	AMScanSetModel* scansModel_;

	/// List of the different views available
	QList<AMScanViewInternal*> views_;
	/// current view mode
	ViewMode mode_;

	// ui components:
	QGraphicsGridLayout* glayout_;
	int width_, rc_, cc_;// layout locations: width (num cols), rowcounter, columncounter.



	AMScanViewModeBar* modeBar_;
	AMScanViewChannelSelector* scanBars_;



	// build UI
	void setupUI();
	// setup all UI event-handling connections
	void makeConnections();



};

#endif // AMSCANVIEW_H
