#include "AMScanView.h"
#include <QGraphicsWidget>



AMScanViewScanBar::AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent)
	: QFrame(parent)
{
	model_ = model;
	scanIndex_ = scanIndex;

	setObjectName("AMScanViewScanBar");
	setStyleSheet("QFrame#AMScanViewScanBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");

	chButtons_.setExclusive(false);

	AMScan* source = model->scanAt(scanIndex_);

	QHBoxLayout* hl = new QHBoxLayout();
	nameLabel_ = new QLabel();
	if(source)
		nameLabel_->setText(source->name());
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
			QColor color = model->data(model->indexForChannel(scanIndex, i), Qt::DecorationRole).value<QColor>();
			cb->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
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
	hl->setSpacing(24);
	setLayout(hl);



	connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

	connect(&chButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onChannelButtonClicked(int)));

	connect(closeButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

}


void AMScanViewScanBar::onRowInserted(const QModelIndex& parent, int start, int end) {
	// not for us...
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}

	// it is for us... (parent index is our Scan, and it is a new channel)
	AMScan* source = model_->scanAt(scanIndex_);
	// note: AMScanSetModel guarantees only one row inserted at a time
	for(int i=start; i<=end; i++) {
		QToolButton* cb = new QToolButton();
		cb->setText(source->channel(i)->name());
		cb->setCheckable(true);
		QColor color = model_->data(model_->indexForChannel(scanIndex_, i), Qt::DecorationRole).value<QColor>();
		cb->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
		chButtons_.addButton(cb, i);
		chButtonLayout_->insertWidget(i, cb);
		cb->setChecked(model_->data(model_->index(i,0,parent), Qt::CheckStateRole).value<bool>());
	}

}

/// before a scan or channel is deleted in the model:
void AMScanViewScanBar::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(end)

	// check if this isn't one of our channels being deleted.
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}

	// need to remove the 'start' channel (AMScanSetModel guarantees only one removed at once)
	delete chButtons_.button(start);
	// the button group's id's from "start+1" to "count+1" are too high now...
	for(int i=start+1; i<chButtons_.buttons().count()+1; i++)
		chButtons_.setId(chButtons_.button(i), i-1);
}

/// after a scan or channel is deleted in the model:
void AMScanViewScanBar::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
}

/// when model data changes.  Possibilities we care about: scan name, and channels visible/not visible.
void AMScanViewScanBar::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	Q_UNUSED(bottomRight)

	// changes to our scan:
	if(topLeft.internalId() == -1 && topLeft.row() == scanIndex_) {

		nameLabel_->setText(model_->scanAt(scanIndex_)->name());
	}

	// changes to one of our channels:
	else if(topLeft.internalId() == scanIndex_) {

		int channelIndex = topLeft.row();
		AMChannel* channel = model_->channelAt(scanIndex_, channelIndex);
		chButtons_.button(channelIndex)->setText(channel->name());
		chButtons_.button(channelIndex)->setChecked(model_->data(topLeft, Qt::CheckStateRole).value<bool>());

		QColor color = model_->data(model_->indexForChannel(scanIndex_, channelIndex), Qt::DecorationRole).value<QColor>();
		chButtons_.button(channelIndex)->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
	}
}

void AMScanViewScanBar::onChannelButtonClicked(int id) {
	Qt::CheckState visible = chButtons_.button(id)->isChecked() ? Qt::Checked : Qt::Unchecked;
	model_->setData(model_->indexForChannel(scanIndex_, id), QVariant(visible), Qt::CheckStateRole);
}

/// after a scan or channel is added in the model
void AMScanViewChannelSelector::onRowInserted(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(end)

	// top-level: inserting a scan:
	if(!parent.isValid()) {
		AMScanViewScanBar* bar = new AMScanViewScanBar(model_, start);
		barLayout_->insertWidget(start, bar);
		scanBars_.insert(start, bar);
	}

	// otherwise, inserting a channel. Handled separately by our AMScanViewScanBar's
}

/// before a scan or channel is deleted in the model:
void AMScanViewChannelSelector::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(end)
	// invalid (top-level) parent: means we're removing a scan
	if(!parent.isValid()){
		delete scanBars_.takeAt(start);
		// all the scans above this one need to move their scan index down:
		for(int i=start; i<scanBars_.count(); i++)
			scanBars_[i]->scanIndex_--;
	}
}


void AMScanViewScanBar::onCloseButtonClicked() {
	model_->removeScan(model_->scanAt(scanIndex_));
}

AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QFrame(parent)
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
	setStyleSheet("QFrame#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");



}

AMScanViewChannelSelector::AMScanViewChannelSelector(AMScanSetModel* model, QWidget* parent)
	: QWidget(parent) {
	model_ = 0;
	setModel(model);

	barLayout_ = new QVBoxLayout();
	barLayout_->setMargin(0);
	barLayout_->setSpacing(0);
	setLayout(barLayout_);
}



