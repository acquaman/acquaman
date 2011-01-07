#include "AMScanView.h"
#include <QGraphicsWidget>
#include "dataman/AMDataSourceSeriesData.h"
#include "dataman/AMDataSourceImageData.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotSeries.h"
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
	sourceButtons_.setExclusive(false);

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
		for(int i=0; i<source->dataSourceCount(); i++) {
			QToolButton* sourceButton = new QToolButton();
			sourceButton->setMaximumHeight(18);
			sourceButton->setText(source->dataSourceAt(i)->description());	/// \todo description or name? both? name if description is empty?
			QColor color = model->plotColor(scanIndex, i);
			sourceButton->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));	/// \todo special buttons, with lines underneath that show the line color / style, and differentiate 1D, 2D datasets.
			sourceButton->setCheckable(true);
			sourceButton->setChecked(model->isVisible(scanIndex, i));
			sourceButtons_.addButton(sourceButton, i);
			cramBar_->addWidget(sourceButton);
		}
	}

	hl->addWidget(cramBar_);
	hl->addStretch(1);

	/* REMOVED
	closeButton_ = new QToolButton();
	closeButton_->setText("X");
	hl->addWidget(closeButton_);
	*/

	hl->setContentsMargins(6, 0, 6, 0);
	hl->setSpacing(24);
	setLayout(hl);

	//unnecessary: this->setAutoFillBackground(true);
	//unnecessary: ensurePolished();


	connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
	connect(model, SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));

	connect(&sourceButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onSourceButtonClicked(int)));

	// connect(closeButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

}




void AMScanViewScanBar::onRowInserted(const QModelIndex& parent, int start, int end) {
	// not for us...
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}


	// it is for us... (parent index is our Scan, and it is a new data source)
	AMScan* source = model_->scanAt(scanIndex_);
	// note: AMScanSetModel guarantees only one row inserted at a time, but we don't depend on that...
	for(int i=start; i<=end; i++) {
		QToolButton* newButton = new QToolButton();
		newButton->setText(source->dataSourceAt(i)->name());
		newButton->setCheckable(true);
		QColor color = model_->plotColor(scanIndex_, i);
		newButton->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
		sourceButtons_.addButton(newButton, i);
		cramBar_->getLayout()->insertWidget(i, newButton);
		if(exclusiveModeOn_)
			newButton->setChecked( (model_->exclusiveDataSourceName() == source->dataSourceAt(i)->name()) );
		else
			newButton->setChecked(model_->isVisible(scanIndex_, i));

		// qDebug() << "added a data source. exclusiveModeOn is: " << exclusiveModeOn_ << ", source name is:" << source->dataSourceAt(i)->name() << ", exclusiveDataSourceName is:" << model_->exclusiveDataSourceName();
	}

}

// before a scan or data source is deleted in the model.
void AMScanViewScanBar::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(end)

	// check if this isn't one of our data sources being deleted.
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}


	// (AMScanSetModel guarantees only one removed at once -- ie: start == end --, but we don't depend on that)
	for(int di = end; di>=start; di-- ) {
		delete sourceButtons_.button(di);
		// the button group's id's from "start+1" to "count+1" are too high now...
		for(int i=di+1; i<sourceButtons_.buttons().count()+1; i++)
			sourceButtons_.setId(sourceButtons_.button(i), i-1);
	}
}

// after a scan or data source is deleted in the model:
void AMScanViewScanBar::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
	}

// when model data changes.  Possibilities we care about: scan name, and data sources visible/not visible.
/*! This assumes that topLeft == bottomRight, which is ok, given that AMScanSetModel always emits dataChanged() for single items. */
void AMScanViewScanBar::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	Q_UNUSED(bottomRight)

	// changes to our scan:
	if(topLeft.internalId() == -1 && topLeft.row() == scanIndex_) {

		nameLabel_->setText(model_->scanAt(scanIndex_)->name() + QString(" #%1").arg(model_->scanAt(scanIndex_)->number()));
	}

	// changes to one of our data sources:
	else if(topLeft.internalId() == scanIndex_) {

		int dataSourceIndex = topLeft.row();
		AMDataSource* dataSource = model_->dataSourceAt(scanIndex_, dataSourceIndex);
		sourceButtons_.button(dataSourceIndex)->setText(dataSource->description());
		// setting visibility: depends on whether exclusiveMode is on or not
		if(exclusiveModeOn_)
			sourceButtons_.button(dataSourceIndex)->setChecked( (model_->exclusiveDataSourceName() == dataSource->name()) );
		else
			sourceButtons_.button(dataSourceIndex)->setChecked(model_->isVisible(scanIndex_, dataSourceIndex));

		QColor color = model_->plotColor(scanIndex_, dataSourceIndex);
		sourceButtons_.button(dataSourceIndex)->setStyleSheet(QString("color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
	}
}

void AMScanViewScanBar::onSourceButtonClicked(int id) {
	// if in "exclusive" mode, when clicked, set that data source as the exclusive data source
	if(exclusiveModeOn_) {
		//if(visible == Qt::Checked)
		model_->setExclusiveDataSourceByName(model_->dataSourceAt(scanIndex_, id)->name());
		sourceButtons_.button(id)->setChecked(true);
	}
	else {
		model_->setVisible(scanIndex_, id, sourceButtons_.button(id)->isChecked());
	}
}


void AMScanViewScanBar::onExclusiveDataSourceChanged(const QString& exlusiveDataSourceName) {
	// when exclusive mode is on, set checked only when data source name matches the exclusive one
	if(exclusiveModeOn_) {
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++)
			sourceButtons_.button(di)->setChecked( (model_->dataSourceAt(scanIndex_, di)->name() == exlusiveDataSourceName) );
	}
}


void AMScanViewScanBar::setExclusiveModeOn(bool exclusiveModeOn) {

	// turning exclusiveMode on:
	if(exclusiveModeOn && !exclusiveModeOn_) {
		exclusiveModeOn_ = true;
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++) {
			sourceButtons_.button(di)->setChecked( (model_->dataSourceAt(scanIndex_, di)->name() == model_->exclusiveDataSourceName()) );
		}
	}

	// turning exclusiveMode off:
	if(!exclusiveModeOn && exclusiveModeOn_) {
		exclusiveModeOn_ = false;
		//chButtons_.setExclusive(false);
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++) {
			sourceButtons_.button(di)->setChecked( model_->isVisible(scanIndex_, di) );
		}
	}
}




