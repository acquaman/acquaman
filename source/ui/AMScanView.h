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
#include "dataman/AMScan.h"


class AMScanViewScanBar : public QWidget {
public:
	explicit AMScanViewScanBar(const AMScan* source = 0, QWidget* parent = 0);

	// ui components:
	QLabel* nameLabel_;
	QButtonGroup chButtons_;
	QToolButton* closeButton_;
	QHBoxLayout* chButtonLayout_;

};

class AMScanViewModeBar : public QWidget {
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QToolButton* plusButton_, *subtractButton_;
	QButtonGroup* modeButtons_;

};

class AMScanViewChannelEntry {
public:
	AMScanViewChannelEntry() { visible = true; plotSeries = 0; destinationPlot = -1; }
	bool visible;
	MPlotSeriesBasic* plotSeries;
	int destinationPlot;
};

class AMScanViewEntry {
public:
	AMScanViewEntry() { scan = 0; scanBar = 0; primaryChannel = 0; }

	// pointer to the scan itself
	const AMScan* scan;
	// scan->channelNames() could be useful

	QList<AMScanViewChannelEntry> chList;

	// A GUI element that exists for each channel, used to toggle which channels are visible
	AMScanViewScanBar* scanBar;

	// in tab mode, where only one channel is plotted: use this one:
	int primaryChannel;
};

class AMScanViewMainWidget : public MPlotSceneAndView {
	Q_OBJECT
public:

	AMScanViewMainWidget(QWidget* parent = 0) : MPlotSceneAndView(parent) {
		graphicsWidget_ = new QGraphicsWidget();
		this->scene()->addItem(graphicsWidget_);
		QPalette p = graphicsWidget_->palette();
		p.setColor(QPalette::Window, QColor(Qt::red));
		graphicsWidget_->setPalette(p);
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

class AMScanView : public QWidget
{
Q_OBJECT
public:
	enum ViewMode { Invalid = -1, Tabs = 0, OverPlot, MultiScans, MultiChannels };

    explicit AMScanView(QWidget *parent = 0);

signals:

public slots:

	/// change the view mode (newMode is a ViewMode enum: 0 for one channel at a time; 1 for channels overplotted; 2 for one plot per scan; 2 for one plot per channel.
	void changeViewMode(int newMode);

	/// add a scan to the view:
	void addScan(const AMScan* scan);

protected:

	/// helper function: remove all plot series:
	void removeAllPlotSeries();
	/// helper function: find out how many unique channels there are, and populate channelNames_;
	void uniqueChannelSearch();

	// ui components:
	QList<MPlotGW*> plots_;
	QGraphicsGridLayout* glayout_;
	int width_, rc_, cc_;// layout locations: width (num cols), rowcounter, columncounter.

	QVBoxLayout* barLayout_;

	AMScanViewModeBar* modeBar_;

	// data components:
	QList<AMScanViewEntry> scans_;
	ViewMode mode_;

	QStringList channelNames_;

	// build UI
	void setupUI();
	// setup all UI event-handling connections
	void makeConnections();



};

#endif // AMSCANVIEW_H