void AMScanViewChannelSelector::setModel(AMScanSetModel* model) {
	// remove anything associated with the old model:
	if(model_) {

		while(!scanBars_.isEmpty()) {
			delete scanBars_.takeLast();
		}

		disconnect(model_, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
		disconnect(model_, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
		disconnect(model_, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
		disconnect(model_, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex, QModelIndex)));
	}

	model_ = model;

	// add the new model, if it's valid.
	if(model_) {
		// add existing
		for(int i=0; i<model_->numScans(); i++) {
			AMScanViewScanBar* bar = new AMScanViewScanBar(model_, i);
			barLayout_->addWidget(bar);
			scanBars_ << bar;
		}

		// hookup signals:
		connect(model_, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
		connect(model_, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
		connect(model_, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
		connect(model_, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex, QModelIndex)));

	}
}




AMScanView::AMScanView(QWidget *parent) :
    QWidget(parent)
{
	mode_ = Invalid;
	width_ = 1;
	rc_ = cc_ = 0;

	scansModel_ = new AMScanSetModel(this);

	setupUI();
	makeConnections();

	scanBars_->setModel(scansModel_);

	views_ << new AMScanViewExclusiveView(this);
	views_ << new AMScanViewMultiView(this);

	changeViewMode(Tabs);
}


AMScanView::~AMScanView() {
	for(int i=0; i<views_.count(); i++)
		delete views_.at(i);
}

void AMScanView::setupUI() {
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setMargin(6);
	vl->setSpacing(6);

	AMScanViewMainWidget* mw = new AMScanViewMainWidget();
	mw->setMinimumSize(120,120);

	vl->addWidget(mw);

	scanBars_ = new AMScanViewChannelSelector();
	vl->addWidget(scanBars_);

	modeBar_ = new AMScanViewModeBar();
	vl->addWidget(modeBar_);

	setLayout(vl);


	// setup grid layout within main plot area:

	glayout_ = new QGraphicsGridLayout();
	glayout_->setSpacing(12);
	glayout_->setContentsMargins(6,6,6,6);
	mw->graphicsWidget()->setLayout(glayout_);


	// create views:
	/// \todo !!!

}


// newMode:
void AMScanView::changeViewMode(int newMode) {

	// nothing changing:
	if(newMode == mode_)
		return;

	/*
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
	}*/

	if(newMode < 0 || newMode >= views_.count())
		return;

	//deactivate the old view (if valid)
	if(mode_ != Invalid)
		views_.at(mode_)->deactivate();

	mode_ = (AMScanView::ViewMode)newMode;
	views_.at(mode_)->activate();


	// in case this was called programmatically (instead of by clicking on the button)... the mode button won't be set.  This will re-emit the mode-change signal, but this function will exit immediately on the second time because it's already in the correct mode.
	modeBar_->modeButtons_->button(mode_)->setChecked(true);
}

void AMScanView::makeConnections() {

	// connect mode bar to changeViewMode:
	connect(modeBar_->modeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(changeViewMode(int)));
}
/*

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
}*/

/*
void AMScanView::uniqueChannelSearch() {

	// unique channels:
	QSet<QString> uchannels;

	for(int j=0; j<scans_.count(); j++)
		for(int i=0; i<scans_[j].scan->numChannels(); i++)
			if(scans_[j].chList[i].visible && !uchannels.contains(scans_[j].scan->channel(i)->name()))
				uchannels.insert(scans_[j].scan->channel(i)->name());

	channelNames_ = uchannels.toList();
}*/

/// \todo: should scans held in the view be const or non-const?

void AMScanView::addScan(AMScan *newScan) {
	scansModel_->addScan(newScan);
	// that's it!  handling the rowsAdded, rowsRemoved signals from the model will take care of everything else.
}

/// remove a scan from the view:
void AMScanView::removeScan(AMScan* scan) {
	scansModel_->removeScan(scan);
}

/*
void AMScanView::addScan(const AMScan* scan) {
	AMScanViewEntry e;

	e.scan = scan;
	e.scanBar = new AMChannelSelectorBar(scan);
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
}*/



AMScanViewInternal::AMScanViewInternal(AMScanView* masterView)
	: QObject(masterView),
	masterView_(masterView) {

	connect(model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}


QGraphicsGridLayout* AMScanViewInternal::layout() const { return masterView_->glayout_; }

AMScanSetModel* AMScanViewInternal::model() const { return masterView_->model(); }



AMScanViewExclusiveView::AMScanViewExclusiveView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

	// the list of plotSeries_ needs one element for each scan.
	for(int scanIndex=0; scanIndex < model()->numScans(); scanIndex++) {
		addScan(scanIndex);
	}

	connect(model(), SIGNAL(exclusiveChannelChanged(QString)), this, SLOT(onExclusiveChannelChanged(QString)));
}

AMScanViewExclusiveView::~AMScanViewExclusiveView() {
	for(int i=0; i<plotSeries_.count(); i++)
		if(plotSeries_.at(i)) {
			plot_->plot()->removeItem(plotSeries_.at(i));
			delete plotSeries_.at(i);
		}

	delete plot_;
}



void AMScanViewExclusiveView::onRowInserted(const QModelIndex& parent, int start, int end) {

	// inserting a scan:
	if(!parent.isValid()) {
		for(int i=start; i<=end; i++)
			addScan(i);
	}

	// inserting a channel: (parent.row is the scanIndex, start-end are the channel indices)
	else if(parent.internalId() == -1) {
		reviewScan(parent.row());
	}
}
/// before a scan or channel is deleted in the model:
void AMScanViewExclusiveView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int i=end; i>=start; i--) {
			if(plotSeries_.at(i)) {
				plot_->plot()->removeItem(plotSeries_.at(i));
				delete plotSeries_.at(i);
				plotSeries_.removeAt(i);
			}
		}
	}

	// removing a channel. parent.row() is the scanIndex, and start - end are the channel indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int ci = start; ci<=end; ci++) {
			// if this channel is acting as the data for a plot series, we're about to lose it.  Remove the plot series and set it to 0.
			if(plotSeries_.at(si) && model()->channelAt(si, ci) == plotSeries_.at(si)->model()) {
				plot_->plot()->removeItem(plotSeries_.at(si));
				delete plotSeries_.at(si);
				plotSeries_[si] = 0;
			}
		}
	}
}
/// after a scan or channel is deleted in the model:
void AMScanViewExclusiveView::onRowRemoved(const QModelIndex& parent, int start, int end) {}
/// when data changes:
void AMScanViewExclusiveView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {}

