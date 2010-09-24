#include "AMScanView.h"
#include <QGraphicsWidget>

#include <QScrollBar>

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


	exclusiveModeOn_ = false;
	chButtons_.setExclusive(false);

	AMScan* source = model->scanAt(scanIndex_);


	// UI Setup:
	///////////////////////

	QHBoxLayout* hl = new QHBoxLayout();
	nameLabel_ = new QLabel();
	if(source)
		nameLabel_->setText(source->name() + QString(" #%1").arg(source->number()));
	nameLabel_->setStyleSheet("color: white;");
	hl->addWidget(nameLabel_);
	hl->addStretch(0.5);

	cramBar_ = new AMCramBarHorizontal();


	if(source) {
		for(int i=0; i<source->numChannels(); i++) {
			QToolButton* cb = new QToolButton();
			cb->setText(source->channel(i)->name());
			QColor color = model->data(model->indexForChannel(scanIndex, i), Qt::DecorationRole).value<QColor>();
			cb->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
			cb->setCheckable(true);
			cb->setChecked(model->data(model->indexForChannel(scanIndex, i), Qt::CheckStateRole).value<bool>());
			chButtons_.addButton(cb, i);
			cramBar_->addWidget(cb);
		}
	}

	hl->addWidget(cramBar_);
	hl->addStretch(1);

	/* REMOVED
	closeButton_ = new QToolButton();
	closeButton_->setText("X");
	hl->addWidget(closeButton_);
	*/

	hl->setMargin(6);
	hl->setSpacing(24);
	setLayout(hl);

	//unnecessary: this->setAutoFillBackground(true);
	//unnecessary: ensurePolished();


	connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
	connect(model, SIGNAL(exclusiveChannelChanged(QString)), this, SLOT(onExclusiveChannelChanged(QString)));

	connect(&chButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onChannelButtonClicked(int)));

	// connect(closeButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

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
		cramBar_->getLayout()->insertWidget(i, cb);
		if(exclusiveModeOn_)
			cb->setChecked( (model_->exclusiveChannel() == source->channel(i)->name()) );
		else
			cb->setChecked(model_->data(model_->index(i,0,parent), Qt::CheckStateRole).value<bool>());

		qDebug() << "added a channel. exclusiveModeOn is: " << exclusiveModeOn_ << ", channelName is:" << source->channel(i)->name() << ", exclusiveChannelName is:" << model_->exclusiveChannel();
	}

}

/// before a scan or channel is deleted in the model:  (\todo this one depends on the guarantee that only one row is removed at a time. fix to work with a general model.)
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

		nameLabel_->setText(model_->scanAt(scanIndex_)->name() + QString(" #%1").arg(model_->scanAt(scanIndex_)->number()));
	}

	// changes to one of our channels:
	else if(topLeft.internalId() == scanIndex_) {

		int channelIndex = topLeft.row();
		AMChannel* channel = model_->channelAt(scanIndex_, channelIndex);
		chButtons_.button(channelIndex)->setText(channel->name());
		// setting visibility: depends on whether exclusiveMode is on or not
		if(exclusiveModeOn_)
			chButtons_.button(channelIndex)->setChecked( (model_->exclusiveChannel() == channel->name()) );
		else
			chButtons_.button(channelIndex)->setChecked(model_->data(topLeft, Qt::CheckStateRole).value<bool>());

		QColor color = model_->data(model_->indexForChannel(scanIndex_, channelIndex), Qt::DecorationRole).value<QColor>();
		chButtons_.button(channelIndex)->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
	}
}

void AMScanViewScanBar::onChannelButtonClicked(int id) {
	// if in "exclusive" mode, when clicked, set that channel as the exclusive channel
	if(exclusiveModeOn_) {
		//if(visible == Qt::Checked)
		model_->setExclusiveChannel(model_->channelAt(scanIndex_, id)->name());
		chButtons_.button(id)->setChecked(true);
	}
	else {
		Qt::CheckState visible = chButtons_.button(id)->isChecked() ? Qt::Checked : Qt::Unchecked;
		model_->setData(model_->indexForChannel(scanIndex_, id), QVariant(visible), Qt::CheckStateRole);
	}
}


void AMScanViewScanBar::onExclusiveChannelChanged(const QString &exclusiveChannelName) {
	// when exclusive mode is on, set checked only when channel(button) name matches
	if(exclusiveModeOn_) {
		int numChannelButtons = chButtons_.buttons().count();
		for(int ci=0; ci<numChannelButtons; ci++)
			chButtons_.button(ci)->setChecked( (model_->channelAt(scanIndex_, ci)->name() == exclusiveChannelName) );
	}
}

