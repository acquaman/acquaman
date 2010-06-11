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

	this->setAutoFillBackground(true);
	ensurePolished();


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
	/*setStyleSheet("QFrame#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");*/



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



AMScanViewInternal::AMScanViewInternal(AMScanView* masterView)
	: QGraphicsWidget(),
	masterView_(masterView) {

	connect(model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}


AMScanSetModel* AMScanViewInternal::model() const { return masterView_->model(); }



AMScanViewExclusiveView::AMScanViewExclusiveView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	setLayout(gl);

	// the list of plotSeries_ needs one element for each scan.
	for(int scanIndex=0; scanIndex < model()->numScans(); scanIndex++) {
		addScan(scanIndex);
	}

	connect(model(), SIGNAL(exclusiveChannelChanged(QString)), this, SLOT(onExclusiveChannelChanged(QString)));
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
}
/// when data changes:
void AMScanViewExclusiveView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
	Q_UNUSED(topLeft)
	Q_UNUSED(bottomRight)
}

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





/////////////////////////////
AMScanViewMultiView::AMScanViewMultiView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	setLayout(gl);

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
void AMScanViewMultiView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
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
				QPen pen = model()->data(model()->indexForChannel(si, ci), AMScanSetModel::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);
			}
		}
	}
}






///////////////////////////////////////////////////

AMScanViewMultiScansView::AMScanViewMultiScansView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	layout_ = new QGraphicsGridLayout();
	layout_->setContentsMargins(0,0,0,0);
	setLayout(layout_);

	// we need to have at least one plot, to fill our widget,  even if there are no scans.
	MPlotGW* plot;
	plot = new MPlotGW();
	plot->plot()->axisRight()->setTicks(0);
	plot->plot()->axisBottom()->setTicks(4);
	plot->plot()->axisLeft()->showGrid(false);
	plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

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
		plot->plot()->axisRight()->setTicks(0);
		plot->plot()->axisBottom()->setTicks(4);
		plot->plot()->axisLeft()->showGrid(false);
		plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

		plots_.insert(si, plot);
	}

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

			plots_[si]->plot()->addItem(series);
			scanList << series;
		}
		else // otherwise, append null series
			scanList << 0;
	}
	plotSeries_.insert(si, scanList);

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
				QPen pen = model()->data(model()->indexForChannel(si, ci), AMScanSetModel::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);

				plots_[si]->plot()->addItem(series);
				plotSeries_[si].insert(ci, series);
			}
			else // otherwise, append null series
				plotSeries_[si].insert(ci, 0);
		}
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
		}
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

			// need to create:
			if(visible && series == 0) {
				plotSeries_[si][ci] = series = new MPlotSeriesBasic(model()->channelAt(si, ci));
				series->setMarker(MPlotMarkerShape::None);
				plots_[si]->plot()->addItem(series);
			}

			// need to get rid of:
			if(!visible && series) {
				plots_[si]->plot()->removeItem(series);
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
	setLayout(layout_);

	// we need to have at least one plot, to fill our widget,  even if there are no scans.
	MPlotGW* plot;
	plot = new MPlotGW();
	plot->plot()->axisRight()->setTicks(0);
	plot->plot()->axisBottom()->setTicks(4);
	plot->plot()->axisLeft()->showGrid(false);
	plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

	firstPlotEmpty_ = true;
	channelPlots_ << plot;

	// add all of the scans we have already
	for(int si=0; si<model()->numScans(); si++) {
		addScan(si);
	}

	reLayout();
}

void AMScanViewMultiChannelsView::addScan(int si) {

	// create plot (unless we're inserting Scan 0 and an empty plot is ready for us)
	if(si == 0 && firstPlotEmpty_) {
		firstPlotEmpty_ = false;
	}
	else {
		MPlotGW* plot;
		plot = new MPlotGW();
		plot->plot()->axisRight()->setTicks(0);
		plot->plot()->axisBottom()->setTicks(4);
		plot->plot()->axisLeft()->showGrid(false);
		plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

		channelPlots_.insert(si, plot);
	}

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

			channelPlots_[si]->plot()->addItem(series);
			scanList << series;
		}
		else // otherwise, append null series
			scanList << 0;
	}
	plotSeries_.insert(si, scanList);

	// note: haven't yet added this new plot to the layout_.  That's up to reLayout();
}

AMScanViewMultiChannelsView::~AMScanViewMultiChannelsView() {

	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).
	for(int si=0; si<plotSeries_.count(); si++)
		for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++)
			if(plotSeries_[si][ci]) {
				channelPlots_[si]->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}*/


	for(int pi=0; pi<channelPlots_.count(); pi++)
		delete channelPlots_.at(pi);
}