/// when the model's "exclusive channel" changes. This is the one channel that we display for all of our scans (as long as they have it).
void AMScanViewExclusiveView::onExclusiveChannelChanged(const QString& exclusiveChannel) {
	Q_UNUSED(exclusiveChannel)

	for(int i=0; i<model()->numScans(); i++)
		reviewScan(i);
}


/// Helper function to handle adding a scan (at row scanIndex in the model)
void AMScanViewExclusiveView::addScan(int scanIndex) {

	QString channelName = model()->exclusiveChannel();

	AMChannel* channel = model()->scanAt(scanIndex)->channel(channelName);
	int channelIndex = model()->indexOf(channel, model()->scanAt(scanIndex));

	// does this scan have the "exclusive" channel in it?
	if(channel) {
		MPlotSeriesBasic* series = new MPlotSeriesBasic(channel);

		series->setMarker(MPlotMarkerShape::None);
		QPen pen = model()->data(model()->indexForChannel(scanIndex, channelIndex), AMScanSetModel::LinePenRole).value<QPen>();
		pen.setColor(model()->data(model()->indexForChannel(scanIndex, channelIndex), Qt::DecorationRole).value<QColor>());
		series->setLinePen(pen);

		plotSeries_.insert(scanIndex, series);
		plot_->plot()->addItem(series);
	}
	else
		plotSeries_.insert(scanIndex, 0);
}

/// Helper function to handle review a scan when a channel is added or the exclusive channel changes.
void AMScanViewExclusiveView::reviewScan(int scanIndex) {

	QString channelName = model()->exclusiveChannel();

	AMChannel* channel = model()->scanAt(scanIndex)->channel(channelName);
	int channelIndex = model()->indexOf(channel, model()->scanAt(scanIndex));

	// does this scan have the "exclusive" channel in it?
	if(channel) {
		qDebug() << "scan: " << scanIndex << "chanel: " << channelIndex;
		if(plotSeries_.at(scanIndex) == 0) {
			plotSeries_[scanIndex] = new MPlotSeriesBasic();
			plot_->plot()->addItem(plotSeries_.at(scanIndex));
			plotSeries_.at(scanIndex)->setMarker(MPlotMarkerShape::None);
		}

		plotSeries_.at(scanIndex)->setModel(channel);

		QPen pen = model()->data(model()->indexForChannel(scanIndex, channelIndex), AMScanSetModel::LinePenRole).value<QPen>();
		pen.setColor(model()->data(model()->indexForChannel(scanIndex, channelIndex), Qt::DecorationRole).value<QColor>());
		plotSeries_.at(scanIndex)->setLinePen(pen);
	}
	// if it doesn't, but we used to have it:
	else if(plotSeries_.at(scanIndex)) {
		plot_->plot()->removeItem(plotSeries_.at(scanIndex));
		delete plotSeries_.at(scanIndex);
		plotSeries_[scanIndex] = 0;
	}
}