AMScanViewSourceSelector::AMScanViewSourceSelector(AMScanSetModel* model, QWidget* parent)
	: QWidget(parent) {
	model_ = 0;
	setModel(model);

	barLayout_ = new QVBoxLayout();
	barLayout_->setMargin(0);
	barLayout_->setSpacing(0);
	setLayout(barLayout_);

	exclusiveModeOn_ = false;
}


void AMScanViewSourceSelector::setModel(AMScanSetModel* model) {
	// remove anything associated with the old model:
	if(model_) {

		while(!scanBars_.isEmpty()) {
			delete scanBars_.takeLast();
		}

		disconnect(model_, 0, this, 0);
	}

	model_ = model;

	// add the new model, if it's valid.
	if(model_) {
		// add existing
		for(int i=0; i<model_->scanCount(); i++) {
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

// after scans or data sources are added in the model
void AMScanViewSourceSelector::onRowInserted(const QModelIndex& parent, int start, int end) {

	// top-level: inserting scans:
	if(!parent.isValid()) {
		for(int si=start; si<=end; si++) {
			AMScanViewScanBar* bar = new AMScanViewScanBar(model_, si);
			bar->setExclusiveModeOn(exclusiveModeOn_);
			barLayout_->insertWidget(si, bar);
			scanBars_.insert(si, bar);
		}
	}

	// otherwise, inserting a data source. Handled separately by our AMScanViewScanBar's
}

// before scans or data sources are deleted in the model:
void AMScanViewSourceSelector::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
	// invalid (top-level) parent: means we're removing scans
	if(!parent.isValid()){
		for(int si=end; si>=start; si--) {
			delete scanBars_.takeAt(si);
			// all the scans above this one need to move their scan index down:
			for(int i=si; i<scanBars_.count(); i++)
				scanBars_[i]->scanIndex_--;
		}
	}
}

// ScanBars have two behaviours.  When exclusiveMode is on, they only allow one data source to be "checked" or selected at a time, and tell the model to make this the exclusive data source.  Otherwise, they allow multiple data sources within each Scan to be checked, and toggle the data sources' visibility in the model.
void AMScanViewSourceSelector::setExclusiveModeOn(bool exclusiveModeOn) {
	exclusiveModeOn_ = exclusiveModeOn;
	foreach(AMScanViewScanBar* bar, scanBars_) {
		bar->setExclusiveModeOn(exclusiveModeOn);
	}
}













#include <QCheckBox>
#include <QDoubleSpinBox>

AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QFrame(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->setSpacing(0);


	QToolButton* tabButton_ = new QToolButton();
	tabButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	tabButton_->setText("1");
	QToolButton* overplotButton_ = new QToolButton();
	overplotButton_->setText("OP");
	overplotButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	QToolButton* multiScansButton_ = new QToolButton();
	multiScansButton_->setText("M-S");
	multiScansButton_->setAttribute(Qt::WA_MacBrushedMetal, true);
	QToolButton* multiSourcesButton = new QToolButton();
	multiSourcesButton->setText("M-C");
	multiSourcesButton->setAttribute(Qt::WA_MacBrushedMetal, true);

	tabButton_->setCheckable(true);
	overplotButton_->setCheckable(true);
	multiScansButton_->setCheckable(true);
	multiSourcesButton->setCheckable(true);

	modeButtons_ = new QButtonGroup(this);
	modeButtons_->addButton(tabButton_, AMScanView::Tabs);
	modeButtons_->addButton(overplotButton_, AMScanView::OverPlot);
	modeButtons_->addButton(multiScansButton_, AMScanView::MultiScans);
	modeButtons_->addButton(multiSourcesButton, AMScanView::MultiSources);
	tabButton_->setChecked(true);

	hl2->addWidget(tabButton_);
	hl2->addWidget(overplotButton_);
	hl2->addWidget(multiScansButton_);
	hl2->addWidget(multiSourcesButton);

	hl->addLayout(hl2);
	hl->addStretch(1);

	normalizationCheckBox_ = new QCheckBox("Show at same scale");
	normalizationCheckBox_->setChecked(true);
	hl->addWidget(normalizationCheckBox_);
	waterfallCheckBox_ = new QCheckBox("Waterfall  Amount");
	waterfallCheckBox_->setChecked(true);
	hl->addWidget(waterfallCheckBox_);
	waterfallAmount_ = new QDoubleSpinBox();
	waterfallAmount_->setMinimum(0);
	waterfallAmount_->setMaximum(1e12);
	waterfallAmount_->setValue(0.2);
	waterfallAmount_->setSingleStep(0.1);
	hl->addWidget(waterfallAmount_);

	hl->setMargin(6);
	// hl->setSpacing(24);
	setLayout(hl);

	setObjectName("AMScanViewModeBar");
	/*setStyleSheet("QFrame#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");*/

	connect(normalizationCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(normalizationEnabled(bool)));
	connect(waterfallCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(waterfallOffsetEnabled(bool)));
	connect(waterfallAmount_, SIGNAL(valueChanged(double)), this, SIGNAL(waterfallOffsetChanged(double)));
}






AMScanView::AMScanView(AMScanSetModel* model, QWidget *parent) :
		QWidget(parent)
{
	mode_ = Invalid;

	scansModel_ = model;
	if(scansModel_ == 0)
		scansModel_ = new AMScanSetModel(this);

	setupUI();
	makeConnections();

	scanBars_->setModel(scansModel_);

	modeAnim_ = new QPropertyAnimation(gview_->graphicsWidget(), "geometry", this);
	modeAnim_->setDuration(500);
	modeAnim_->setEasingCurve(QEasingCurve::InOutCubic);

	changeViewMode(Tabs);


}


AMScanView::~AMScanView() {
	for(int i=0; i<views_.count(); i++)
		delete views_.at(i);
}

void AMScanView::setupUI() {
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setMargin(6);
	vl->setSpacing(0);

	gview_ = new AMGraphicsViewAndWidget();
	gview_->setMinimumSize(400,300);
	gview_->graphicsWidget()->setGeometry(0,0,640*4, 480);

	vl->addWidget(gview_);

	scanBars_ = new AMScanViewSourceSelector();
	vl->addWidget(scanBars_);

	vl->addSpacing(6);

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
	views_ << new AMScanViewMultiSourcesView(this);

	for(int i=0; i<views_.count(); i++) {
		AMScanViewInternal* v = views_.at(i);
		v->enableNormalization(true);
		v->setWaterfallOffset(0.2);
		v->enableWaterfallOffset(true);
		glayout_->addItem(v);
	}

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

	// connect enabling/disabling normalization and waterfall to each view
	for(int i=0; i<views_.count(); i++) {
		AMScanViewInternal* v = views_.at(i);
		connect(modeBar_, SIGNAL(normalizationEnabled(bool)), v, SLOT(enableNormalization(bool)));
		connect(modeBar_, SIGNAL(waterfallOffsetEnabled(bool)), v, SLOT(enableWaterfallOffset(bool)));
		connect(modeBar_, SIGNAL(waterfallOffsetChanged(double)), v, SLOT(setWaterfallOffset(double)));
	}
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

// remove a scan from the view:
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

	normalizationEnabled_ = false;
	waterfallEnabled_ = false;
	waterfallOffset_  = 0;

}


AMScanSetModel* AMScanViewInternal::model() const { return masterView_->model(); }



AMScanViewExclusiveView::AMScanViewExclusiveView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left | MPlotAxis::Right);
	plot_->plot()->axisBottom()->showAxisName(false);
	plot_->plot()->axisLeft()->showAxisName(false);

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	gl->setSizePolicy(sizePolicy());
	setLayout(gl);


	// the list of plotItems_ needs one element for each scan.
	for(int scanIndex=0; scanIndex < model()->scanCount(); scanIndex++) {
		addScan(scanIndex);
	}

	connect(model(), SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));

	refreshTitle();
}