void AMScanViewScanBar::setExclusiveModeOn(bool exclusiveModeOn) {

	// turning exclusiveMode on:
	if(exclusiveModeOn && !exclusiveModeOn_) {
		exclusiveModeOn_ = true;
		int numChannelButtons = chButtons_.buttons().count();
		for(int ci=0; ci<numChannelButtons; ci++) {
			chButtons_.button(ci)->setChecked( (model_->channelAt(scanIndex_, ci)->name() == model_->exclusiveChannel()) );
		}
		//chButtons_.setExclusive(true);
	}

	// turning exclusiveMode off:
	if(!exclusiveModeOn && exclusiveModeOn_) {
		exclusiveModeOn_ = false;
		//chButtons_.setExclusive(false);
		int numChannelButtons = chButtons_.buttons().count();
		for(int ci=0; ci<numChannelButtons; ci++) {
			chButtons_.button(ci)->setChecked( model_->data(model_->indexForChannel(scanIndex_, ci), Qt::CheckStateRole).value<bool>() );
		}
	}
}

/*
void AMScanViewScanBar::onCloseButtonClicked() {
	model_->removeScan(model_->scanAt(scanIndex_));
}
*/











AMScanViewChannelSelector::AMScanViewChannelSelector(AMScanSetModel* model, QWidget* parent)
	: QWidget(parent) {
	model_ = 0;
	setModel(model);

	barLayout_ = new QVBoxLayout();
	barLayout_->setMargin(0);
	barLayout_->setSpacing(0);
	setLayout(barLayout_);

	exclusiveModeOn_ = false;
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

/// after a scan or channel is added in the model
void AMScanViewChannelSelector::onRowInserted(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(end)

	// top-level: inserting a scan:
	if(!parent.isValid()) {
		AMScanViewScanBar* bar = new AMScanViewScanBar(model_, start);
		bar->setExclusiveModeOn(exclusiveModeOn_);
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

/// ScanBars have two behaviours.  When exclusiveMode is on, they only allow one channel to be "checked" or selected at a time, and tell the model to make this the exclusive channel.  Otherwise, they allows multiple channels within each Scan to be checked, and toggle the channels' visibility in the model.
void AMScanViewChannelSelector::setExclusiveModeOn(bool exclusiveModeOn) {
	exclusiveModeOn_ = exclusiveModeOn;
	foreach(AMScanViewScanBar* bar, scanBars_) {
		bar->setExclusiveModeOn(exclusiveModeOn);
	}
}














AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QFrame(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout(), *hl3 = new QHBoxLayout();
	hl2->setSpacing(0);
	hl3->setSpacing(0);

	QToolButton* tabButton_ = new QToolButton();
	tabButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	tabButton_->setText("1");
	QToolButton* overplotButton_ = new QToolButton();
	overplotButton_->setText("OP");
	overplotButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	QToolButton* multiScansButton_ = new QToolButton();
	multiScansButton_->setText("M-S");
	multiScansButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	QToolButton* multiChannelsButton_ = new QToolButton();
	multiChannelsButton_->setText("M-C");
	multiChannelsButton_->setAttribute(Qt::WA_MacBrushedMetal, true);

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
	plusButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	hl3->addWidget(plusButton_);
	subtractButton_ = new QToolButton();
	subtractButton_->setText("-");
	subtractButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	hl3->addWidget(subtractButton_);

	hl->addLayout(hl3);
	hl->setMargin(6);
	hl->setSpacing(24);
	setLayout(hl);

	setObjectName("AMScanViewModeBar");
	/*setStyleSheet("QFrame#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");*/
}






AMScanView::AMScanView(QWidget *parent) :
	QWidget(parent)
{
	mode_ = Invalid;

	scansModel_ = new AMScanSetModel(this);

	setupUI();
	makeConnections();

	scanBars_->setModel(scansModel_);

	modeAnim_ = new QPropertyAnimation(gview_->graphicsWidget(), "geometry", this);
	modeAnim_->setDuration(500);
	modeAnim_->setEasingCurve(QEasingCurve::InOutQuad);

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

	gview_ = new AMGraphicsViewAndWidget();
	gview_->setMinimumSize(400,300);
	gview_->graphicsWidget()->setGeometry(0,0,400*4, 300);

	vl->addWidget(gview_);

	scanBars_ = new AMScanViewChannelSelector();
	vl->addWidget(scanBars_);

	modeBar_ = new AMScanViewModeBar();
	vl->addWidget(modeBar_);

	setLayout(vl);

	// setup linear layout within main graphics area:
	glayout_ = new QGraphicsLinearLayout();
	glayout_->setSpacing(0);
	glayout_->setContentsMargins(0,0,0,0);
	gview_->graphicsWidget()->setLayout(glayout_);

	views_ << new AMScanViewExclusiveView(this);
	views_ << new AMScanViewMultiView(this);
	views_ << new AMScanViewMultiScansView(this);
	views_ << new AMScanViewMultiChannelsView(this);

	for(int i=0; i<views_.count(); i++)
		glayout_->addItem(views_.at(i));

}


// newMode:
void AMScanView::changeViewMode(int newMode) {

	// nothing changing:
	if(newMode == mode_)
		return;

	if(newMode < 0 || newMode >= views_.count())
		return;

	mode_ = (ViewMode)newMode;
	scanBars_->setExclusiveModeOn( (mode_ == Tabs) );

	resizeViews();

	// in case this was called programmatically (instead of by clicking on the button)... the mode button won't be set.  This will re-emit the mode-change signal, but this function will exit immediately on the second time because it's already in the correct mode.
	modeBar_->modeButtons_->button(mode_)->setChecked(true);
}

void AMScanView::makeConnections() {

	// connect mode bar to changeViewMode:
	connect(modeBar_->modeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(changeViewMode(int)));

	// connect resize event from graphicsView to resize the stuff inside the view
	connect(gview_, SIGNAL(resized(QSizeF)), this, SLOT(resizeViews()), Qt::QueuedConnection);
}

void AMScanView::resizeViews() {

	QSize viewSize = gview_->size();
	QSizeF mainWidgetSize = QSizeF(viewSize.width()*views_.count(), viewSize.height());

	gview_->setSceneRect(QRectF(QPointF(0,0), viewSize ));

	QPointF pos;
	if(mode_ == Invalid)
		pos = QPointF(0,0);
	else
		pos = QPointF(-viewSize.width()*mode_, 0);

	//gview_->graphicsWidget()->setGeometry(QRectF(pos, mainWidgetSize));


	modeAnim_->stop();

	modeAnim_->setStartValue(gview_->graphicsWidget()->geometry());
	modeAnim_->setEndValue(QRectF(pos, mainWidgetSize));

	modeAnim_->start();
}

/// \todo: should scans held in the view be const or non-const?

void AMScanView::addScan(AMScan *newScan) {
	scansModel_->addScan(newScan);
	// that's it!  handling the rowsAdded, rowsRemoved signals from the model will take care of everything else.
}

/// remove a scan from the view:
void AMScanView::removeScan(AMScan* scan) {
	scansModel_->removeScan(scan);
}

#include <QSizePolicy>

AMScanViewInternal::AMScanViewInternal(AMScanView* masterView)
	: QGraphicsWidget(),
	masterView_(masterView) {

	connect(model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

	QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Preferred);
	sp.setHorizontalStretch(1);
	setSizePolicy(sp);
	// note that the _widget_'s size policy will be meaningless after a top-level layout is set. (the sizePolicy() of the layout is used instead.)  Therefore, subclasses with top-level layouts need to copy this sizePolicy() to their layout before setting it.

}


AMScanSetModel* AMScanViewInternal::model() const { return masterView_->model(); }



AMScanViewExclusiveView::AMScanViewExclusiveView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);
	plot_->plot()->axisBottom()->showAxisName(false);
	plot_->plot()->axisLeft()->showAxisName(false);
	plot_->plot()->enableAxisNormalizationLeft(true);
	plot_->plot()->setWaterfallLeft();

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	gl->setSizePolicy(sizePolicy());
	setLayout(gl);


	// the list of plotSeries_ needs one element for each scan.
	for(int scanIndex=0; scanIndex < model()->numScans(); scanIndex++) {
		addScan(scanIndex);
	}

	connect(model(), SIGNAL(exclusiveChannelChanged(QString)), this, SLOT(onExclusiveChannelChanged(QString)));

	refreshTitle();
}

AMScanViewExclusiveView::~AMScanViewExclusiveView() {
	// this isn't really necessary... PlotSeries's will be deleted by as children items of the plot.
	/*
	for(int i=0; i<plotSeries_.count(); i++) {
		if(plotSeries_.at(i)) {
			plot_->plot()->removeItem(plotSeries_.at(i));
			delete plotSeries_.at(i);
		}
	}*/

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

	refreshTitle();
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
void AMScanViewExclusiveView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	refreshTitle();

}
/// when data changes:
void AMScanViewExclusiveView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
	Q_UNUSED(topLeft)
	Q_UNUSED(bottomRight)

	// the one situation we need to worry about here: if the color of a channel changes, and it's the exclusive (currently-displayed) channel, we'll need to re-color the legend

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		// no changes that we really care about...
	}

	// changes to one of our channels: internalId is the scanIndex.  Channel index is from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->numScans()) {

		int si = topLeft.internalId();	// si: scan index
		for(int ci=topLeft.row(); ci<=bottomRight.row(); ci++) {	// ci: channel index

			// if this channel is the exclusive channel:
			if(model()->channelAt(si, ci)->name() == model()->exclusiveChannel()) {
				QColor color = model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>();
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				pen.setColor(color);
				plotSeries_.at(si)->setLinePen(pen);
			}
		}
	}

}