/// add our specific view elements to the AMScanView
void AMScanViewExclusiveView::activate() {
	layout()->addItem(plot_, 0, 0);

}
/// remove our specific view elements from the AMScanView
void AMScanViewExclusiveView::deactivate() {

	// this is unbelievable... No way to remove an item from a QGraphicsLayout without iterating through all items?  Where is QGraphicsLayout::removeItem(QGraphicsLayoutItem*) ?

	plot_->scene()->removeItem(plot_);

	for(int i=0; i<layout()->count(); i++)
		if(layout()->itemAt(i) == plot_)
			layout()->removeAt(i);


}





/////////////////////////////
AMScanViewMultiView::AMScanViewMultiView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

	// the list of plotSeries_ needs one list for each scan,
	for(int si=0; si<model()->numScans(); si++) {
		addScan(si);
	}
}

void AMScanViewMultiView::addScan(int si) {
	QList<MPlotSeriesBasic*> scanList;

	for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++) {

		AMChannel* ch = model()->channelAt(si, ci);
		// if visible, create and append the list
		if(model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>()) {

			MPlotSeriesBasic* series = new MPlotSeriesBasic(ch);

			series->setMarker(MPlotMarkerShape::None);
			QPen pen = model()->data(model()->indexForChannel(si, ci), AMScanSetModel::LinePenRole).value<QPen>();
			pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
			series->setLinePen(pen);

			plot_->plot()->addItem(series);
			scanList << series;
		}
		else // otherwise, append null series
			scanList << 0;
	}
	plotSeries_.insert(si, scanList);
}

AMScanViewMultiView::~AMScanViewMultiView() {

	for(int si=0; si<plotSeries_.count(); si++)
		for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++)
			if(plotSeries_[si][ci]) {
				plot_->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}


	delete plot_;
}



void AMScanViewMultiView::onRowInserted(const QModelIndex& parent, int start, int end) {

	// inserting a scan:
	if(!parent.isValid()) {
		for(int i=start; i<=end; i++)
			addScan(i);
	}

	// inserting a channel: (parent.row is the scanIndex, start-end are the channel indices)
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int ci=start; ci<=end; ci++) {

			AMChannel* ch = model()->channelAt(si, ci);
			// if visible, create and append to the list, and add to plot.
			if(model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>()) {

				MPlotSeriesBasic* series = new MPlotSeriesBasic(ch);

				series->setMarker(MPlotMarkerShape::None);
				QPen pen = model()->data(model()->indexForChannel(si, ci), AMScanSetModel::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);

				plot_->plot()->addItem(series);
				plotSeries_[si].insert(ci, series);
			}
			else // otherwise, append null series
				plotSeries_[si].insert(ci, 0);
		}
	}
}
/// before a scan or channel is deleted in the model:
void AMScanViewMultiView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++) {
				if(plotSeries_[si][ci]) {
					plot_->plot()->removeItem(plotSeries_[si][ci]);
					delete plotSeries_[si][ci];
				}
			}
			plotSeries_.removeAt(si);
		}
	}

	// removing a channel. parent.row() is the scanIndex, and start - end are the channel indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int ci = end; ci>=start; ci--) {
			if(plotSeries_[si][ci]) {
				plot_->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}
			plotSeries_[si].removeAt(ci);
		}
	}
}
/// after a scan or channel is deleted in the model:
void AMScanViewMultiView::onRowRemoved(const QModelIndex& parent, int start, int end) {}

/// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		// no changes that we really care about...

	}

	// changes to one of our channels: internalId is the scanIndex.  Channel index is from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->numScans()) {

		int si = topLeft.internalId();
		for(int ci=topLeft.row(); ci<=bottomRight.row(); ci++) {

			// handle visibility changes:
			bool visible = model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>();

			MPlotSeriesBasic* series = plotSeries_[si][ci];

			// need to create:
			if(visible && series == 0) {
				plotSeries_[si][ci] = series = new MPlotSeriesBasic(model()->channelAt(si, ci));
				series->setMarker(MPlotMarkerShape::None);
				plot_->plot()->addItem(series);
			}

			// need to get rid of:
			if(!visible && series) {
				plot_->plot()->removeItem(series);
				delete series;
				plotSeries_[si][ci] = 0;
			}

			// finally, apply color and linestyle changes, if visible:
			if(visible) {
				QPen pen = model()->data(model()->indexForChannel(si, ci), AMScanSetModel::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);
			}
		}
	}
}




/// add our specific view elements to the AMScanView
void AMScanViewMultiView::activate() {
	layout()->addItem(plot_, 0, 0);

}
/// remove our specific view elements from the AMScanView
void AMScanViewMultiView::deactivate() {

	plot_->scene()->removeItem(plot_);

	for(int i=0; i<layout()->count(); i++)
		if(layout()->itemAt(i) == plot_)
			layout()->removeAt(i);

}