AMScanViewExclusiveView::~AMScanViewExclusiveView() {
	// PlotSeries's will be deleted as children items of the plot.

	delete plot_;
}



void AMScanViewExclusiveView::onRowInserted(const QModelIndex& parent, int start, int end) {

	// inserting scans:
	if(!parent.isValid()) {
		for(int i=start; i<=end; i++)
			addScan(i);
	}

	// inserting data sources: (parent.row is the scanIndex, start-end are the data source indices)
	else if(parent.internalId() == -1) {
		reviewScan(parent.row());
	}

	refreshTitle();
}
// before scans or data sources is deleted in the model:
void AMScanViewExclusiveView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int i=end; i>=start; i--) {
			if(plotItems_.at(i)) {
				// unnecessary: deleting an item removes it from its plot: plot_->plot()->removeItem(plotItems_.at(i));
				delete plotItems_.at(i);
				plotItems_.removeAt(i);
				plotItemDataSources_.removeAt(i);
			}
		}
	}

	// removing a data source. parent.row() is the scanIndex, and start - end are the data source indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int ci = start; ci<=end; ci++) {
			// Are we displaying the data source that's about to be removed? If we are, we're going to lose it...
			if(model()->dataSourceAt(si, ci) == plotItemDataSources_.at(si)) {
				if(plotItems_.at(si)) {
					// unnecessary: deleting an item removes it from its plot: plot_->plot()->removeItem(plotItems_.at(si));
					delete plotItems_.at(si);
					plotItems_[si] = 0;
				}
				plotItemDataSources_[si] = 0;
			}
		}
	}

}

// after a scan or data source is deleted in the model:
void AMScanViewExclusiveView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	refreshTitle();

}
// when data changes.
void AMScanViewExclusiveView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	// the one situation we need to worry about here: if the color, line pen, or colormap of a data source changes, and it's the exclusive (currently-displayed) data source, we'll need to update the plot item.

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		// no changes that we really care about...
	}

	// changes to one of our data sources: internalId is the scanIndex.  Data source indices are from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->scanCount()) {

		int si = topLeft.internalId();	// si: scan index
		for(int di=topLeft.row(); di<=bottomRight.row(); di++) {	// di: data source index

			AMDataSource* dataSource = model()->dataSourceAt(si, di);
			// if this data source is the one we're currently displaying...
			if(dataSource == plotItemDataSources_.at(si)) {
				switch(dataSource->rank()) {
				case 1: {
					QColor color = model()->plotColor(si, di);
					QPen pen = model()->plotPen(si, di);
					pen.setColor(color);
					static_cast<MPlotAbstractSeries*>(plotItems_.at(si))->setLinePen(pen);
					break; }
				case 2: {
					/// \todo This is inefficient... Institute separate signals for different dataChanged() parameters, or check if old color map matches new color map.
					static_cast<MPlotAbstractImage*>(plotItems_.at(si))->setColorMap(model()->plotColorMap(si, di));
					break;
				default:
					break; }
				}
			}
		}
	}

}

// when the model's "exclusive data source" changes. This is the one data source that we display for all of our scans (as long as they have it).
void AMScanViewExclusiveView::onExclusiveDataSourceChanged(const QString& exclusiveDataSourceName) {
	Q_UNUSED(exclusiveDataSourceName)

	for(int i=0; i<model()->scanCount(); i++) {
		reviewScan(i);
	}

	refreshTitle();
}

void AMScanViewExclusiveView::refreshTitle() {

	int numScansShown = 0;
	foreach(MPlotItem* s, plotItems_)
		if(s)
			numScansShown++;

	QString scanCount = (numScansShown == 1) ? " (1 scan)" : QString(" (%1 scans)").arg(numScansShown);
	/// \todo This should show a data source description; not a name. However, we don't know that all the descriptions (for all the data sources matching the exclusive data source) are the same....
	plot_->plot()->legend()->setTitleText(model()->exclusiveDataSourceName() + scanCount);	// result ex: "tey (3 scans)"
}