/// when the model's "exclusive channel" changes. This is the one channel that we display for all of our scans (as long as they have it).
void AMScanViewExclusiveView::onExclusiveChannelChanged(const QString& exclusiveChannel) {
	Q_UNUSED(exclusiveChannel)

	for(int i=0; i<model()->numScans(); i++) {
		reviewScan(i);
	}

	refreshTitle();
}

void AMScanViewExclusiveView::refreshTitle() {

	int numScansShown = 0;
	foreach(MPlotSeriesBasic* s, plotSeries_)
		if(s)
			numScansShown++;

	QString scanCount = (numScansShown == 1) ? " (1 scan)" : QString(" (%1 scans)").arg(numScansShown);
	plot_->plot()->legend()->setTitleText(model()->exclusiveChannel() + scanCount);	// result ex: "tey (3 scans)"
}

/// Helper function to handle adding a scan (at row scanIndex in the model)
void AMScanViewExclusiveView::addScan(int scanIndex) {

	QString channelName = model()->exclusiveChannel();

	int channelIndex = model()->scanAt(scanIndex)->indexOfChannel(channelName);
	AMChannel* channel = model()->channelAt(scanIndex, channelIndex);

	// does this scan have the "exclusive" channel in it?
	if(channel) {
		MPlotSeriesBasic* series = new MPlotSeriesBasic(channel);

		series->setMarker(MPlotMarkerShape::None);
		QPen pen = model()->data(model()->indexForChannel(scanIndex, channelIndex), AM::LinePenRole).value<QPen>();
		QColor color = model()->data(model()->indexForChannel(scanIndex, channelIndex), Qt::DecorationRole).value<QColor>();
		pen.setColor(color);
		series->setLinePen(pen);
		series->setDescription(model()->scanAt(scanIndex)->fullName());

		plotSeries_.insert(scanIndex, series);
		plot_->plot()->addItem(series);

	}
	else {
		plotSeries_.insert(scanIndex, 0);
	}
}

