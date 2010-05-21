#include "AMScanView.h"
#include <QGraphicsWidget>


AMScanViewScanBar::AMScanViewScanBar(const AMScan* source, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	nameLabel_ = new QLabel(source->name());
	nameLabel_->setStyleSheet("color: white;");
	hl->addWidget(nameLabel_);
	hl->addStretch(0.5);

	chButtonLayout_ = new QHBoxLayout();
	chButtonLayout_->setSpacing(0);
	hl->addLayout(chButtonLayout_);
	if(source) {
		for(int i=0; i<source->numChannels(); i++) {
			QToolButton* cb = new QToolButton();
			cb->setText(source->channel(i)->name());
			cb->setCheckable(true);
			chButtons_.addButton(cb, i);
			chButtonLayout_->addWidget(cb);
		}
	}
	hl->addStretch(1);

	closeButton_ = new QToolButton();
	closeButton_->setText("X");
	hl->addWidget(closeButton_);

	hl->setMargin(6);
	setLayout(hl);

	setObjectName("AMScanViewScanBar");
	setStyleSheet("QWidget#AMScanViewScanBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");
}


AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout(), *hl3 = new QHBoxLayout();
	hl2->setSpacing(0);
	hl3->setSpacing(0);

	QToolButton* tabButton_ = new QToolButton();
	tabButton_->setText("1");
	QToolButton* overplotButton_ = new QToolButton();
	overplotButton_->setText("OP");
	QToolButton* multiScansButton_ = new QToolButton();
	multiScansButton_->setText("M-S");
	QToolButton* multiChannelsButton_ = new QToolButton();
	multiChannelsButton_->setText("M-C");

	tabButton_->setCheckable(true);
	overplotButton_->setCheckable(true);
	multiScansButton_->setCheckable(true);
	multiChannelsButton_->setCheckable(true);

	modeButtons_ = new QButtonGroup(this);
	modeButtons_->addButton(tabButton_, AMScanView::Tabs);
	modeButtons_->addButton(overplotButton_, AMScanView::OverPlot);
	modeButtons_->addButton(multiScansButton_, AMScanView::MultiScans);
	modeButtons_->addButton(multiChannelsButton_, AMScanView::MultiChannels);
	tabButton_->setChecked(true);

	hl2->addWidget(tabButton_);
	hl2->addWidget(overplotButton_);
	hl2->addWidget(multiScansButton_);
	hl2->addWidget(multiChannelsButton_);

	hl->addLayout(hl2);
	hl->addStretch(1);

	plusButton_ = new QToolButton();
	plusButton_->setText("+");
	hl3->addWidget(plusButton_);
	subtractButton_ = new QToolButton();
	subtractButton_->setText("-");
	hl3->addWidget(subtractButton_);

	hl->addLayout(hl3);
	hl->setMargin(6);
	hl->setSpacing(24);
	setLayout(hl);

	setObjectName("AMScanViewModeBar");
	setStyleSheet("QWidget#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");



}

AMScanView::AMScanView(QWidget *parent) :
    QWidget(parent)
{
	mode_ = Invalid;

	setupUI();
	makeConnections();

	changeViewMode(Tabs);
}


void AMScanView::setupUI() {
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setMargin(6);
	vl->setSpacing(6);

	AMScanViewMainWidget* mw = new AMScanViewMainWidget();

	vl->addWidget(mw);

	barLayout_ = new QVBoxLayout();
	barLayout_->setSpacing(0);
	vl->addLayout(barLayout_);

	modeBar_ = new AMScanViewModeBar();
	vl->addWidget(modeBar_);

	setLayout(vl);


	// setup grid layout within main plot area:

	glayout_ = new QGraphicsGridLayout();
	glayout_->setSpacing(12);
	glayout_->setContentsMargins(6,6,6,6);
	mw->graphicsWidget()->setLayout(glayout_);

}