// Helper function to handle adding a scan (at row scanIndex in the model)
void AMScanViewExclusiveView::addScan(int scanIndex) {

	QString dataSourceName = model()->exclusiveDataSourceName();

	int dataSourceIndex = model()->scanAt(scanIndex)->indexOfDataSource(dataSourceName);
	AMDataSource* dataSource = model()->dataSourceAt(scanIndex, dataSourceIndex);	// dataSource will = 0 if this scan doesn't have the exclusive data source in it.

	MPlotItem* newItem = 0;
	if(dataSource)
		newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(scanIndex, dataSourceIndex));

	if(newItem) {
		newItem->setDescription(model()->scanAt(scanIndex)->fullName());
		plot_->plot()->addItem(newItem);
	}
	plotItems_.insert(scanIndex, newItem);
	plotItemDataSources_.insert(scanIndex, dataSource);

}

// Helper function to create an appropriate MPlotItem and connect it to the data, depending on the dimensionality of \c dataSource.  Returns 0 if we can't handle this dataSource and no item was created (ex: unsupported dimensionality; we only handle 1D or 2D data for now.)
MPlotItem* AMScanViewInternal::createPlotItemForDataSource(const AMDataSource* dataSource, const AMDataSourcePlotSettings& plotSettings) {
	MPlotItem* rv = 0;

	if(dataSource == 0)
		return 0;

	switch(dataSource->rank()) {	// depending on the rank, we'll need an XY-series or an image to display it. 3D and 4D, etc. we don't handle for now.

	case 1: {
		MPlotSeriesBasic* series = new MPlotSeriesBasic();
		series->setModel(new AMDataSourceSeriesData(dataSource), true);
		series->setMarker(MPlotMarkerShape::None);
		QPen pen = plotSettings.linePen;
		pen.setColor(plotSettings.color);
		series->setLinePen(pen);
		rv = series;
		break; }

	case 2: {
		MPlotImageBasic* image = new MPlotImageBasic();
		image->setModel(new AMDataSourceImageData(dataSource), true);
		image->setColorMap(plotSettings.colorMap);
		image->setYAxisTarget(MPlotAxis::Right);
		image->setZValue(-1000);
		rv = image;
		break; }
	default:
		rv = 0;
		break;
	}

	return rv;
}

// Helper function to review a scan when a data source is added or the exclusive data source changes.
void AMScanViewExclusiveView::reviewScan(int scanIndex) {

	QString dataSourceName = model()->exclusiveDataSourceName();

	int dataSourceIndex = model()->scanAt(scanIndex)->indexOfDataSource(dataSourceName);
	AMDataSource* dataSource = model()->dataSourceAt(scanIndex, dataSourceIndex);

	// does this scan have the "exclusive" data source in it?
	if(dataSource) {


		// the current plot item exists, but it's the wrong type to handle the current scan data. (ie: dataSource->rank() is 2, but we've currently got a plot series instead of a plot image.
		if(plotItems_.at(scanIndex) && plotItems_.at(scanIndex)->rank() != dataSource->rank() ) {
			// delete the plot item; we'll recreate the new one of the proper size in the next check.
			delete plotItems_.at(scanIndex);
			plotItems_[scanIndex] = 0;
			plotItemDataSources_[scanIndex] = 0;
		}

		// need to create new plot item for this scan. (Don't have one yet)
		if(plotItems_.at(scanIndex) == 0) {
			plotItems_[scanIndex] = createPlotItemForDataSource(dataSource, model()->plotSettings(scanIndex, dataSourceIndex));
			plotItems_.at(scanIndex)->setDescription(model()->scanAt(scanIndex)->fullName());
			plot_->plot()->addItem(plotItems_.at(scanIndex));
			plotItemDataSources_[scanIndex] = dataSource;
		}

		else {	// We already have one.  Review and update the existing plot item. (When would this be called? // A: When the exclusive data source changes, for one thing. need to change old series/image to represent new data)
			plotItems_.at(scanIndex)->setDescription(model()->scanAt(scanIndex)->fullName());

			switch(dataSource->rank()) {
			case 1: {
				MPlotAbstractSeries* series = static_cast<MPlotAbstractSeries*>(plotItems_.at(scanIndex));
				if(plotItemDataSources_.at(scanIndex) != dataSource) {
					series->setModel(new AMDataSourceSeriesData(dataSource), true);
					plotItemDataSources_[scanIndex] = dataSource;
				}
				QPen pen = model()->plotPen(scanIndex, dataSourceIndex);
				QColor color = model()->plotColor(scanIndex, dataSourceIndex);
				pen.setColor(color);
				series->setLinePen(pen);
				break; }
			case 2: {
				MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItems_.at(scanIndex));
				if(plotItemDataSources_.at(scanIndex) != dataSource) {
					AMDataSourceImageData* newData = new AMDataSourceImageData(dataSource);
					image->setModel(newData, true);
					plotItemDataSources_[scanIndex] = dataSource;
				}
				image->setColorMap(model()->plotColorMap(scanIndex, dataSourceIndex));
				break; }
			default:
				break;
			}
		}
	}
	// if this scan doesn't have the exclusive data source, but we used to have it. Remove the old plot.
	else {
		if(plotItems_.at(scanIndex)) {
			delete plotItems_.at(scanIndex);
			plotItems_[scanIndex] = 0;
		}
		plotItemDataSources_[scanIndex] = 0;
	}
}


void AMScanViewExclusiveView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	plot_->plot()->enableAxisNormalizationLeft(normalizationOn, min, max);

}

void AMScanViewExclusiveView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	if(waterfallOn)
		plot_->plot()->setWaterfallLeft(waterfallOffset_);
	else
		plot_->plot()->setWaterfallLeft(0);

}

void AMScanViewExclusiveView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_)
		plot_->plot()->setWaterfallLeft(offset);

}

/////////////////////////////
AMScanViewMultiView::AMScanViewMultiView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	// create our main plot:
	plot_ = new MPlotGW();
	plot_->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->plot()->axisRight()->setTicks(0);
	plot_->plot()->axisBottom()->setTicks(4);
	plot_->plot()->axisLeft()->showGrid(false);
	plot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left | MPlotAxis::Right);
	plot_->plot()->axisBottom()->showAxisName(false);
	plot_->plot()->axisLeft()->showAxisName(false);
	plot_->plot()->legend()->enableDefaultLegend(false);	// turn on or turn off labels for individual scans in this plot

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	gl->setSizePolicy(sizePolicy());
	setLayout(gl);

	// the list of plotSeries_ needs one list for each scan,
	for(int si=0; si<model()->scanCount(); si++) {
		addScan(si);
	}

	refreshTitles();
}