/// Helper function to handle review a scan when a channel is added or the exclusive channel changes.
void AMScanViewExclusiveView::reviewScan(int scanIndex) {

	QString channelName = model()->exclusiveChannel();

	int channelIndex = model()->scanAt(scanIndex)->indexOfChannel(channelName);
	AMChannel* channel = model()->channelAt(scanIndex, channelIndex);

	// does this scan have the "exclusive" channel in it?
	if(channel) {

		if(plotSeries_.at(scanIndex) == 0) {
			plotSeries_[scanIndex] = new MPlotSeriesBasic();
			plot_->plot()->addItem(plotSeries_.at(scanIndex));
			plotSeries_.at(scanIndex)->setMarker(MPlotMarkerShape::None);
		}

		plotSeries_.at(scanIndex)->setModel(channel);

		QPen pen = model()->data(model()->indexForChannel(scanIndex, channelIndex), AM::LinePenRole).value<QPen>();
		QColor color = model()->data(model()->indexForChannel(scanIndex, channelIndex), Qt::DecorationRole).value<QColor>();
		pen.setColor(color);
		plotSeries_.at(scanIndex)->setLinePen(pen);
		plotSeries_.at(scanIndex)->setDescription(model()->scanAt(scanIndex)->fullName());

	}
	// if it doesn't, but we used to have it:
	else if(plotSeries_.at(scanIndex)) {
		plot_->plot()->removeItem(plotSeries_.at(scanIndex));
		delete plotSeries_.at(scanIndex);
		plotSeries_[scanIndex] = 0;
	}
}





/////////////////////////////
AMScanViewMultiView::AMScanViewMultiView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);
	plot_->plot()->axisBottom()->showAxisName(false);
	plot_->plot()->axisLeft()->showAxisName(false);
	plot_->plot()->legend()->enableDefaultLegend(false);	// turn on or turn off labels for individual scans in this plot
	plot_->plot()->enableAxisNormalizationLeft(true);
	plot_->plot()->setWaterfallLeft();

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	gl->setSizePolicy(sizePolicy());
	setLayout(gl);

	// the list of plotSeries_ needs one list for each scan,
	for(int si=0; si<model()->numScans(); si++) {
		addScan(si);
	}

	refreshTitles();
}

void AMScanViewMultiView::addScan(int si) {
	QList<MPlotSeriesBasic*> scanList;

	for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++) {

		AMChannel* ch = model()->channelAt(si, ci);
		// if visible, create and append the list
		if(model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>()) {

			MPlotSeriesBasic* series = new MPlotSeriesBasic(ch);

			series->setMarker(MPlotMarkerShape::None);
			QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
			pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
			series->setLinePen(pen);
			series->setDescription(model()->scanAt(si)->fullName() + ": " + ch->name());

			plot_->plot()->addItem(series);
			scanList << series;
		}
		else // otherwise, append null series
			scanList << 0;
	}
	plotSeries_.insert(si, scanList);
}