void AMScanViewMultiChannelsView::onRowInserted(const QModelIndex& parent, int start, int end) {

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
				QPen pen = model()->data(model()->indexForChannel(si, ci), AMScanSetModel::LinePenRole).value<QPen>();
				pen.setColor(model()->data(model()->indexForChannel(si, ci), Qt::DecorationRole).value<QColor>());
				series->setLinePen(pen);

				channelPlots_[si]->plot()->addItem(series);
				plotSeries_[si].insert(ci, series);
			}
			else // otherwise, append null series
				plotSeries_[si].insert(ci, 0);
		}
	}
}
/// before a scan or channel is deleted in the model:
void AMScanViewMultiChannelsView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			for(int ci=0; ci<model()->scanAt(si)->numChannels(); ci++) {
				if(plotSeries_[si][ci]) {
					channelPlots_[si]->plot()->removeItem(plotSeries_[si][ci]);
					delete plotSeries_[si][ci];
				}
			}
			plotSeries_.removeAt(si);
			if(si == 0 && channelPlots_.count()==1) {
				firstPlotEmpty_ = true;
			}
			else {
				delete channelPlots_.takeAt(si);
			}
		}
		reLayout();
	}

	// removing a channel. parent.row() is the scanIndex, and start - end are the channel indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int ci = end; ci>=start; ci--) {
			if(plotSeries_[si][ci]) {
				channelPlots_[si]->plot()->removeItem(plotSeries_[si][ci]);
				delete plotSeries_[si][ci];
			}
			plotSeries_[si].removeAt(ci);
		}
	}
}
/// after a scan or channel is deleted in the model:
void AMScanViewMultiChannelsView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
}

/// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiChannelsView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

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
				channelPlots_[si]->plot()->addItem(series);
			}

			// need to get rid of:
			if(!visible && series) {
				channelPlots_[si]->plot()->removeItem(series);
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




/// re-do the layout
void AMScanViewMultiChannelsView::reLayout() {

	for(int li=0; li<layout_->count(); li++)
		layout_->removeAt(li);

	int rc=0, cc=0, width = sqrt(channelPlots_.count());

	for(int pi=0; pi<channelPlots_.count(); pi++) {
		layout_->addItem(channelPlots_.at(pi), rc, cc++, Qt::AlignCenter);
		if(cc == width) {
			rc++;
			cc = 0;
		}
	}
}


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


// This function removes from the plot, deletes, and sets to 0 any AMPlotSeriesBasic(simple, expensive version)
void AMScanViewMultiChannelsView::removePlotSeriesWithChannelName(const QString& channelName) {

}

void AMScanViewMultiChannelsView::reviewChannels() {

	QStringList modelChannels = model()->visibleChannelNames();

	// delete all in [us] not contained in [model]
	for(int i=0; i<channelNames_.count(); i++) {
		if(!modelChannels.contains(channelNames_.at(i))) {
			// remove and delete any plotSeries_ with this channel name
			for(int si=0; si<plotSeries_[i].count(); si++) {
				if(plotSeries_[i][si]) {
					channelPlots_[i]->plot()->removeItem(plotSeries_[i][si]);
					delete plotSeries_[i][si];
					plotSeries_[i][si] = 0;
				}
			}
			// delete the plot, unless it's the first one and there are no others
			if(i==0 && channelPlots_.count()==1)
				firstPlotEmpty_ = true;
			else
				delete channelPlots_.takeAt(i);
			// delete the entry in the channel names, remove, and decrement i since the old i will now be the next element
			channelNames_.removeAt(i);
			plotSeries_.removeAt(i);
			i--;
		}
	}

	// iterate through the model list from i=0 onwards:
	for(int i=0; i<modelChannels.count(); i++) {

		if(i >= channelNames_.count()) {
			// append to our lists
			channelNames_.insert(i, modelChannels.at(i));
			if(i==0 && firstPlotEmpty_)
				firstPlotEmpty_ = false;
			else {
				MPlotGW* plot;
				plot = new MPlotGW();
				plot->plot()->axisRight()->setTicks(0);
				plot->plot()->axisBottom()->setTicks(4);
				plot->plot()->axisLeft()->showGrid(false);
				plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

				channelPlots_.insert(i, plot);
			}
			plotSeries_.insert(i, QList<MPlotSeriesBasic*>() );

		}

		else if(channelNames_.at(i) != modelChannels.at(i)) {
			// do we have it somewhere else? Move it
			if(channelNames_.contains(modelChannels.at(i))) {
				int oldLocation = channelNames_.indexOf(modelChannels.at(i));
				channelNames_.move(oldLocation,i);
				channelPlots_.move(oldLocation,i);
				plotSeries_.move(oldLocation, i);
			}
			// otherwise create new and insert at i
			else {
				channelNames_.insert(i, modelChannels.at(i));
				/// \todo plotSeries_.insert(i, emptyPlotSeriesList(model()->numScans()));
				if(i==0 && firstPlotEmpty_)
					firstPlotEmpty_ = false;
				else {
					MPlotGW* plot;
					plot = new MPlotGW();
					plot->plot()->axisRight()->setTicks(0);
					plot->plot()->axisBottom()->setTicks(4);
					plot->plot()->axisLeft()->showGrid(false);
					plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left);

					channelPlots_.insert(i, plot);
				}
			}
		}
	}
}