void AMScanViewMultiView::addScan(int si) {
	QList<MPlotItem*> scanList;

	for(int di=0; di<model()->scanAt(si)->dataSourceCount(); di++) {

		AMDataSource* dataSource = model()->dataSourceAt(si, di);
		// if visible, create and append the list
		if(model()->isVisible(si, di)) {

			MPlotItem* newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(si, di));
			if(newItem) {
				plot_->plot()->addItem(newItem);
				newItem->setDescription(model()->scanAt(si)->fullName() + ": " + dataSource->name());
			}
			scanList << newItem;
		}
		else // otherwise, append null series
			scanList << 0;
	}
	plotItems_.insert(si, scanList);
}

AMScanViewMultiView::~AMScanViewMultiView() {

	/* Not necessary; deleting the plot should delete its children.
	for(int si=0; si<plotSeries_.count(); si++)
		for(int ci=0; ci<model()->scanAt(si)->dataSourceCount(); ci++)
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

	// inserting data sources: (parent.row is the scanIndex, start-end are the data source indices)
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int di=start; di<=end; di++) {

			AMDataSource* dataSource = model()->dataSourceAt(si, di);
			// if visible, create and append to the list, and add to plot.
			if(model()->isVisible(si, di)) {

				MPlotItem* newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(si, di));
				if(newItem) {
					newItem->setDescription(model()->scanAt(si)->fullName() + ": " + dataSource->name());
					plot_->plot()->addItem(newItem);
				}
				plotItems_[si].insert(di, newItem);
			}
			else // otherwise, append null series
				plotItems_[si].insert(di, 0);
		}
	}

	refreshTitles();
}
// before scans or dataSources are deleted in the model:
void AMScanViewMultiView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing scans: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			for(int ci=0; ci<model()->scanAt(si)->dataSourceCount(); ci++) {
				if(plotItems_[si][ci]) {
					// unnecessary: plot_->plot()->removeItem(plotItems_[si][ci]);
					delete plotItems_[si][ci];
				}
			}
			plotItems_.removeAt(si);
		}
	}

	// removing a dataSource. parent.row() is the scanIndex, and start - end are the data source indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int di = end; di>=start; di--) {
			if(plotItems_[si][di]) {
				// unnecessary: plot_->plot()->removeItem(plotItems_[si][ci]);
				delete plotItems_[si][di];
			}
			plotItems_[si].removeAt(di);
		}
	}


}
// after scans or data sources are deleted in the model:
void AMScanViewMultiView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	refreshTitles();
}

// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		// no changes that we really care about...

	}

	// changes to our data sources: internalId is the scanIndex.  data source index is from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->scanCount()) {

		int si = topLeft.internalId();
		for(int di=topLeft.row(); di<=bottomRight.row(); di++) {

			// handle visibility changes:
			bool visible = model()->isVisible(si, di);

			MPlotItem* plotItem = plotItems_[si][di];

			if(visible) {	// plot item should be visible
				// need to create?
				if(plotItem == 0) {
					plotItems_[si][di] = plotItem = createPlotItemForDataSource(model()->dataSourceAt(si, di), model()->plotSettings(si, di));
					if(plotItem) {
						plot_->plot()->addItem(plotItem);
						plotItem->setDescription(model()->scanAt(si)->fullName() + ": " + model()->dataSourceAt(si, di)->description());
					}
				}
				// otherwise, already have a plot item; just need to update it.
				else {
					plotItem->setDescription(model()->scanAt(si)->fullName() + ": " + model()->dataSourceAt(si, di)->description());
					switch(model()->dataSourceAt(si, di)->rank()) {
					case 1: {
						QPen pen = model()->plotPen(si, di);
						pen.setColor(model()->plotColor(si, di));
						static_cast<MPlotAbstractSeries*>(plotItem)->setLinePen(pen);
						break; }
					case 2: {
						/// \todo This is inefficient... Institute separate signals for different dataChanged() parameters, or check if old color map matches new color map.
						static_cast<MPlotAbstractImage*>(plotItem)->setColorMap(model()->plotColorMap(si, di));
						break; }
					default:
						break;
					}
				}
			}


			else {	// shouldn't be visible
				if(plotItem) { // need to get rid of?  (ie: it's not supposed to be visible, but we have a plot item)
					plot_->plot()->removeItem(plotItem);
					delete plotItem;
					plotItems_[si][di] = 0;
				}
			}
		}
	}

	refreshTitles();
}


void AMScanViewMultiView::refreshTitles() {

	if(model()->scanCount() == 1)
		plot_->plot()->legend()->setTitleText("1 scan");
	else
		plot_->plot()->legend()->setTitleText(QString("%1 scans").arg(model()->scanCount()));

	/// \todo set legend description on all plot series, and then case use default legend text, instead of custom body text

	plot_->plot()->legend()->setBodyText(model()->scanNames().join("<br>"));
}


void AMScanViewMultiView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	plot_->plot()->enableAxisNormalizationLeft(normalizationOn, min, max);
}

void AMScanViewMultiView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	if(waterfallOn)
		plot_->plot()->setWaterfallLeft(waterfallOffset_);
	else
		plot_->plot()->setWaterfallLeft(0);
}

void AMScanViewMultiView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_)
		plot_->plot()->setWaterfallLeft(offset);
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
	plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left | MPlotAxis::Right);
	plot->plot()->axisBottom()->showAxisName(false);
	plot->plot()->axisLeft()->showAxisName(false);
	plot->plot()->legend()->enableDefaultLegend(false);	/// \todo Right now we maintain our own legend (instead of using MPlotLegend's automatic one), to keep it sorted by data source order. If you could introduce consistent ordering to MPlotLegend and MPlot::items(), we wouldn't have to.  [done, can insertPlotItem()s instead of addPlotItem()s now.]

	firstPlotEmpty_ = true;
	plots_ << plot;

	// add all of the scans we have already
	for(int si=0; si<model()->scanCount(); si++) {
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
		plot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left | MPlotAxis::Right);
		plot->plot()->axisBottom()->showAxisName(false);
		plot->plot()->axisLeft()->showAxisName(false);
		plot->plot()->legend()->enableDefaultLegend(false);

		plot->plot()->enableAxisNormalizationLeft(normalizationEnabled_, normMin_, normMax_);
		if(waterfallEnabled_)
			plot->plot()->setWaterfallLeft(waterfallOffset_);

		plots_.insert(si, plot);
	}

	QList<MPlotItem*> scanList;
	QStringList sourceLegendText;

	for(int di=0; di<model()->scanAt(si)->dataSourceCount(); di++) {

		AMDataSource* dataSource = model()->dataSourceAt(si, di);
		// if visible, create and append the list
		if(model()->isVisible(si, di)) {

			MPlotItem* newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(si, di));
			if(newItem) {
				newItem->setDescription(dataSource->description());
				plots_.at(si)->plot()->addItem(newItem);
			}
			scanList << newItem;

			QColor color = model()->plotColor(si, di);
			sourceLegendText << QString("<font color=#%1%2%3>%4</font><br>")
					.arg(color.red(), 2, 16, QChar('0'))
					.arg(color.green(), 2, 16, QChar('0'))
					.arg(color.blue(), 2, 16, QChar('0'))
					.arg(dataSource->description());	/// \todo use data source description or name?
			/// \todo convert to using built-in legend system.

		}
		else { // otherwise, append null series
			scanList << 0;
			sourceLegendText << QString();
		}
	}
	plotItems_.insert(si, scanList);
	plotLegendText_.insert(si, sourceLegendText);

	// set plot title and legend
	plots_.at(si)->plot()->legend()->setTitleText(model()->scanAt(si)->fullName());
	refreshLegend(si);

	// note: haven't yet added this new plot to the layout_.  That's up to reLayout();
}

AMScanViewMultiScansView::~AMScanViewMultiScansView() {

	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).
	for(int si=0; si<plotSeries_.count(); si++)
		for(int ci=0; ci<model()->scanAt(si)->dataSourceCount(); ci++)
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

	// inserting a data source: (parent.row is the scanIndex, start-end are the data source indices)
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int di=start; di<=end; di++) {

			AMDataSource* dataSource = model()->dataSourceAt(si, di);
			// if visible, create and append to the list, and add to plot.
			if(model()->isVisible(si, di)) {

				MPlotItem* newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(si, di));
				if(newItem) {
					newItem->setDescription(dataSource->description());
					plots_.at(si)->plot()->addItem(newItem);
				}
				plots_[si]->plot()->addItem(newItem);
				plotItems_[si].insert(di, newItem);

				QColor color = model()->plotColor(si, di);
				QString legendText = QString("<font color=#%1%2%3>%4</font><br>")
									 .arg(color.red(), 2, 16, QChar('0'))
									 .arg(color.green(), 2, 16, QChar('0'))
									 .arg(color.blue(), 2, 16, QChar('0'))
									 .arg(dataSource->description());
				plotLegendText_[si].insert(di, legendText);
			}
			else { // otherwise, append null series
				plotItems_[si].insert(di, 0);
				plotLegendText_[si].insert(di, QString());
			}
		}
		// update the legend text
		refreshLegend(si);
	}
}
// before a scan or data source is deleted in the model:
void AMScanViewMultiScansView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			for(int di=0; di<model()->scanAt(si)->dataSourceCount(); di++) {
				if(plotItems_[si][di]) {
					// unnecessary: plots_[si]->plot()->removeItem(plotSeries_[si][di]);
					delete plotItems_[si][di];
				}
			}
			plotItems_.removeAt(si);
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

	// removing data sources. parent.row() is the scanIndex, and start - end are the data source indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int di = end; di>=start; di--) {
			if(plotItems_[si][di]) {
				// unnecessary: plots_[si]->plot()->removeItem(plotSeries_[si][di]);
				delete plotItems_[si][di];
			}
			plotItems_[si].removeAt(di);
			plotLegendText_[si].removeAt(di);
		}
		refreshLegend(si);
	}
}
// after scans or data sources deleted in the model:
void AMScanViewMultiScansView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
	}

// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiScansView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		// no changes that we really care about...

	}

	// changes to our data sources: internalId is the scanIndex.  Data source index is from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->scanCount()) {

		int si = topLeft.internalId();
		for(int di=topLeft.row(); di<=bottomRight.row(); di++) {

			// handle visibility changes:
			bool visible = model()->isVisible(si, di);

			MPlotItem* plotItem = plotItems_[si][di];

			if(visible) {
				if(plotItem == 0) { // need to create? (becoming visible)
					plotItems_[si][di] = plotItem = createPlotItemForDataSource(model()->dataSourceAt(si, di), model()->plotSettings(si, di));
					if(plotItem) {
						plotItem->setDescription(model()->dataSourceAt(si, di)->description());
						plots_[si]->plot()->addItem(plotItem);
					}
				}
				else {	// apply data changes to existing plot item
					plotItem->setDescription(model()->dataSourceAt(si, di)->description());

					switch(model()->dataSourceAt(si, di)->rank()) {
					case 1: {
						QPen pen = model()->plotPen(si, di);
						pen.setColor(model()->plotColor(si, di));
						static_cast<MPlotAbstractSeries*>(plotItem)->setLinePen(pen);
						break; }
					case 2: {
						/// \todo This is inefficient... Institute separate signals for different dataChanged() parameters, or check if old color map matches new color map.
						static_cast<MPlotAbstractImage*>(plotItem)->setColorMap(model()->plotColorMap(si, di));
						break; }
					default:
						break;
					}

				}

				QColor color = model()->plotColor(si, di);
				plotLegendText_[si][di] = QString("<font color=#%1%2%3>%4</font><br>")
										  .arg(color.red(), 2, 16, QChar('0'))
										  .arg(color.green(), 2, 16, QChar('0'))
										  .arg(color.blue(), 2, 16, QChar('0'))
										  .arg(model()->dataSourceAt(si, di)->description());	/// \todo: warning in legend if plot item can't be displayed (ex: higher dimensionality?) -- easy to detect: plotItem would be = 0 here still.
			}


			else {	// shouldn't be visible.
				if(plotItem) {	// need to get rid of? (becoming invisible)
					// unnecessary: plots_[si]->plot()->removeItem(plotItem);
					delete plotItem;
					plotItems_[si][di] = 0;
				}
				plotLegendText_[si][di] = QString(); // if invisible, remove from legend
			}
		}
		refreshLegend(si);
	}
}




// re-do the layout
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




void AMScanViewMultiScansView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	for(int i=0; i<plots_.count(); i++) {
		plots_.at(i)->plot()->enableAxisNormalizationLeft(normalizationOn, min, max);
	}
}

void AMScanViewMultiScansView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	for(int i=0; i<plots_.count(); i++) {
		if(waterfallOn)
			plots_.at(i)->plot()->setWaterfallLeft(waterfallOffset_);
		else
			plots_.at(i)->plot()->setWaterfallLeft(0);
	}

}

void AMScanViewMultiScansView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_)
		for(int i=0; i<plots_.count(); i++)
			plots_.at(i)->plot()->setWaterfallLeft(offset);
}





//////////////////////////////////////////////////


AMScanViewMultiSourcesView::AMScanViewMultiSourcesView(AMScanView* masterView) : AMScanViewInternal(masterView) {

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
	firstPlot_->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left | MPlotAxis::Right);
	firstPlot_->plot()->axisBottom()->showAxisName(false);
	firstPlot_->plot()->axisLeft()->showAxisName(false);

	firstPlotEmpty_ = true;

	// add all of the scans/data sources we have already
	reviewDataSources();

	reLayout();
}



AMScanViewMultiSourcesView::~AMScanViewMultiSourcesView() {

	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).*/

	foreach(MPlotGW* plot, dataSource2Plot_)
		delete plot;
}



