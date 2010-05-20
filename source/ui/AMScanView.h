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

#include "MPlot/MPlot.h"
#include "MPlot/MPlotSceneAndView.h"
#include "dataman/AMScan.h"


class AMScanViewScanBar : public QWidget {
public:
	explicit AMScanViewScanBar(const QString& scanName = QString(), QWidget* parent = 0)
		: QWidget(parent)
	{
		QHBoxLayout* hl = new QHBoxLayout();
		nameLabel_ = new QLabel(scanName);
		hl->addWidget(nameLabel_);
		hl->addStretch(0.25);

		chButtonLayout_ = new QHBoxLayout();
		hl->addLayout(chButtonLayout_);
		hl->addStretch(1);

		closeButton_ = new QToolButton();
		closeButton_->setText("X");
		hl->addWidget(closeButton_);

		setLayout(hl);
	}

	// ui components:
	QLabel* nameLabel_;
	QButtonGroup chButtons_;
	QToolButton* closeButton_;
	QHBoxLayout* chButtonLayout_;

};

class AMScanViewModeBar : public QWidget {
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QToolButton* tabButton_, *overplotButton_, *multiScansButton_, *multiChannelsButton_, *plusButton_, *subtractButton_;

};

class AMScanViewEntry {
	// pointer to the scan itself
	AMScan* scan;
	// scan->channelNames() could be useful

	// for the set of channels: which are visible?
	QList<bool> channelVisible;
	// for the list of channels: the MPlotSeriesBasic used to display each
	QList<MPlotSeriesBasic*> plotSeries;
	// for the list of channels: the index of the plot in the view that is currently housing this series
	QList<int> destinationPlot;

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
	enum ViewMode { Tabs, OverPlot, MultiScans, MultiChannels };

    explicit AMScanView(QWidget *parent = 0);

signals:

public slots:

protected:
	// ui components:
	QList<MPlotGW*> plots_;
	QGraphicsGridLayout* glayout_;

	QVBoxLayout* barLayout_;

	AMScanViewModeBar* modeBar_;

	// data components:
	QList<AMScanViewEntry> scans_;
	ViewMode mode_;

	// build UI
	void setupUI();
	// setup all UI event-handling connections
	void makeConnections();


};

#endif // AMSCANVIEW_H