AMScanViewMultiView::~AMScanViewMultiView() {

	/* Not necessary; deleting the plot should delete its children.
	for(int si=0; si<plotSeries_.count(); si++)
		for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++)
			if(plotSeries_[si][ci]) {
				plot_->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}*/

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
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);
				series->setDescription(model()->scanAt(si)->fullName() + ": " + ch->name());

				plot_->plot()->addItem(series);
				plotSeries_[si].insert(ci, series);
			}
			else // otherwise, append null series
				plotSeries_[si].insert(ci, 0);
		}
	}

	refreshTitles();
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
void AMScanViewMultiView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	refreshTitles();
}

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
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);
				series->setDescription(model()->scanAt(si)->fullName() + ": " + model()->channelAt(si, ci)->name());
			}
		}
	}

	refreshTitles();
}



void AMScanViewMultiView::refreshTitles() {

	if(model()->numScans() == 1)
		plot_->plot()->legend()->setTitleText("1 scan");
	else
		plot_->plot()->legend()->setTitleText(QString("%1 scans").arg(model()->numScans()));

	plot_->plot()->legend()->setBodyText("(" + model()->scanNames().join(", ") + ")");
}


///////////////////////////////////////////////////

AMScanViewMultiScansView::AMScanViewMultiScansView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	layout_ = new QGraphicsGridLayout();
	layout_->setContentsMargins(0,0,0,0);
	layout_->setSpacing(0);
	layout_->setSizePolicy(sizePolicy());
	setLayout(layout_);

	// we need to have at least one plot, to fill our widget,  even if there are no scans.
	MPlotGW* plot;
	plot = new MPlotGW();
	plot->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot->plot()->axisRight()->setTicks(0);
	plot->plot()->axisBottom()->setTicks(4);
	plot->plot()->axisLeft()->showGrid(false);
	plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);
	plot->plot()->axisBottom()->showAxisName(false);
	plot->plot()->axisLeft()->showAxisName(false);
	plot->plot()->legend()->enableDefaultLegend(false);	/// \todo Right now we maintain our own legend (instead of using MPlotLegend's automatic one), to keep it sorted by channel order. If you could introduce consistent ordering to MPlotLegend and MPlot::items(), we wouldn't have to.
	plot->plot()->enableAxisNormalizationLeft(true);

	firstPlotEmpty_ = true;
	plots_ << plot;

	// add all of the scans we have already
	for(int si=0; si<model()->numScans(); si++) {
		addScan(si);
	}

	reLayout();
}

void AMScanViewMultiScansView::addScan(int si) {

	// create plot (unless we're inserting Scan 0 and an empty plot is ready for us)
	if(si == 0 && firstPlotEmpty_) {
		firstPlotEmpty_ = false;
	}
	else {
		MPlotGW* plot;
		plot = new MPlotGW();
		plot->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
		plot->plot()->axisRight()->setTicks(0);
		plot->plot()->axisBottom()->setTicks(4);
		plot->plot()->axisLeft()->showGrid(false);
		plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);
		plot->plot()->axisBottom()->showAxisName(false);
		plot->plot()->axisLeft()->showAxisName(false);
		plot->plot()->legend()->enableDefaultLegend(false);
		plot->plot()->enableAxisNormalizationLeft(true);

		plots_.insert(si, plot);
	}

	QList<MPlotSeriesBasic*> scanList;
	QStringList channelLegendText;

	for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++) {

		AMChannel* ch = model()->channelAt(si, ci);
		// if visible, create and append the list
		if(model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>()) {

			MPlotSeriesBasic* series = new MPlotSeriesBasic(ch);

			series->setMarker(MPlotMarkerShape::None);
			QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
			QColor color = model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>();
			pen.setColor(color);
			series->setLinePen(pen);
			series->setDescription(ch->name());

			plots_.at(si)->plot()->addItem(series);
			scanList << series;

			channelLegendText << QString("<font color=#%1%2%3>%4</font><br>")
					.arg(color.red(), 2, 16, QChar('0'))
					.arg(color.green(), 2, 16, QChar('0'))
					.arg(color.blue(), 2, 16, QChar('0'))
					.arg(ch->name());	/// \todo use channel description instead of name

		}
		else { // otherwise, append null series
			scanList << 0;
			channelLegendText << QString();
		}
	}
	plotSeries_.insert(si, scanList);
	plotLegendText_.insert(si, channelLegendText);

	// set plot title and legend
	plots_.at(si)->plot()->legend()->setTitleText(model()->scanAt(si)->fullName());
	refreshLegend(si);

	// note: haven't yet added this new plot to the layout_.  That's up to reLayout();
}

AMScanViewMultiScansView::~AMScanViewMultiScansView() {

	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).
	for(int si=0; si<plotSeries_.count(); si++)
		for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++)
			if(plotSeries_[si][ci]) {
				plots_[si]->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}*/


	for(int pi=0; pi<plots_.count(); pi++)
		delete plots_.at(pi);
}