void AMScanViewMultiSourcesView::onRowInserted(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(reviewDataSources())
		reLayout();
}


// before scans or data sources are deleted in the model:
void AMScanViewMultiSourcesView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan(s):    (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			AMScan* scan = model()->scanAt(si);

			// go through all data sources that this scan has; check if they exist; remove and delete plot series if they do.
			for(int ci=0; ci<scan->dataSourceCount(); ci++) {
				QString dataSourceName = scan->dataSourceAt(ci)->name();
				if(dataSource2Plot_.contains(dataSourceName) && sourceAndScan2PlotItem_[dataSourceName].contains(scan)) {
					// unnecessary: dataSource2Plot_[dataSourceName]->plot()->removeItem(sourceAndScan2PlotItem_[dataSourceName][scan]);
					delete sourceAndScan2PlotItem_[dataSourceName][scan];
					sourceAndScan2PlotItem_[dataSourceName].remove(scan);
					// note: checking whether a plot for this dataSource should still exist (in dataSource2Plot_) will be done after the deletion is finished, inside reviewSources() called by onRowRemoved().
				}
			}

		}
	}


	// removing data source(s).     parent.row() is the scanIndex, and start - end are the data source indexes.
	else if(parent.internalId() == -1) {

		int si = parent.row();
		AMScan* scan = model()->scanAt(si);

		for(int di = end; di>=start; di--) {
			QString sourceName = model()->dataSourceAt(si, di)->name();
			if(dataSource2Plot_.contains(sourceName) && sourceAndScan2PlotItem_[sourceName].contains(scan)) {
				// unnecessary: dataSource2Plot_[sourceName]->plot()->removeItem(sourceAndScan2PlotItem_[sourceName][scan]);
				delete sourceAndScan2PlotItem_[sourceName][scan];
				sourceAndScan2PlotItem_[sourceName].remove(scan);
				// note: checking whether a plot for this data source should still exist (in dataSource2Plot_) will be done after the deletion is finished, inside reviewSources() called by onRowRemoved().
			}
		}
	}
}