// newMode:
void AMScanView::changeViewMode(int newMode) {

	// nothing changing:
	if(newMode == mode_)
		return;

	// if we're having to change the number of plots, we need to delete and remove all the plot series (otherwise some will be deleted with their plot parents)
	removeAllPlotSeries();

	// starting point for all: all series have been removed from their plots. However, an uncertain number of plots may still exist in the layout.

	switch(newMode) {
	case Tabs:
		{
			// remove plots until there is only one left:
			while(plots_.count() > 1)
				delete plots_.takeLast();

			// add a plot if we don't have any yet.
			if(plots_.count() == 0) {
				plots_ << new MPlotGW();
				plots_[0]->plot()->setScalePadding(5);
				plots_[0]->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(plots_[0], 0, 0, Qt::AlignCenter);
			}

			// for all associated scans: add primary channel
			for(int i=0; i<scans_.count(); i++) {
				AMScanViewEntry& scan = scans_[i];


				AMScanViewChannelEntry& ch = scan.chList[scan.primaryChannel];
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(scan.scan->channel(scan.primaryChannel));
				plots_[0]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = 0;
				// todo: colorize?
			}

			mode_ = Tabs;
		}
		break;

	case OverPlot:
		{
			// remove plots until there is only one left:
			while(plots_.count() > 1)
				delete plots_.takeLast();

			// add a plot if we don't have any yet.
			if(plots_.count() == 0) {
				plots_ << new MPlotGW();
				plots_[0]->plot()->setScalePadding(5);
				plots_[0]->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(plots_[0], 0, 0, Qt::AlignCenter);
			}

			// for all associated scans: add all visible channels:
			for(int j=0; j<scans_.count(); j++) {
				AMScanViewEntry& scan = scans_[j];

				for(int i=0; i<scan.chList.count(); i++) {
					AMScanViewChannelEntry& ch = scan.chList[i];
					if(ch.visible == false)
						continue;
					if(ch.plotSeries == 0) {
						ch.plotSeries = new MPlotSeriesBasic();
					}
					ch.plotSeries->setModel(scan.scan->channel(i));
					plots_[0]->plot()->addItem(ch.plotSeries);
					ch.destinationPlot = 0;
					// todo: colorize?
				}
			}

			mode_ = OverPlot;
		}
		break;

	// multi-scans mode: every scan is on a separate plot (every visible channel is shown). We need as many plots as there are scans.
	case MultiScans:
		{
			// remove all plots:
			while(!plots_.isEmpty() > 0)
				delete plots_.takeLast();

			width_ = sqrt(scans_.count());
			rc_ = 0; cc_ = 0;
			// add a plot if we don't have enough yet
			while(plots_.count() < scans_.count()) {
				MPlotGW* newPlot = new MPlotGW();
				plots_ << newPlot;
				newPlot->plot()->setScalePadding(5);
				newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
				if(cc_ == width_) {
					rc_++;
					cc_ = 0;
				}
			}

			// for all associated scans: add all visible channels:
			for(int j=0; j<scans_.count(); j++) {
				AMScanViewEntry& scan = scans_[j];

				for(int i=0; i<scan.chList.count(); i++) {
					AMScanViewChannelEntry& ch = scan.chList[i];
					if(ch.visible == false)
						continue;
					if(ch.plotSeries == 0) {
						ch.plotSeries = new MPlotSeriesBasic();
					}
					ch.plotSeries->setModel(scan.scan->channel(i));
					plots_[j]->plot()->addItem(ch.plotSeries);
					ch.destinationPlot = j;
					// todo: colorize?
				}
			}

			mode_ = MultiScans;
		}
		break;

	case MultiChannels:
		{

			// how many different channels are there?
			uniqueChannelSearch();

			// remove all plots:
			while(!plots_.isEmpty() > 0)
				delete plots_.takeLast();

			width_ = sqrt(channelNames_.count());
			rc_ = 0; cc_ = 0;
			// add a plot if we don't have enough yet
			while(plots_.count() < channelNames_.count()) {
				MPlotGW* newPlot = new MPlotGW();
				plots_ << newPlot;
				newPlot->plot()->setScalePadding(5);
				newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
				if(cc_ == width_) {
					rc_++;
					cc_ = 0;
				}
			}

			// for all associated scans: add all visible channels:
			for(int j=0; j<scans_.count(); j++) {
				AMScanViewEntry& scan = scans_[j];

				for(int i=0; i<scan.chList.count(); i++) {
					AMScanViewChannelEntry& ch = scan.chList[i];
					if(ch.visible == false)
						continue;
					if(ch.plotSeries == 0) {
						ch.plotSeries = new MPlotSeriesBasic();
					}
					ch.plotSeries->setModel(scan.scan->channel(i));
					int plotIndex = channelNames_.indexOf(scan.scan->channel(i)->name());
					plots_[plotIndex]->plot()->addItem(ch.plotSeries);
					ch.destinationPlot = plotIndex;
					// todo: colorize?
				}
			}

			mode_ = MultiChannels;
		}
		break;
	default:
		break;
	}

	// in case this was called programmatically (instead of by clicking on the button)... the mode button won't be set.  This will re-emit the mode-change signal, but this function will exit immediately on the second time because it's already in the correct mode.
	modeBar_->modeButtons_->button(mode_)->setChecked(true);
}

void AMScanView::makeConnections() {

	// connect mode bar to changeViewMode:
	connect(modeBar_->modeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(changeViewMode(int)));
}

// removes all existing plotSeries from their plots.  This keeps the series alive; does not delete; only removes from parent plot so we can delete the parent plot.
void AMScanView::removeAllPlotSeries() {

	for(int j=0; j<scans_.count(); j++) {
		AMScanViewEntry& scan = scans_[j];

		for(int i=0; i<scan.chList.count(); i++) {
			AMScanViewChannelEntry& ch = scan.chList[i];
			if(ch.plotSeries && ch.destinationPlot >= 0) {
				plots_[ch.destinationPlot]->plot()->removeItem(ch.plotSeries);
				ch.destinationPlot = -1;
			}
		}
	}
}