void AMScanViewMultiScansView::onRowInserted(const QModelIndex& parent, int start, int end) {

	// inserting a scan:
	if(!parent.isValid()) {
		for(int i=start; i<=end; i++)
			addScan(i);

		reLayout();
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
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				QColor color = model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>();
				pen.setColor(color);
				series->setLinePen(pen);
				series->setDescription(ch->name());

				plots_[si]->plot()->addItem(series);
				plotSeries_[si].insert(ci, series);

				QString legendText = QString("<font color=#%1%2%3>%4</font><br>")
									.arg(color.red(), 2, 16, QChar('0'))
									.arg(color.green(), 2, 16, QChar('0'))
									.arg(color.blue(), 2, 16, QChar('0'))
									.arg(ch->name());	/// \todo use channel description instead of name
				plotLegendText_[si].insert(ci, legendText);
			}
			else { // otherwise, append null series
				plotSeries_[si].insert(ci, 0);
				plotLegendText_[si].insert(ci, QString());
			}
		}
		// update the legend text
		refreshLegend(si);
	}
}
/// before a scan or channel is deleted in the model:
void AMScanViewMultiScansView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++) {
				if(plotSeries_[si][ci]) {
					plots_[si]->plot()->removeItem(plotSeries_[si][ci]);
					delete plotSeries_[si][ci];
				}
			}
			plotSeries_.removeAt(si);
			// if this is the first plot, and its the only one left, need to leave it around but flagged as empty
			if(si == 0 && plots_.count()==1) {
				firstPlotEmpty_ = true;
				plots_.at(si)->plot()->legend()->setTitleText("");
			}
			else {
				delete plots_.takeAt(si);
			}
		}
		reLayout();
	}

	// removing a channel. parent.row() is the scanIndex, and start - end are the channel indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int ci = end; ci>=start; ci--) {
			if(plotSeries_[si][ci]) {
				plots_[si]->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}
			plotSeries_[si].removeAt(ci);
			plotLegendText_[si].removeAt(ci);
		}
		refreshLegend(si);
	}
}
/// after a scan or channel is deleted in the model:
void AMScanViewMultiScansView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
}

/// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiScansView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

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

			// need to create? (becoming visible)
			if(visible && series == 0) {
				plotSeries_[si][ci] = series = new MPlotSeriesBasic(model()->channelAt(si, ci));
				series->setMarker(MPlotMarkerShape::None);
				plots_[si]->plot()->addItem(series);
			}

			// need to get rid of? (becoming invisible)
			if(!visible && series) {
				plots_[si]->plot()->removeItem(series);
				delete series;
				plotSeries_[si][ci] = 0;
			}

			// finally, apply color and linestyle changes, if visible:
			if(visible) {
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				QColor color = model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>();
				pen.setColor(color);
				series->setLinePen(pen);
				series->setDescription(model()->channelAt(si, ci)->name());

				plotLegendText_[si][ci] = QString("<font color=#%1%2%3>%4</font><br>")
									.arg(color.red(), 2, 16, QChar('0'))
									.arg(color.green(), 2, 16, QChar('0'))
									.arg(color.blue(), 2, 16, QChar('0'))
									.arg(model()->channelAt(si, ci)->name());	/// \todo use channel description instead of name
			}
			// if invisible, remove from legend
			else {
				plotLegendText_[si][ci] = QString();
			}
		}
		refreshLegend(si);
	}
}




/// re-do the layout
void AMScanViewMultiScansView::reLayout() {

	for(int li=0; li<layout_->count(); li++)
		layout_->removeAt(li);

	int rc=0, cc=0, width = sqrt(plots_.count());

	for(int pi=0; pi<plots_.count(); pi++) {
		layout_->addItem(plots_.at(pi), rc, cc++, Qt::AlignCenter);
		if(cc == width) {
			rc++;
			cc = 0;
		}
	}
}





//////////////////////////////////////////////////

AMScanViewMultiChannelsView::AMScanViewMultiChannelsView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	layout_ = new QGraphicsGridLayout();
	layout_->setContentsMargins(0,0,0,0);
	layout_->setSpacing(0);
	layout_->setSizePolicy(sizePolicy());
	setLayout(layout_);

	// we need to have at least one plot, to fill our widget,  even if there are no scans.
	firstPlot_ = new MPlotGW();
	firstPlot_->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	firstPlot_->plot()->axisRight()->setTicks(0);
	firstPlot_->plot()->axisBottom()->setTicks(4);
	firstPlot_->plot()->axisLeft()->showGrid(false);
	firstPlot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);
	firstPlot_->plot()->axisBottom()->showAxisName(false);
	firstPlot_->plot()->axisLeft()->showAxisName(false);
	firstPlot_->plot()->enableAxisNormalizationLeft(true);


	firstPlotEmpty_ = true;

	// add all of the scans/channels we have already
	reviewChannels();

	reLayout();
}