// after scans or data sources are deleted in the model:
void AMScanViewMultiSourcesView::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(reviewDataSources())
		reLayout();
}

// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiSourcesView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	// changes to a scan:
	if(topLeft.internalId() == -1) {
		/// \todo What if there's changes to the scan's name and number? (This applies to all AMScanViewInternal subclasses, not just this one.)
		// here specifically, if the scan name changes, we need to go through and change the descriptions of all the plot items that represent that scan.

	}

	// changes to one of our data sources: internalId is the scanIndex.  Data source index is from topLeft.row to bottomRight.row
	else if((unsigned)topLeft.internalId() < (unsigned)model()->scanCount()) {

		// handling visibility changes: if a data source has been turned off (or on), and it was the only source of its kind, we'll need to delete/add a plot.
		/// \todo this is expensive... Would be nice to know _what_ changed (since simply changing the color of a line shouldn't require this whole process.  Add specific visibility-change signalling, to make this more efficient.
		if(reviewDataSources())
			reLayout();

		// apply possible line pen and color changes:
		int si = topLeft.internalId();
		AMScan* scan = model()->scanAt(si);

		for(int di=topLeft.row(); di<=bottomRight.row(); di++) {

			QString sourceName = model()->scanAt(si)->dataSourceAt(di)->name();

			if(dataSource2Plot_.contains(sourceName) && sourceAndScan2PlotItem_[sourceName].contains(scan)) {
				MPlotItem* plotItem = sourceAndScan2PlotItem_[sourceName][scan];

				switch(model()->dataSourceAt(si, di)->rank()) {
				case 1: {
					QPen pen = model()->plotPen(si, di);
					pen.setColor(model()->plotColor(si, di));
					static_cast<MPlotAbstractSeries*>(plotItem)->setLinePen(pen);
					break; }
				case 2: {
					/// \todo This is inefficient... Institute separate signals for different dataChanged() parameters, or check if old color map matches new color map.
					static_cast<MPlotAbstractImage*>(plotItem)->setColorMap(model()->plotColorMap(si, di));
					break; }
				default:
					break;
				}
			}
		}
	}
}




// re-do the layout
void AMScanViewMultiSourcesView::reLayout() {

	for(int li=0; li<layout_->count(); li++)
		layout_->removeAt(li);

	int rc=0, cc=0, width = sqrt(dataSource2Plot_.count());

	foreach(MPlotGW* plot, dataSource2Plot_) {
		layout_->addItem(plot, rc, cc++, Qt::AlignCenter);
		if(cc == width) {
			rc++;
			cc = 0;
		}
	}

	if(dataSource2Plot_.isEmpty())
		layout_->addItem(firstPlot_, 0, 0, Qt::AlignCenter);
}