void AMScanView::uniqueChannelSearch() {

	// unique channels:
	QSet<QString> uchannels;

	for(int j=0; j<scans_.count(); j++)
		for(int i=0; i<scans_[j].scan->numChannels(); i++)
			if(scans_[j].chList[i].visible && !uchannels.contains(scans_[j].scan->channel(i)->name()))
				uchannels.insert(scans_[j].scan->channel(i)->name());

	channelNames_ = uchannels.toList();
}


void AMScanView::addScan(const AMScan* scan) {
	AMScanViewEntry e;

	e.scan = scan;
	e.scanBar = new AMScanViewScanBar(scan);
	barLayout_->addWidget(e.scanBar);
	// todo: hookup connections for clicks on channel buttons.

	for(int i=0; i<scan->numChannels(); i++) {
		AMScanViewChannelEntry ch;
		// todo: defaults for which channels are visible, which aren't.
		// todo: defaults for which is the primary channel.
		e.chList.append(ch);
	}



	// insert handling depends on mode_ from here on:
	switch(mode_) {
	case Tabs:
		{
			AMScanViewChannelEntry& ch = e.chList[e.primaryChannel];
			if(ch.plotSeries == 0) {
				ch.plotSeries = new MPlotSeriesBasic();
			}
			ch.plotSeries->setModel(e.scan->channel(e.primaryChannel));
			plots_[0]->plot()->addItem(ch.plotSeries);
			ch.destinationPlot = 0;

			scans_.append(e);
			e.scanBar->chButtons_.button(e.primaryChannel)->setChecked(true);
		}
		break;

	case OverPlot:
		{
			e.scanBar->chButtons_.setExclusive(false);

			for(int i=0; i<e.chList.count(); i++) {
				AMScanViewChannelEntry& ch = e.chList[i];
				if(ch.visible == false) {
					e.scanBar->chButtons_.button(i)->setChecked(false);
					continue;
				}
				e.scanBar->chButtons_.button(i)->setChecked(true);
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(e.scan->channel(i));
				plots_[0]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = 0;
				// todo: colorize?
			}

			scans_.append(e);
		}
		break;

	case MultiScans:
		{
			e.scanBar->chButtons_.setExclusive(false);

			// going to need one more plot...
			MPlotGW* newPlot = new MPlotGW();
			plots_ << newPlot;
			newPlot->plot()->setScalePadding(5);
			newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
			glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
			if(cc_ == width_) {
				rc_++;
				cc_ = 0;
			}


			for(int i=0; i<e.chList.count(); i++) {
				AMScanViewChannelEntry& ch = e.chList[i];
				if(ch.visible == false) {
					e.scanBar->chButtons_.button(i)->setChecked(false);
					continue;
				}
				e.scanBar->chButtons_.button(i)->setChecked(true);
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(e.scan->channel(i));
				plots_[plots_.count()-1]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = plots_.count()-1;
				// todo: colorize?
			}

			scans_.append(e);

		}
		break;

	case MultiChannels:
		{

			e.scanBar->chButtons_.setExclusive(false);

			// see if this introduces a new channel that we don't have already:
			QSet<QString> newChannels = QSet<QString>::fromList(scan->channelNames());
			for(int i=0; i<e.chList.count(); i++)
				if(!e.chList[i].visible)
					newChannels.remove(scan->channel(i)->name());

			QSet<QString> oldChannels = QSet<QString>::fromList(channelNames_);
			newChannels.subtract(oldChannels);
			channelNames_.append(newChannels.toList());

			foreach(QString newChannel, newChannels) {
				MPlotGW* newPlot = new MPlotGW();
				plots_ << newPlot;
				newPlot->plot()->setScalePadding(5);
				newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
				// todo: problem where we don't recalcualte width. Will keep on adding with current width.
				// fix by switching to flowlayout.
				if(cc_ == width_) {
					rc_++;
					cc_ = 0;
				}
			}

			// add all visible channels:

			for(int i=0; i<e.chList.count(); i++) {
				AMScanViewChannelEntry& ch = e.chList[i];
				if(ch.visible == false) {
					e.scanBar->chButtons_.button(i)->setChecked(false);
					continue;
				}
				e.scanBar->chButtons_.button(i)->setChecked(true);
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(e.scan->channel(i));
				int plotIndex = channelNames_.indexOf(e.scan->channel(i)->name());
				plots_[plotIndex]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = plotIndex;
				// todo: colorize?
			}

			scans_.append(e);
		}
		break;

	default:
		changeViewMode(Tabs);
		break;

	}
}