AMScanViewMultiChannelsView::~AMScanViewMultiChannelsView() {

	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).*/

	foreach(MPlotGW* plot, channel2Plot_)
		delete plot;
}



void AMScanViewMultiChannelsView::onRowInserted(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(reviewChannels())
		reLayout();
}


/// before a scan or channel is deleted in the model:
void AMScanViewMultiChannelsView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan(s):    (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			AMScan* scan = model()->scanAt(si);

			// go through all channels that this scan has; check if they exist; remove and delete plot series if they do.
			for(int ci=0; ci<scan->numChannels(); ci++) {
				QString channelName = scan->channel(ci)->name();
				if(channel2Plot_.contains(channelName) && channelAndScan2Series_[channelName].contains(scan)) {
					channel2Plot_[channelName]->plot()->removeItem(channelAndScan2Series_[channelName][scan]);
					delete channelAndScan2Series_[channelName][scan];
					channelAndScan2Series_[channelName].remove(scan);
					// note: checking whether a plot for this channel should still exist (in channel2Plot_) will be done after the deletion is finished, inside reviewChannels() called by onRowRemoved().
				}
			}

		}
	}


	// removing a channel(s).     parent.row() is the scanIndex, and start - end are the channel indexes.
	else if(parent.internalId() == -1) {

		int si = parent.row();
		AMScan* scan = model()->scanAt(si);

		for(int ci = end; ci>=start; ci--) {
			QString channelName = model()->channelAt(si, ci)->name();
			if(channel2Plot_.contains(channelName) && channelAndScan2Series_[channelName].contains(scan)) {
				channel2Plot_[channelName]->plot()->removeItem(channelAndScan2Series_[channelName][scan]);
				delete channelAndScan2Series_[channelName][scan];
				channelAndScan2Series_[channelName].remove(scan);
				// note: checking whether a plot for this channel should still exist (in channel2Plot_) will be done after the deletion is finished, inside reviewChannels() called by onRowRemoved().
			}
		}
	}
}



/// after a scan or channel is deleted in the model:
void AMScanViewMultiChannelsView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(reviewChannels())
		reLayout();
}

/// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiChannelsView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		/// \todo zzzzz changes to name and number?


	}

	// changes to one of our channels: internalId is the scanIndex.  Channel index is from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->numScans()) {

		// handling visibility changes: if a channel has been turned off (or on), and it was the only channel of its kind, we'll need to delete/add a plot.
		// this is expensive... Would be nice to know _what_ changed (since simply changing the color of a line shouldn't require this whole process)
		if(reviewChannels())
			reLayout();

		// apply possible line pen and color changes:
		int si = topLeft.internalId();
		AMScan* scan = model()->scanAt(si);

		for(int ci=topLeft.row(); ci<=bottomRight.row(); ci++) {

			QString channelName = model()->scanAt(si)->channel(ci)->name();

			if(channel2Plot_.contains(channelName) && channelAndScan2Series_[channelName].contains(scan)) {
				MPlotSeriesBasic* series = channelAndScan2Series_[channelName][scan];
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);
			}
		}
	}
}




/// re-do the layout
void AMScanViewMultiChannelsView::reLayout() {

	for(int li=0; li<layout_->count(); li++)
		layout_->removeAt(li);

	int rc=0, cc=0, width = sqrt(channel2Plot_.count());

	foreach(MPlotGW* plot, channel2Plot_) {
		layout_->addItem(plot, rc, cc++, Qt::AlignCenter);
		if(cc == width) {
			rc++;
			cc = 0;
		}
	}

	if(channel2Plot_.isEmpty())
		layout_->addItem(firstPlot_, 0, 0, Qt::AlignCenter);
}


/// The following comment is old and no longer correct.  We no longer care about maintaining the same ordering as model()->visibleChannelNames(). We now maintain a QMap of plots indexed by channelName, instead of a list of plots ordered the same as visibleChannelNames().