// The following comment is old and no longer correct.  We no longer care about maintaining the same ordering as model()->visibleChannelNames(). We now maintain a QMap of plots indexed by channelName, instead of a list of plots ordered the same as visibleChannelNames().

// when full list of channels in the model changes, we need to sync/match up our channel lists (channelNames_ and plots_) with the model list.
/* Here's what changes could have happened in the model list:
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
bool AMScanViewMultiSourcesView::reviewDataSources() {

	QSet<QString> modelSources = model()->visibleDataSourceNames().toSet();
	QSet<QString> ourSources = dataSource2Plot_.keys().toSet();
	QSet<QString> deleteTheseSources(ourSources);
	deleteTheseSources -= modelSources;

	QSet<QString> addTheseSources(modelSources);
	addTheseSources -= ourSources;

	// now, deleteTheseSources is a set of the source names we have as plots (but shouldn't have)
	// and, addTheseSources is a set of the source names we don't have plots for (but should have)

	// if there's anything in deleteTheseSources or in addTheseSources, we'll need to re-layout the plots
	bool areChanges = ( deleteTheseSources.count() || addTheseSources.count() );

	// delete the source plots that don't belong:
	foreach(QString sourceName, deleteTheseSources) {
		// delete the plot (will also delete any MPlotSeries within it)

		// WAIT: can't delete if it's the last/only plot. Instead, need to remove/delete all series from it and mark it as empty.
		if(dataSource2Plot_.count() == 1) {
			foreach(MPlotItem* plotItem, sourceAndScan2PlotItem_[sourceName]) {
				//unnecessary: dataSource2Plot_[sourceName]->plot()->removeItem(series);
				delete plotItem;
			}

			firstPlot_ = dataSource2Plot_[sourceName];
			firstPlotEmpty_ = true;
		}
		else
			delete dataSource2Plot_[sourceName];

		// remove pointer to deleted plot, and pointers to deleted series
		dataSource2Plot_.remove(sourceName);
		sourceAndScan2PlotItem_.remove(sourceName);
	}

	// add the source plots that do belong:
	foreach(QString sourceName, addTheseSources) {

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
			newPlot->plot()->enableAutoScale(MPlotAxis::Bottom | MPlotAxis::Left | MPlotAxis::Right);
			newPlot->plot()->axisBottom()->showAxisName(false);
			newPlot->plot()->axisLeft()->showAxisName(false);

			newPlot->plot()->enableAxisNormalizationLeft(normalizationEnabled_, normMin_, normMax_);
			if(waterfallEnabled_)
				newPlot->plot()->setWaterfallLeft(waterfallOffset_);
		}

		dataSource2Plot_.insert(sourceName, newPlot);
		sourceAndScan2PlotItem_.insert(sourceName, QHash<AMScan*, MPlotItem*>());
		newPlot->plot()->legend()->setTitleText(sourceName);
	}


	// now... for each source plot, add any series that are missing. Also need to remove any series that have had their visibility turned off...
	foreach(QString sourceName, modelSources) {

		// remove any existing series, that have had their visibility turned off...
		foreach(AMScan* scan, sourceAndScan2PlotItem_[sourceName].keys()) {
			int si = model()->indexOf(scan);
			int di = scan->indexOfDataSource(sourceName);
			// if not visible, remove and delete series
			if(!model()->isVisible(si, di)) {
				//unnecessary: dataSource2Plot_[sourceName]->plot()->removeItem(sourceAndScan2PlotItem_[sourceName][scan]);
				delete sourceAndScan2PlotItem_[sourceName].take(scan);
			}
		}

		// loop through all scans, adding series to this plot if required...
		for(int si=0; si<model()->scanCount(); si++) {
			AMScan* scan = model()->scanAt(si);

			int di = scan->indexOfDataSource(sourceName);

			// if this scan contains this data source, and it's visible, and we don't have a series for it yet... make and add the new series
			if(di >= 0 && model()->isVisible(si, di) && !sourceAndScan2PlotItem_[sourceName].contains(scan)) {

				MPlotItem* newItem = createPlotItemForDataSource(scan->dataSourceAt(di), model()->plotSettings(si, di));
				if(newItem) {
					newItem->setDescription(scan->fullName());
					dataSource2Plot_[sourceName]->plot()->addItem(newItem);
					// zzzzzzzz Always add, even if 0? (requires checking everywhere for null plot items). Or only add if valid? (Going with later... hope this is okay, in event someone tries at add 0d, 3d or 4d data source.
					sourceAndScan2PlotItem_[sourceName].insert(scan, newItem);
				}
			}
		}

		// only show the detailed legend (with the scan names) if there's more than one scan open. If there's just one, it's kinda obvious, so don't waste the space.
		if(model()->scanCount() > 1)
			dataSource2Plot_[sourceName]->plot()->legend()->enableDefaultLegend(true);
		else
			dataSource2Plot_[sourceName]->plot()->legend()->enableDefaultLegend(false);
	}

	return areChanges;
}


void AMScanViewMultiSourcesView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	if(firstPlotEmpty_)
		firstPlot_->plot()->enableAxisNormalizationLeft(normalizationOn, min, max);
	QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
	while(i.hasNext()) {
		i.next();
		i.value()->plot()->enableAxisNormalizationLeft(normalizationOn, min, max);
	}
}

void AMScanViewMultiSourcesView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	if(waterfallOn) {
		if(firstPlotEmpty_)
			firstPlot_->plot()->setWaterfallLeft(waterfallOffset_);
		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->setWaterfallLeft(waterfallOffset_);
		}
	}
	else {
		if(firstPlotEmpty_)
			firstPlot_->plot()->setWaterfallLeft(0);
		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->setWaterfallLeft(0);
		}
	}
}

void AMScanViewMultiSourcesView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_) {
		if(firstPlotEmpty_)
			firstPlot_->plot()->setWaterfallLeft(offset);

		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->setWaterfallLeft(offset);
		}
	}
}