/// when full list of channels in the model changes, we need to sync/match up our channel lists (channelNames_ and plots_) with the model list.
/*! Here's what changes could have happened in the model list:
	- channels added (one or multiple)
	- channels removed (one or multiple)
	- order changed

	Assumptions:
	- unique channel names (no duplicates)

	Required outcome:
	- final order the same in both lists
	- keep entities whenever we already have them

	Here's our sync algorithm:

	- delete all in [us] not contained in [model]
	// [us] is now the same or smaller than [model]
	- iterate through [model] from i=0 onwards
		- if( i >= us.count )
			- append to us
		- else if ( model[i] != us[i] )
			- if ( us.contains(model[i]) )
				- move it to i
			- else
				- us.insert at i

*/
bool AMScanViewMultiChannelsView::reviewChannels() {

	QSet<QString> modelChannels = model()->visibleChannelNames().toSet();
	QSet<QString> ourChannels = channel2Plot_.keys().toSet();
	QSet<QString> deleteChannels(ourChannels);
	deleteChannels -= modelChannels;

	QSet<QString> addChannels(modelChannels);
	addChannels -= ourChannels;

	// now, deleteChannels is a set of the channel names we have as plots (but shouldn't have)
	// and, addChannels is a set of the channel names we don't have plots for (but should have)

	// if there's anything in deleteChannels or in addChannels, we'll need to re-layout the plots
	bool areChanges = ( deleteChannels.count() || addChannels.count() );

	// delete the channel plots that don't belong:
	foreach(QString channelName, deleteChannels) {
		// delete the plot (will also delete any MPlotSeries within it)

		// WAIT: can't delete if it's the last/only plot. Instead, need to remove/delete all series from it and mark it as empty.
		if(channel2Plot_.count() == 1) {
			foreach(MPlotSeriesBasic* series, channelAndScan2Series_[channelName]) {
				channel2Plot_[channelName]->plot()->removeItem(series);
				delete series;
			}

			firstPlot_ = channel2Plot_[channelName];
			firstPlotEmpty_ = true;
		}
		else
			delete channel2Plot_[channelName];

		// remove pointer to deleted plot, and pointers to deleted series
		channel2Plot_.remove(channelName);
		channelAndScan2Series_.remove(channelName);
	}

	// add the channel plots that do:
	foreach(QString channelName, addChannels) {

		// create a new plot, or use the first one if it's available
		MPlotGW* newPlot;

		if(firstPlotEmpty_ == true) {
			newPlot = firstPlot_;
			firstPlotEmpty_ = false;
		}
		else {
			newPlot = new MPlotGW();
			newPlot->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
			newPlot->plot()->axisRight()->setTicks(0);
			newPlot->plot()->axisBottom()->setTicks(4);
			newPlot->plot()->axisLeft()->showGrid(false);
			newPlot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);
			newPlot->plot()->axisBottom()->showAxisName(false);
			newPlot->plot()->axisLeft()->showAxisName(false);
			newPlot->plot()->enableAxisNormalizationLeft(true);
		}

		channel2Plot_.insert(channelName, newPlot);
		channelAndScan2Series_.insert(channelName, QHash<AMScan*, MPlotSeriesBasic*>());
		newPlot->plot()->legend()->setTitleText(channelName);
	}


	// now... for each channel plot, add any series that are missing. Also need to remove any series that have had their visibility turned off...
	foreach(QString channelName, modelChannels) {

		// remove any existing series, that have had their visibility turned off...
		foreach(AMScan* scan, channelAndScan2Series_[channelName].keys()) {
			int si = model()->indexOf(scan);
			int ci = scan->indexOfChannel(channelName);	// cheating (bad design... exploiting the fact that channel indexes are the same inside the scan as they are in our AMScanSetModel model().  Should really use: int ci = model()->indexOf(scan->channel(channelName), scan);
			// if not visible, remove and delete series
			if(!model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>()) {
				channel2Plot_[channelName]->plot()->removeItem(channelAndScan2Series_[channelName][scan]);
				delete channelAndScan2Series_[channelName].take(scan);
			}
		}

		// loop through all scans, adding series to this plot if required...
		for(int si=0; si<model()->numScans(); si++) {
			AMScan* scan = model()->scanAt(si);

			// What's the channel index for this channelName, within this scan?
			// proper, but slower:
			// int ci = model()->indexOf(scan->channel(channelName), scan);
			// faster, but assumes channel indexes are the same in the scan's channelList model as in the AMScanSetModel. (true for now)
			int ci = scan->indexOfChannel(channelName);

			// if this scan contains this channel, and it's visible, and we don't have a series for it yet... make and add the new series
			if(ci >= 0 && model()->data(model()->indexForChannel(si, ci), Qt::CheckStateRole).value<bool>() && !channelAndScan2Series_[channelName].contains(scan)) {
				MPlotSeriesBasic* series = new MPlotSeriesBasic(scan->channel(ci));

				series->setMarker(MPlotMarkerShape::None);
				QPen pen = model()->data(model()->indexForChannel(si, ci), AM::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);
				series->setDescription(scan->fullName());

				channel2Plot_[channelName]->plot()->addItem(series);
				channelAndScan2Series_[channelName].insert(scan, series);
			}
		}

		// only show the detailed legend (with the scan names) if there's more than one scan open. If there's just one, it's kinda obvious, so don't waste the space.
		if(model()->numScans() > 1)
			channel2Plot_[channelName]->plot()->legend()->enableDefaultLegend(true);
		else
			channel2Plot_[channelName]->plot()->legend()->enableDefaultLegend(false);
	}

	return areChanges;
}
