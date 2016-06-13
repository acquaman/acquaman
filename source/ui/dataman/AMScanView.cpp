/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMScanView.h"
#include <QGraphicsWidget>
#include "dataman/AMScan.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMDataSourceImageData.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotTools.h"
#include <QScrollBar>
#include "ui/dataman/AMColoredTextToolButton.h"
#include "util/AMErrorMonitor.h"

#include <QAction>
#include <QGroupBox>
#include <QStringBuilder>
#include <QPrintDialog>

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
			scanBars_.takeLast()->deleteLater();
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
			scanBars_.takeAt(si)->deleteLater();
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

#include <QDoubleSpinBox>
#include <QApplication>
#include <QStyle>

 AMScanViewModeBar::~AMScanViewModeBar(){}
AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QFrame(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->setSpacing(0);

	QStyle* style = QApplication::style();

	QToolButton* tabButton = new QToolButton();
	tabButton->setIcon(style->standardIcon(QStyle::SP_FileDialogInfoView));
	tabButton->setText("1");
	tabButton->setToolTip("Show single data source");
	QToolButton* overplotButton_ = new QToolButton();
	overplotButton_->setIcon(style->standardIcon(QStyle::SP_FileDialogDetailedView));
	overplotButton_->setText("OP");
	overplotButton_->setToolTip("Show all data sources");
	QToolButton* multiScansButton_ = new QToolButton();
	multiScansButton_->setIcon(style->standardIcon(QStyle::SP_FileDialogListView));
	multiScansButton_->setText("M-S");
	multiScansButton_->setToolTip("Separate plots per scan");
	QToolButton* multiSourcesButton = new QToolButton();
	multiSourcesButton->setIcon(style->standardIcon(QStyle::SP_FileDialogListView));
	multiSourcesButton->setText("M-C");
	multiSourcesButton->setToolTip("Separate plots per data source");

	tabButton->setCheckable(true);
	overplotButton_->setCheckable(true);
	multiScansButton_->setCheckable(true);
	multiSourcesButton->setCheckable(true);

	modeButtons_ = new QButtonGroup(this);
	modeButtons_->addButton(tabButton, AMScanView::Tabs);
	modeButtons_->addButton(overplotButton_, AMScanView::OverPlot);
	modeButtons_->addButton(multiScansButton_, AMScanView::MultiScans);
	modeButtons_->addButton(multiSourcesButton, AMScanView::MultiSources);
	tabButton->setChecked(true);

	hl2->addWidget(tabButton);
	hl2->addWidget(overplotButton_);
	hl2->addWidget(multiScansButton_);
	hl2->addWidget(multiSourcesButton);

	hl->addLayout(hl2);
	hl->addStretch(1);

	showSourcesButton_ = new QToolButton();
	showSourcesButton_->setIcon(QIcon(":/22x22/view-list-details-symbolic.png"));
	showSourcesButton_->setToolTip("Show or hide the data source visibility buttons");
	showSourcesButton_->setCheckable(true);
	showSourcesButton_->setChecked(true);

	hl->addWidget(showSourcesButton_);
	hl->addStretch(1);

	logCheckBox_ = new QCheckBox("Log Scale");
	logCheckBox_->setChecked(false);
	hl->addWidget(logCheckBox_);
	normalizationCheckBox_ = new QCheckBox("Show at same scale");
	normalizationCheckBox_->setChecked(true);
	hl->addWidget(normalizationCheckBox_);
	waterfallCheckBox_ = new QCheckBox("Waterfall:");
	waterfallCheckBox_->setChecked(true);
	hl->addWidget(waterfallCheckBox_);
	waterfallAmount_ = new QDoubleSpinBox();
	waterfallAmount_->setMinimum(0);
	waterfallAmount_->setMaximum(1e6);
	waterfallAmount_->setValue(0.2);
	waterfallAmount_->setSingleStep(0.1);
	hl->addWidget(waterfallAmount_);
	showSpectra_ = new QCheckBox("Show Spectra");
	showSpectra_->setChecked(false);
	showSpectra_->setEnabled(false);
	showSpectra_->setVisible(false);
	hl->addWidget(showSpectra_);

	hl->setMargin(6);
	setLayout(hl);

	setObjectName("AMScanViewModeBar");
	/*setStyleSheet("QFrame#AMScanViewModeBar { "
  "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
  "border-bottom: 1px solid black;"
  "}");*/

	connect(logCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(logScaleEnabled(bool)));
	connect(normalizationCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(normalizationEnabled(bool)));
	connect(waterfallCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(waterfallOffsetEnabled(bool)));
	connect(waterfallAmount_, SIGNAL(valueChanged(double)), this, SIGNAL(waterfallOffsetChanged(double)));
}

AMScanView::AMScanView(AMScanSetModel* model, QWidget *parent) :
	QWidget(parent)
{
	mode_ = Invalid;
	spectrumViewIsVisible_ = false;

	scansModel_ = model;
	if(scansModel_ == 0)
		scansModel_ = new AMScanSetModel(this);

	setupUI();
	makeConnections();

	scanBars_->setModel(scansModel_);

	if(scansModel_->scanCount() >= AM_SCAN_VIEW_HIDE_SCANBARS_AFTER_N_SCANS)
		setScanBarsVisible(false);

	changeViewMode(Tabs);
}


AMScanView::~AMScanView() {
	for(int i=0; i<views_.count(); i++)
		views_.at(i)->deleteLater();
}

void AMScanView::setupUI() {

	QVBoxLayout* vl = new QVBoxLayout();
	vl->setMargin(6);
	vl->setSpacing(0);

	toolsView_ = new AMScanViewPlotToolsView(0);

	vl->addWidget(toolsView_);

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
		v->enableLogScale(false);
		v->enableNormalization(true);
		v->setWaterfallOffset(0.2);
		v->enableWaterfallOffset(true);
		glayout_->addItem(v);
	}

	spectrumView_ = new AMScanViewSingleSpectrumView(this);

	spectrumViewBox_ = new QGroupBox;
	QHBoxLayout *spectrumViewBoxLayout = new QHBoxLayout;
	spectrumViewBoxLayout->addWidget(spectrumView_);
	spectrumViewBox_->setLayout(spectrumViewBoxLayout);
	spectrumViewBox_->setWindowTitle("View Single Spectrum");
}

void AMScanView::setSpectrumViewVisibility(bool visible)
{
	spectrumViewIsVisible_ = visible;
	spectrumViewBox_->setVisible(spectrumViewIsVisible_);
}

void AMScanView::setUnitsFromScan(AMScan *scan)
{
	QString xUnits = "";

	if (scan && scan->rawData() && scan->rawData()->scanAxesCount() >= 1) {
		xUnits = scan->rawData()->scanAxisAt(0).units;
	}

	foreach (AMScanViewInternal *scanView, views_) {
		if (scanView && scanView->tools()) {
			foreach (MPlotAbstractTool *tool, scanView->tools()->tools()) {

				// If one of the views' tools is a position tool, update the units.

				if (tool) {
					MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);
					if (positionTool) {
						positionTool->setUnits(QStringList() << xUnits);
					}
				}
			}
		}
	}
}

void AMScanView::onDataPositionChanged(const QPointF &point)
{
	AMnDIndex index = getIndex(point);

	if (modeBar_->showSpectra_)
		spectrumView_->onDataPositionChanged(index);
}

void AMScanView::setEnergyRange(double low, double high)
{
	spectrumView_->setEnergyRange(low, high);
}

void AMScanView::addSingleSpectrumEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	spectrumView_->addEmissionLineNameFilter(newNameFilter);
}

void AMScanView::removeSingleSpectrumEmissionLineNameFilter(int index)
{
	spectrumView_->removeEmissionLineNameFilter(index);
}

void AMScanView::removeSingleSpectrumEmissionLineNameFilter(const QRegExp &filter)
{
	spectrumView_->removeEmissionLineNameFilter(filter);
}

void AMScanView::addSingleSpectrumPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	spectrumView_->addPileUpPeakNameFilter(newNameFilter);
}

void AMScanView::removeSingleSpectrumPileUpPeakNameFilter(int index)
{
	spectrumView_->removePileUpPeakNameFilter(index);
}

void AMScanView::removeSingleSpectrumPileUpPeakNameFilter(const QRegExp &filter)
{
	spectrumView_->removePileUpPeakNameFilter(filter);
}

void AMScanView::addSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	spectrumView_->addCombinationPileUpPeakNameFilter(newNameFilter);
}

void AMScanView::removeSingleSpectrumCombinationPileUpPeakNameFilter(int index)
{
	spectrumView_->removeCombinationPileUpPeakNameFilter(index);
}

void AMScanView::removeSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &filter)
{
	spectrumView_->removeCombinationPileUpPeakNameFilter(filter);
}

void AMScanView::setSingleSpectrumDataSource(const QString &name)
{
	spectrumView_->setDataSourceByName(name);
}

void AMScanView::setPlotCursorCoordinates(const QPointF &newCoordinates)
{
	foreach (AMScanViewInternal *view, views_) {
		if (view && view->tools()) {
			foreach (MPlotAbstractTool *tool, view->tools()->tools()) {
				MPlotDataPositionCursorTool *cursorTool = qobject_cast<MPlotDataPositionCursorTool*>(tool);
				if (cursorTool)
					cursorTool->setCursorPosition(newCoordinates);
			}
		}
	}
}

void AMScanView::setPlotCursorCoordinates(AMNumber newCoordinate)
{
	if (newCoordinate.isValid())
		setPlotCursorCoordinates(double(newCoordinate));
}

void AMScanView::setPlotCursorCoordinates(double newCoordinate)
{
	foreach (AMScanViewInternal *view, views_) {
		if (view && view->tools()) {
			foreach (MPlotAbstractTool *tool, view->tools()->tools()) {
				MPlotDataPositionCursorTool *cursorTool = qobject_cast<MPlotDataPositionCursorTool*>(tool);
				if (cursorTool)
					cursorTool->setCursorPositionX(newCoordinate);
			}
		}
	}
}

void AMScanView::setPlotCursorVisibility(bool isVisible)
{
	foreach (AMScanViewInternal *view, views_) {
		if (view && view->tools()) {
			foreach (MPlotAbstractTool *tool, view->tools()->tools()) {
				MPlotDataPositionCursorTool *cursorTool = qobject_cast<MPlotDataPositionCursorTool*>(tool);
				if (cursorTool) {
					if (isVisible) {
						view->tools()->addSelectedTool(cursorTool);
						cursorTool->setCursorVisibility(isVisible);
					} else {
						view->tools()->removeSelectedTool(cursorTool);
						cursorTool->setCursorVisibility(isVisible);
					}
				}
			}
		}
	}
}

void AMScanView::setAxisInfoForSpectrumView(const AMAxisInfo &info, bool propogateToPlotRange)
{
	spectrumView_->setAxisInfo(info, propogateToPlotRange);
}

// newMode:
void AMScanView::changeViewMode(int newMode) {

	// nothing changing:
	if(newMode == mode_)
		return;

	if(newMode < 0 || newMode >= views_.count())
		return;

	// Hide all views.

	toolsView_->setTools(0);

	for (int i = 0, count = views_.size(); i< count; i++)
		views_.at(i)->hide();

	// Identify and show the desired view.

	mode_ = (ViewMode)newMode;
	scanBars_->setExclusiveModeOn( (mode_ == Tabs) );

	toolsView_->setTools(views_.at(mode_)->tools());

	views_.at(mode_)->show();
	resizeViews();

	// in case this was called programmatically (instead of by clicking on the button)... the mode button won't be set.  This will re-emit the mode-change signal, but this function will exit immediately on the second time because it's already in the correct mode.
	modeBar_->modeButtons_->button(mode_)->setChecked(true);
}

void AMScanView::makeConnections() {
	// connect mode bar to changeViewMode:
	connect(modeBar_->modeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(changeViewMode(int)));

	// connect resize event from graphicsView to resize the stuff inside the view
	connect(gview_, SIGNAL(resized(QSizeF)), this, SLOT(resizeViews()), Qt::QueuedConnection);

	// connect the "show scan bars" button to show/hide them
	connect(modeBar_->showSourcesButton_, SIGNAL(clicked(bool)), scanBars_, SLOT(setVisible(bool)));
	connect(modeBar_->showSpectra_, SIGNAL(toggled(bool)), this, SLOT(setSpectrumViewVisibility(bool)));
	connect((AMScanViewExclusiveView *)views_.first(), SIGNAL(dataPositionChanged(QPointF)), this, SLOT(onDataPositionChanged(QPointF)));

	foreach (AMScanViewInternal *view, views_)
		connect(view, SIGNAL(dataPositionChanged(QPointF)), this, SIGNAL(dataPositionChanged(QPointF)));

	connect(scansModel_, SIGNAL(scanAdded(AMScan*)), this, SLOT(onScanAdded(AMScan*)));
	connect(scansModel_, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));

	// connect enabling/disabling normalization and waterfall to each view
	for(int i=0; i<views_.count(); i++) {
		AMScanViewInternal* v = views_.at(i);
		connect(modeBar_, SIGNAL(logScaleEnabled(bool)), v, SLOT(enableLogScale(bool)));
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

	gview_->graphicsWidget()->setGeometry(QRectF(pos, mainWidgetSize));
}

/// \todo: should scans held in the view be const or non-const?

void AMScanView::addScan(AMScan *newScan) {
	scansModel_->addScan(newScan);
	// that's it!  handling the rowsAdded, rowsRemoved signals from the model will take care of everything else.
}

void AMScanView::onScanAdded(AMScan *scan)
{
	QList<AMDataSource *> sources;

	int scanRank = scan->scanRank();

	foreach (AMDataSource *source, scan->rawDataSources()->toList())
		if (source->rank()-scanRank == 1)
			sources.append(source);

	foreach (AMDataSource *source, scan->analyzedDataSources()->toList())
		if (source->rank()-scanRank == 1)
			sources.append(source);

	modeBar_->showSpectra_->setEnabled(!sources.isEmpty());
	modeBar_->showSpectra_->setVisible(!sources.isEmpty());

	// Apply changes to views.

	spectrumView_->setTitle(QString("%1 #%2").arg(scan->name()).arg(scan->number()));
	spectrumView_->setDataSources(sources);

	setUnitsFromScan(scan);
}

AMnDIndex AMScanView::getIndex(const QPointF &point) const
{
	if (point.isNull())
		return AMnDIndex();

	if (scansModel_->scanCount() == 0)
		return AMnDIndex();

	AMnDIndex index;

	switch(scansModel_->scanAt(0)->scanRank()){

	case 0:
		index = AMnDIndex();
		break;

	case 1:{

		AMScan *scan = scansModel_->scanAt(0);
		AMDataSource *datasource = scan->dataSourceAt(scan->indexOfDataSource(scansModel_->exclusiveDataSourceName()));
		int x = -1;
		int size = scan->scanSize(0);

		if (datasource) {

			// First point and last points are special.
			if (fabs(point.x() - double(datasource->axisValue(0, 0))) < fabs(double(datasource->axisValue(0, 1))-double(datasource->axisValue(0, 0))))
				x = 0;

			for (int i = 1, count = size-1; i < count; i++)
				if (fabs(point.x() - double(datasource->axisValue(0, i))) < qMax(fabs(double(datasource->axisValue(0, i+1))-double(datasource->axisValue(0, i))), fabs(double(datasource->axisValue(0, i))-double(datasource->axisValue(0, i-1)))))
					x = i;

			if (fabs(point.x() - double(datasource->axisValue(0, size-1))) < fabs(double(datasource->axisValue(0, size-1))-double(datasource->axisValue(0, size-2))))
				x = size-1;
		}

		index = AMnDIndex(x);
		break;
	}

	case 2:{

		int x = -1;
		int y = -1;
		AMScan *scan = scansModel_->scanAt(0);
		AMDataSource *datasource = scan->dataSourceAt(scan->indexOfDataSource(scansModel_->exclusiveDataSourceName()));

		if (datasource) {
			// For performance (speed) reasons, I have assumed that the axis values are evenly spaced for 2D maps.  This is unlike the 1D case where different spacings are possible and expected.
			// This assumes 2D maps where the size is greater than 1x1, 1xN, or Nx1.
			double delX = (double(datasource->axisValue(0, 1)) - double(datasource->axisValue(0, 0)))/2;
			double delY = (double(datasource->axisValue(1, 1)) - double(datasource->axisValue(1, 0)))/2;

			for (int i = 0, size = scan->scanSize(0); i < size; i++)
				if (fabs(point.x() - double(datasource->axisValue(0, i))) < delX)
					x = i;

			for (int i = 0, size = scan->scanSize(1); i < size; i++)
				if (fabs(point.y() - double(datasource->axisValue(1, i))) < delY)
					y = i;
		}

		index = AMnDIndex(x, y);
		break;
	}
	}

	return index;
}

// remove a scan from the view:
void AMScanView::removeScan(AMScan* scan) {
	scansModel_->removeScan(scan);
}

void AMScanView::showEvent(QShowEvent *e)
{
	if (!spectrumViewBox_->isVisible() && spectrumViewIsVisible_)
		spectrumViewBox_->show();

	QWidget::showEvent(e);
}

void AMScanView::hideEvent(QHideEvent *e)
{
	if (spectrumViewBox_->isVisible())
		spectrumViewBox_->hide();

	QWidget::hideEvent(e);
}

void AMScanView::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
		emit dataPositionChanged(e->globalPos());

	QWidget::mousePressEvent(e);
}

#include <QSizePolicy>

 AMScanViewInternal::~AMScanViewInternal(){ delete tools_;}
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

	logScaleEnabled_ = false;
	normalizationEnabled_ = false;
	waterfallEnabled_ = false;
	waterfallOffset_  = 0;

	tools_ = 0;
}

void AMScanViewInternal::setPlotTools(AMScanViewPlotTools *newTools)
{
	if (tools_ != newTools) {

		if (tools_) {
			disconnect( tools_, 0, this, 0 );
		}

		tools_ = newTools;

		if (tools_) {
			connect( tools_, SIGNAL(selectedToolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(updatePlotTools()) );
		}

		updatePlotTools();

		emit plotToolsChanged(tools_);
	}
}

void AMScanViewInternal::updatePlotTools()
{
	if (tools_)
		applyPlotTools(tools_->selectedTools());
}

AMScanSetModel* AMScanViewInternal::model() const { return masterView_->model(); }

MPlotGW * AMScanViewInternal::createDefaultPlot()
{
	MPlotGW* rv = new MPlotGW();
	rv->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	rv->plot()->axisBottom()->setTicks(4);
	rv->plot()->axisTop()->setTicks(4);
	rv->plot()->axisLeft()->showGrid(false);
	rv->plot()->axisScaleLeft()->setAutoScaleEnabled();
	rv->plot()->axisScaleBottom()->setAutoScaleEnabled();
	rv->plot()->axisScaleRight()->setAutoScaleEnabled();

	rv->plot()->axisBottom()->showAxisName(true);
	rv->plot()->axisLeft()->showAxisName(false);
	rv->plot()->axisRight()->showAxisName(true);

	rv->plot()->setMarginRight(rv->plot()->marginLeft());

	return rv;
}

void AMScanViewInternal::addToolToPlot(MPlot *plot, MPlotAbstractTool *tool)
{
	if (plot && tool) {
		plot->addTool(tool);

		// Most tools just need to be added.
		// If the tool is a data position tool, however, it's axis scales must also be set.

		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);
		if (positionTool) {
			positionTool->setDataPositionIndicator(plot->axisScaleBottom(), plot->axisScaleLeft());
		}
	}
}

void AMScanViewInternal::removeToolFromPlot(MPlot *plot, MPlotAbstractTool *tool)
{
	if (plot && tool && plot->tools().contains(tool)) {
		plot->removeTool(tool);

		// Most tools can just be removed.
		// If the tool is a data position tool, however, it's axis scales must also be set to 0.

		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);

		if (positionTool) {
			positionTool->setDataPositionIndicator(0, 0);
		}
	}
}

void AMScanViewInternal::removeToolsFromPlot(MPlot *plot)
{
	if (plot)
		plot->removeTools();
}

// Helper function to create an appropriate MPlotItem and connect it to the data, depending on the dimensionality of \c dataSource.  Returns 0 if we can't handle this dataSource and no item was created (ex: unsupported dimensionality, we only handle 1D or 2D data for now.)
MPlotItem* AMScanViewInternal::createPlotItemForDataSource(const AMDataSource* dataSource, const AMDataSourcePlotSettings& plotSettings) {
	MPlotItem* rv = 0;

	if(dataSource == 0) {
		AMErrorMon::debug(this, AMSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_NULL_DATA_SOURCE, "Asked to create a plot item for a null data source.");
		return 0;
	}

	switch(dataSource->rank()) {	// depending on the rank, we'll need an XY-series or an image to display it. 3D and 4D, etc. we don't handle for now.

	case 1: {
		MPlotSeriesBasic* series = new MPlotSeriesBasic();
		series->setModel(new AMDataSourceSeriesData(dataSource), true);
		series->setMarker(plotSettings.markerShape, 6, QPen(plotSettings.markerColor));
		series->setLinePen(plotSettings.linePen);
		rv = series;
		break; }

	case 2: {
		MPlotImageBasic* image = new MPlotImageBasic();
		AMDataSourceImageData *model = new AMDataSourceImageData;
		model->setDataSource(dataSource);
		image->setModel(model, true);
		image->setColorMap(plotSettings.colorMap);
		image->setZValue(-1000);
		rv = image;
		break; }
	default:
		AMErrorMon::alert(this, AMSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_UNHANDLED_RANK, QString("Asked to create a plot item for a rank that we don't handle. Source name is: %1").arg(dataSource->name()));
		rv = 0;
		break;
	}

	return rv;
}

// Helper function to look at a plot and configure the left and right axes depending on whether there are 1D and/or 2D data sources in the plot.
/* We plot 2D data sources on the right axis scale, and 1D data sources on the left axis scale.  We could therefore end up in one of 4 states for the axes configuration:
0: No data sources of any kind: hide both axis labels; assign both axes to the left axis scale.
1: Only 1D data sources: show the left axis labels, hide the right axis labels; assign both axes to the left axis scale.
2: Only 2D data sources: hide the left axis labels, show the right axis labels; assign both axes to the right axis scale.
3: Both 1D and 2D data sources: show the left and right axis labels; assign the left axis to the left axis scale and the right axis to the right axis scale.
  */
void AMScanViewInternal::reviewPlotAxesConfiguration(MPlotGW *plotGW)
{
	MPlot* plot = plotGW->plot();
	bool has1Dsources = (plot->seriesItemsCount() != 0);
	bool has2Dsources = (plot->imageItemsCount() != 0);

	// No data sources of any kind: hide both axis labels, assign both axes to the left axis scale.
	if(!has1Dsources && !has2Dsources) {
		plot->axisLeft()->showTickLabels(false);
		plot->axisLeft()->setAxisScale(plot->axisScaleLeft());
		plot->axisRight()->showTickLabels(false);
		plot->axisRight()->setAxisScale(plot->axisScaleLeft());
	}
	// Only 1D data sources: show the left axis labels, hide the right axis labels, assign both axes to the left axis scale.
	else if(has1Dsources && !has2Dsources) {
		plot->axisLeft()->showTickLabels(true);
		plot->axisLeft()->setAxisScale(plot->axisScaleLeft());
		plot->axisRight()->showTickLabels(false);
		plot->axisRight()->setAxisScale(plot->axisScaleLeft());
	}
	// Only 2D data sources: hide the left axis labels, show the right axis labels, assign both axes to the right axis scale.
	else if(!has1Dsources && has2Dsources) {
		plot->axisLeft()->showTickLabels(false);
		plot->axisLeft()->setAxisScale(plot->axisScaleRight());
		plot->axisRight()->showTickLabels(true);
		plot->axisRight()->setAxisScale(plot->axisScaleRight());
	}
	// Both 1D and 2D data sources: show the left and right axis labels, assign the left axis to the left axis scale and the right axis to the right axis scale.
	else if(has1Dsources && has2Dsources) {
		plot->axisLeft()->showTickLabels(true);
		plot->axisLeft()->setAxisScale(plot->axisScaleLeft());
		plot->axisRight()->showTickLabels(true);
		plot->axisRight()->setAxisScale(plot->axisScaleRight());
	}
}

// AMScanViewExclusiveView
/////////////////////////////

AMScanViewExclusiveView::AMScanViewExclusiveView(AMScanView* masterView) : AMScanViewInternal(masterView)
{
	// Create our main plot:

	plot_ = createDefaultPlot();

	// Create instances of the tools we want available and add them to the tool options.

	selectorTool_ = new MPlotPlotSelectorTool();
	dragZoomerTool_ = new MPlotDragZoomerTool();
	wheelZoomerTool_ = new MPlotWheelZoomerTool();
	dataPositionTool_ = new MPlotDataPositionCursorTool(false);

	AMScanViewPlotTools *tools = new AMScanViewPlotTools(QList<MPlotAbstractTool*>());
	tools->setExclusiveSelectionEnabled(true);
	tools->setTools(QList<MPlotAbstractTool*>() << selectorTool_ << dataPositionTool_ << dragZoomerTool_ << wheelZoomerTool_);
	tools->setSelectedTools(QList<MPlotAbstractTool*>() << dragZoomerTool_);

	setPlotTools(tools);

	// Set layout.

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

	// Make connections.

	connect(model(), SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));
	connect( plot_->plot()->signalSource(), SIGNAL(dataPositionChanged(QPointF)), this, SIGNAL(dataPositionChanged(QPointF)) );

	reviewPlotAxesConfiguration(plot_);
	refreshTitle();
}

AMScanViewExclusiveView::~AMScanViewExclusiveView() {

	// Tools that are currently selected (ie applied to the plot) will be deleted when
	// the plot is deleted. Must delete the rest of the tools here.

	QList<MPlotAbstractTool*> selectedTools = tools()->selectedTools();

	if (!selectedTools.contains(selectorTool_))
		selectorTool_->deleteLater();

	if (!selectedTools.contains(dragZoomerTool_))
		dragZoomerTool_->deleteLater();

	if (!selectedTools.contains(wheelZoomerTool_))
		wheelZoomerTool_->deleteLater();

	if (!selectedTools.contains(dataPositionTool_))
		dataPositionTool_->deleteLater();

	// Delete the plot.

	plot_->deleteLater();
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

	reviewPlotAxesConfiguration(plot_);
	refreshTitle();
}
// before scans or data sources is deleted in the model:
void AMScanViewExclusiveView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing a scan: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int i=end; i>=start; i--) {
			if(plotItems_.at(i)) {
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

	reviewPlotAxesConfiguration(plot_);
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
					MPlotAbstractSeries* series = static_cast<MPlotAbstractSeries*>(plotItems_.at(si));
					QPen pen = model()->plotPen(si, di);
					if(pen != series->linePen())
						series->setLinePen(pen);
					MPlotMarkerShape::Shape shape = model()->markerShape(si, di);
					QColor markerColor = model()->markerColor(si, di);
					series->setMarker(shape, 6, QPen(markerColor));
					break; }
				case 2: {
					MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItems_.at(si));
					MPlotColorMap newMap = model()->plotColorMap(si, di);
					if(newMap != image->colorMap())
						image->setColorMap(newMap);
					break;
				}
				default:
					break;
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

	reviewPlotAxesConfiguration(plot_);
	refreshTitle();
}

void AMScanViewExclusiveView::applyPlotTools(const QList<MPlotAbstractTool*> &newSelection)
{
	// Clear the plot of previously added tools.

	removeToolsFromPlot(plot_->plot());

	// Add tools to the plot, according to selection.

	foreach (MPlotAbstractTool *tool, newSelection) {
		addToolToPlot(plot_->plot(), tool);
	}
}

void AMScanViewExclusiveView::setPlotCursorVisibility(bool isVisible)
{
	if (isVisible && !tools_->isSelectedTool(dataPositionTool_)) {
		tools_->addSelectedTool(dataPositionTool_);

	} else if (!isVisible && tools_->isSelectedTool(dataPositionTool_)) {
		tools_->removeSelectedTool(dataPositionTool_);
	}
}

void AMScanViewExclusiveView::refreshTitle() {

	int numScansShown = 0;
	foreach(MPlotItem* s, plotItems_)
		if(s)
			numScansShown++;

	QString scanCount = (numScansShown == 1) ? " (1 scan)" : QString(" (%1 scans)").arg(numScansShown);
	/// \todo This should show a data source description, not a name. However, we don't know that all the descriptions (for all the data sources matching the exclusive data source) are the same....
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
		plot_->plot()->addItem(newItem, (dataSource->rank() == 2 ? MPlot::Right : MPlot::Left));
	}
	plotItems_.insert(scanIndex, newItem);
	plotItemDataSources_.insert(scanIndex, dataSource);
}



// testing: #include "ui/dataman/AM3dDataSourceView.h"

// Helper function to review a scan when a data source is added or the exclusive data source changes.
void AMScanViewExclusiveView::reviewScan(int scanIndex) {

	QString dataSourceName = model()->exclusiveDataSourceName();

	int dataSourceIndex = model()->scanAt(scanIndex)->indexOfDataSource(dataSourceName);
	AMDataSource* dataSource = model()->dataSourceAt(scanIndex, dataSourceIndex);

	// does this scan have the "exclusive" data source in it?
	if(dataSource) {


		// the current plot item exists, but it's the wrong type to handle the current scan data. (ie: dataSource->rank() is 2, but we've currently got a plot series instead of a plot image.
		if(plotItems_.at(scanIndex) && plotItems_.at(scanIndex)->rank() != dataSource->rank() ) {
			// delete the plot item, we'll recreate the new one of the proper size in the next check.
			delete plotItems_.at(scanIndex);
			plotItems_[scanIndex] = 0;
			plotItemDataSources_[scanIndex] = 0;
		}

		// need to create new plot item for this scan. (Don't have one yet)
		if(plotItems_.at(scanIndex) == 0) {
			MPlotItem* newItem;
			plotItems_[scanIndex] = newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(scanIndex, dataSourceIndex));
			if(newItem) {
				newItem->setDescription(model()->scanAt(scanIndex)->fullName());
				plot_->plot()->addItem(newItem, (dataSource->rank() == 2? MPlot::Right : MPlot::Left));
				AMScan *scan = model()->scanAt(scanIndex);

				plot_->plot()->axisBottom()->setAxisName(bottomAxisName(scan, dataSource));
				plot_->plot()->axisRight()->setAxisName(rightAxisName(scan, dataSource));

			}
			/// \todo: if there are 2d images on any plots, set their right axis to show the right axisScale, and show ticks.

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
				series->setLinePen(pen);
				MPlotMarkerShape::Shape shape = model()->markerShape(scanIndex, dataSourceIndex);
				QColor markerColor = model()->markerColor(scanIndex, dataSourceIndex);
				series->setMarker(shape, 6, QPen(markerColor));
				break; }
			case 2: {
				MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItems_.at(scanIndex));
				if(plotItemDataSources_.at(scanIndex) != dataSource) {
					AMDataSourceImageData* newData = new AMDataSourceImageData;
					newData->setDataSource(dataSource);
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

void AMScanViewExclusiveView::enableLogScale(bool logScaleOn)
{
	AMScanViewInternal::enableLogScale(logScaleOn);

	setDataRangeConstraint(MPlot::Left);
	plot_->plot()->enableLogScale(MPlot::Left, logScaleOn);
}

void AMScanViewExclusiveView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	setDataRangeConstraint(MPlot::Left);
	plot_->plot()->enableAxisNormalization(MPlot::Left, normalizationOn, MPlotAxisRange(min, max));

}

void AMScanViewExclusiveView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	if(waterfallOn)
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);
	else
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, 0);

}

void AMScanViewExclusiveView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_)
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);

}

void AMScanViewExclusiveView::setDataRangeConstraint(int id)
{
	switch(id){

	case MPlot::Left:

		if (logScaleEnabled_ && !normalizationEnabled_){

			double min = plot_->plot()->minimumYSeriesValue();

			if (min <= 0)
				plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(min, MPLOT_POS_INFINITY));
			else
				plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));

		}
		else if (logScaleEnabled_ && normalizationEnabled_)
			plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(1e-4, MPLOT_POS_INFINITY));
		else
			plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(MPLOT_NEG_INFINITY, MPLOT_POS_INFINITY));

		break;
	}
}

// AMScanViewMultiView
/////////////////////////////

AMScanViewMultiView::AMScanViewMultiView(AMScanView* masterView) : AMScanViewInternal(masterView)
{
	// create our main plot:

	plot_ = createDefaultPlot();

	// Create instances of the tools we want available and add them to the tool options.

	selectorTool_ = new MPlotPlotSelectorTool();
	dragZoomerTool_ = new MPlotDragZoomerTool();
	wheelZoomerTool_ = new MPlotWheelZoomerTool();

	AMScanViewPlotTools *tools = new AMScanViewPlotTools(QList<MPlotAbstractTool*>());
	tools->setTools(QList<MPlotAbstractTool*>() << selectorTool_ << dragZoomerTool_ << wheelZoomerTool_);
	tools->setSelectedTools(QList<MPlotAbstractTool*>() << dragZoomerTool_);

	setPlotTools(tools);

	connect( tools_, SIGNAL(selectedToolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(applyPlotTools(QList<MPlotAbstractTool*>)) );

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

	// Make connections.

	connect(plot_->plot()->signalSource(), SIGNAL(dataPositionChanged(QPointF)), this, SIGNAL(dataPositionChanged(QPointF)));

	reviewPlotAxesConfiguration(plot_);
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
				plot_->plot()->addItem(newItem, (dataSource->rank() == 2 ? MPlot::Right : MPlot::Left));
				/// \todo: if there are 2d images on any plots, set their right axis to show the right axisScale, and show ticks.

				AMScan *scan = model()->scanAt(si);

				plot_->plot()->axisBottom()->setAxisName(bottomAxisName(scan, dataSource));
				plot_->plot()->axisRight()->setAxisName(rightAxisName(scan, dataSource));

				newItem->setDescription(model()->scanAt(si)->fullName() + ": " + dataSource->name());
			}
			scanList << newItem;
		}
		else // otherwise, append null series
			scanList << 0;
	}
	plotItems_.insert(si, scanList);
}

AMScanViewMultiView::~AMScanViewMultiView()
{
	// Tools that are currently selected (ie applied to the plot) will be deleted when
	// the plot is deleted. Must delete the rest of the tools here.

	QList<MPlotAbstractTool*> selectedTools = tools()->selectedTools();

	if (!selectedTools.contains(dragZoomerTool_))
		dragZoomerTool_->deleteLater();

	if (!selectedTools.contains(wheelZoomerTool_))
		wheelZoomerTool_->deleteLater();

	// Delete the plot.

	plot_->deleteLater();
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

					plot_->plot()->addItem(newItem, (dataSource->rank() == 2 ? MPlot::Right : MPlot::Left));

					/// \todo: if there are 2d images on any plots, set their right axis to show the right axisScale, and show ticks.
				}
				plotItems_[si].insert(di, newItem);
			}
			else // otherwise, append null series
				plotItems_[si].insert(di, 0);
		}
	}

	reviewPlotAxesConfiguration(plot_);
	refreshTitles();
}
// before scans or dataSources are deleted in the model:
void AMScanViewMultiView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// removing scans: (start through end are the scan index)
	if(!parent.isValid()) {
		for(int si=end; si>=start; si--) {
			for(int ci=0; ci<model()->scanAt(si)->dataSourceCount(); ci++) {
				if(plotItems_[si][ci]) {
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

	reviewPlotAxesConfiguration(plot_);
	refreshTitles();
}

// when data changes: (Things we care about: color, linePen, and visible)
void AMScanViewMultiView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	bool plotItemsAddedOrRemoved = false;

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
						plotItemsAddedOrRemoved = true;
						plot_->plot()->addItem(plotItem, (model()->dataSourceAt(si, di)->rank() == 2 ? MPlot::Right : MPlot::Left));

						/// \todo: if there are 2d images on any plots, set their right axis to show the right axisScale, and show ticks.
						plotItem->setDescription(model()->scanAt(si)->fullName() + ": " + model()->dataSourceAt(si, di)->description());
					}
				}
				// otherwise, already have a plot item, just need to update it.
				else {
					plotItem->setDescription(model()->scanAt(si)->fullName() + ": " + model()->dataSourceAt(si, di)->description());
					switch(model()->dataSourceAt(si, di)->rank()) {
					case 1: {
						MPlotAbstractSeries* series = static_cast<MPlotAbstractSeries*>(plotItem);
						QPen pen = model()->plotPen(si, di);
						if(pen != series->linePen())
							series->setLinePen(pen);
						MPlotMarkerShape::Shape shape = model()->markerShape(si, di);
						QColor markerColor = model()->markerColor(si, di);
						series->setMarker(shape, 6, QPen(markerColor));
						break; }
					case 2: {
						MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItem);
						MPlotColorMap newMap = model()->plotColorMap(si, di);
						if(newMap != image->colorMap())
							image->setColorMap(newMap);
						break; }
					default:
						break;
					}
				}
			}


			else {	// shouldn't be visible
				if(plotItem) { // need to get rid of?  (ie: it's not supposed to be visible, but we have a plot item)
					plotItemsAddedOrRemoved = true;
					plot_->plot()->removeItem(plotItem);
					delete plotItem;
					plotItems_[si][di] = 0;
				}
			}
		}
	}


	if(plotItemsAddedOrRemoved)
		reviewPlotAxesConfiguration(plot_);
	refreshTitles();
}

void AMScanViewMultiView::applyPlotTools(const QList<MPlotAbstractTool *> &newSelection)
{
	// Clear the plot of previously added tools.

	removeToolsFromPlot(plot_->plot());

	// Add tools to the plot, according to selection.

	QString selection;

	foreach (MPlotAbstractTool *tool, newSelection) {
		selection += QString(" %1").arg(tool->name());
		addToolToPlot(plot_->plot(), tool);
	}
}

void AMScanViewMultiView::refreshTitles() {

	if(model()->scanCount() == 1)
		plot_->plot()->legend()->setTitleText("1 scan");
	else
		plot_->plot()->legend()->setTitleText(QString("%1 scans").arg(model()->scanCount()));
}

void AMScanViewMultiView::enableLogScale(bool logScaleOn)
{
	AMScanViewInternal::enableLogScale(logScaleOn);

	setDataRangeConstraint(MPlot::Left);
	plot_->plot()->enableLogScale(MPlot::Left, logScaleOn);
}

void AMScanViewMultiView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	setDataRangeConstraint(MPlot::Left);
	plot_->plot()->enableAxisNormalization(MPlot::Left, normalizationOn, min, max);
}

void AMScanViewMultiView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	if(waterfallOn)
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);
	else
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, 0);
}

void AMScanViewMultiView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_)
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, offset);
}

void AMScanViewMultiView::setDataRangeConstraint(int id)
{
	switch(id){

	case MPlot::Left:

		if (logScaleEnabled_ && !normalizationEnabled_){

			double min = plot_->plot()->minimumYSeriesValue();

			if (min <= 0)
				plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(min, MPLOT_POS_INFINITY));
			else
				plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));

		}
		else if (logScaleEnabled_ && normalizationEnabled_)
			plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(1e-4, MPLOT_POS_INFINITY));
		else
			plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(MPLOT_NEG_INFINITY, MPLOT_POS_INFINITY));

		break;
	}
}

// AMScanViewMultiScansView
///////////////////////////////////////////////////

AMScanViewMultiScansView::AMScanViewMultiScansView(AMScanView* masterView) : AMScanViewInternal(masterView)
{

	layout_ = new QGraphicsGridLayout();
	layout_->setContentsMargins(0,0,0,0);
	layout_->setSpacing(0);
	layout_->setSizePolicy(sizePolicy());
	setLayout(layout_);

	// we need to have at least one plot, to fill our widget,  even if there are no scans.
	MPlotGW* plot;
	plot = createDefaultPlot();
	plot->plot()->legend()->enableDefaultLegend(false);	/// \todo Right now we maintain our own legend (instead of using MPlotLegend's automatic one), to keep it sorted by data source order. If you could introduce consistent ordering to MPlotLegend and MPlot::items(), we wouldn't have to.  [done, can insertPlotItem()s instead of addPlotItem()s now.]

	firstPlotEmpty_ = true;
	plots_ << plot;

	// Create instances of the tools we want available and add them to the tool options.

	dragZoomerTool_ = new MPlotDragZoomerTool();
	wheelZoomerTool_ = new MPlotWheelZoomerTool();

	AMScanViewPlotTools *tools = new AMScanViewPlotTools(QList<MPlotAbstractTool*>());
	tools->setTools(QList<MPlotAbstractTool*>() << dragZoomerTool_ << wheelZoomerTool_);
	tools->setSelectedTools(QList<MPlotAbstractTool*>() << dragZoomerTool_);

	setPlotTools(tools);

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
		plot = createDefaultPlot();
		plot->plot()->legend()->enableDefaultLegend(false);

		plot->plot()->enableAxisNormalization(MPlot::Left, normalizationEnabled_,  normMin_,  normMax_);
		if(waterfallEnabled_)
			plot->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);

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
				plots_.at(si)->plot()->addItem(newItem, (dataSource->rank() == 2 ? MPlot::Right : MPlot::Left));

				AMScan *scan = model()->scanAt(si);
				plots_.at(si)->plot()->axisBottom()->setAxisName(bottomAxisName(scan, dataSource));
				plots_.at(si)->plot()->axisRight()->setAxisName(rightAxisName(scan, dataSource));
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

	reviewPlotAxesConfiguration(plots_.at(si));
	refreshLegend(si);


	// note: haven't yet added this new plot to the layout_.  That's up to reLayout()
}

AMScanViewMultiScansView::~AMScanViewMultiScansView()
{
	// Tools that are currently selected (ie applied to the plot) will be deleted when
	// the plot is deleted. Must delete the rest of the tools here.

	QList<MPlotAbstractTool*> selectedTools = tools()->selectedTools();

	if (!selectedTools.contains(dragZoomerTool_))
		dragZoomerTool_->deleteLater();

	if (!selectedTools.contains(wheelZoomerTool_))
		wheelZoomerTool_->deleteLater();

	// Delete the plots.

	for(int pi=0; pi<plots_.count(); pi++)
		plots_.at(pi)->deleteLater();
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
					plots_.at(si)->plot()->addItem(newItem, (dataSource->rank() == 2 ? MPlot::Right : MPlot::Left));
				}
				plots_.at(si)->plot()->addItem(newItem);
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

		reviewPlotAxesConfiguration(plots_.at(si));
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
				plots_.takeAt(si)->deleteLater();
			}
		}
		reLayout();
	}

	// removing data sources. parent.row() is the scanIndex, and start - end are the data source indexes.
	else if(parent.internalId() == -1) {
		int si = parent.row();
		for(int di = end; di>=start; di--) {
			if(plotItems_[si][di]) {
				delete plotItems_[si][di];
			}
			plotItems_[si].removeAt(di);
			plotLegendText_[si].removeAt(di);
		}
		reviewPlotAxesConfiguration(plots_.at(si));
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

		bool visibilityChanges = false;

		int si = topLeft.internalId();
		for(int di=topLeft.row(); di<=bottomRight.row(); di++) {

			// handle visibility changes:
			bool visible = model()->isVisible(si, di);

			MPlotItem* plotItem = plotItems_[si][di];

			if(visible) {
				if(plotItem == 0) { // need to create? (becoming visible)
					plotItems_[si][di] = plotItem = createPlotItemForDataSource(model()->dataSourceAt(si, di), model()->plotSettings(si, di));
					if(plotItem) {
						visibilityChanges = true;
						plotItem->setDescription(model()->dataSourceAt(si, di)->description());
						plots_.at(si)->plot()->addItem(plotItem, (model()->dataSourceAt(si, di)->rank() == 2 ? MPlot::Right : MPlot::Left));
					}
				}
				else {	// apply data changes to existing plot item
					plotItem->setDescription(model()->dataSourceAt(si, di)->description());

					switch(model()->dataSourceAt(si, di)->rank()) {
					case 1: {
						MPlotAbstractSeries* series = static_cast<MPlotAbstractSeries*>(plotItem);
						QPen pen = model()->plotPen(si, di);
						if(pen != series->linePen())
							series->setLinePen(pen);
						MPlotMarkerShape::Shape shape = model()->markerShape(si, di);
						QColor markerColor = model()->markerColor(si, di);
						series->setMarker(shape, 6, QPen(markerColor));
						break; }
					case 2: {
						MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItem);
						MPlotColorMap newMap = model()->plotColorMap(si, di);
						if(newMap != image->colorMap())
							image->setColorMap(newMap);
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
					visibilityChanges = true;
					delete plotItem;
					plotItems_[si][di] = 0;
				}
				plotLegendText_[si][di] = QString(); // if invisible, remove from legend
			}
		}
		if(visibilityChanges)
			reviewPlotAxesConfiguration(plots_.at(si));
		refreshLegend(si);
	}
}

void AMScanViewMultiScansView::applyPlotTools(const QList<MPlotAbstractTool *> &newSelection)
{
	// Clear all plots of previously added tools.

	foreach (MPlotGW *plot, plots_) {
		removeToolsFromPlot(plot->plot());
	}

	// Add tools to the plots, according to selection.

	foreach (MPlotGW *plot, plots_) {
		foreach (MPlotAbstractTool *tool, newSelection) {
			addToolToPlot(plot->plot(), tool);
		}
	}
}

// re-do the layout
void AMScanViewMultiScansView::reLayout() {

	for(int li=0; li<layout_->count(); li++)
		layout_->removeAt(li);

	int rc=0, cc=0, width = int(sqrt(plots_.count()));

	for(int pi=0; pi<plots_.count(); pi++) {
		layout_->addItem(plots_.at(pi), rc, cc++, Qt::AlignCenter);
		if(cc == width) {
			rc++;
			cc = 0;
		}
	}
}

void AMScanViewMultiScansView::enableLogScale(bool logScaleOn)
{
	AMScanViewInternal::enableLogScale(logScaleOn);

	setDataRangeConstraint(MPlot::Left);

	for (int i = 0; i < plots_.count(); i++)
		plots_.at(i)->plot()->enableLogScale(MPlot::Left, logScaleOn);
}

void AMScanViewMultiScansView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	setDataRangeConstraint(MPlot::Left);

	for(int i=0; i<plots_.count(); i++) {
		plots_.at(i)->plot()->enableAxisNormalization(MPlot::Left, normalizationOn,  min,  max);
	}
}

void AMScanViewMultiScansView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	for(int i=0; i<plots_.count(); i++) {
		if(waterfallOn)
			plots_.at(i)->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);
		else
			plots_.at(i)->plot()->setAxisScaleWaterfall(MPlot::Left, 0);
	}

}

void AMScanViewMultiScansView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_)
		for(int i=0; i<plots_.count(); i++)
			plots_.at(i)->plot()->setAxisScaleWaterfall(MPlot::Left, offset);
}

void AMScanViewMultiScansView::setDataRangeConstraint(int id)
{
	switch(id){

	case MPlot::Left:

		double val;

		if (logScaleEnabled_ && !normalizationEnabled_){

			double min = MPLOT_POS_INFINITY;
			for (int i = 0; i < plots_.count(); i++){

				if (plots_.at(i)->plot()->minimumYSeriesValue() < min)
					min = plots_.at(i)->plot()->minimumYSeriesValue();
			}

			if (min <= 0)
				val = min;
			else
				val = 1;
		}
		else if (logScaleEnabled_ && normalizationEnabled_)
			val = 1e-4;
		else
			val = MPLOT_NEG_INFINITY;

		for (int i = 0; i < plots_.count(); i++)
			plots_.at(i)->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(val, MPLOT_POS_INFINITY));

		break;
	}
}

// AMScanViewMultiSourcesView
//////////////////////////////////////////////////

AMScanViewMultiSourcesView::AMScanViewMultiSourcesView(AMScanView* masterView) : AMScanViewInternal(masterView) {

	layout_ = new QGraphicsGridLayout();
	layout_->setContentsMargins(0,0,0,0);
	layout_->setSpacing(0);
	layout_->setSizePolicy(sizePolicy());
	setLayout(layout_);

	// we need to have at least one plot, to fill our widget,  even if there are no scans.
	firstPlot_ = createDefaultPlot();

	firstPlotEmpty_ = true;

	// add all of the scans/data sources we have already
	reviewDataSources();

	reLayout();
}



AMScanViewMultiSourcesView::~AMScanViewMultiSourcesView() {

	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).*/

	foreach(MPlotGW* plot, dataSource2Plot_)
		plot->deleteLater();
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

			// go through all data sources that this scan has, check if they exist, remove and delete plot series if they do.
			for(int ci=0; ci<scan->dataSourceCount(); ci++) {
				QString dataSourceName = scan->dataSourceAt(ci)->name();
				if(dataSource2Plot_.contains(dataSourceName) && sourceAndScan2PlotItem_[dataSourceName].contains(scan)) {
					delete sourceAndScan2PlotItem_[dataSourceName][scan];
					sourceAndScan2PlotItem_[dataSourceName].remove(scan);
					reviewPlotAxesConfiguration(dataSource2Plot_[dataSourceName]);
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
				delete sourceAndScan2PlotItem_[sourceName][scan];
				sourceAndScan2PlotItem_[sourceName].remove(scan);
				reviewPlotAxesConfiguration(dataSource2Plot_[sourceName]);
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
					MPlotAbstractSeries* series = static_cast<MPlotAbstractSeries*>(plotItem);
					QPen pen = model()->plotPen(si, di);
					if(pen != series->linePen())
						series->setLinePen(pen);
					MPlotMarkerShape::Shape shape = model()->markerShape(si, di);
					QColor markerColor = model()->markerColor(si, di);
					series->setMarker(shape, 6, QPen(markerColor));
					break; }
				case 2: {
					MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItem);
					MPlotColorMap newMap = model()->plotColorMap(si, di);
					if(newMap != image->colorMap())
						image->setColorMap(newMap);
					break; }
				default:
					break;
				}
			}
		}
	}
}

//void AMScanViewMultiSourcesView::onToolSelectionChanged(QList<MPlotAbstractTool*> newSelection)
//{
//	// Clear all plots of previously added tools.

//	foreach (MPlotGW *plot, dataSource2Plot_.values()) {
//		plot->plot()->removeTools();
//	}

//	// Add tools to the plots, according to selection.

//	foreach (MPlotGW *plot, dataSource2Plot_.values()) {
//		foreach (MPlotAbstractTool *tool, newSelection) {
//			addToolToPlot(plot->plot(), tool);
//		}
//	}
//}

// re-do the layout
void AMScanViewMultiSourcesView::reLayout() {

	for(int li=0; li<layout_->count(); li++)
		layout_->removeAt(li);

	int rc=0, cc=0, width = int(sqrt(dataSource2Plot_.count()));

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
	bool layoutChanges = ( deleteTheseSources.count() || addTheseSources.count() );

	// this is a set of the data source names for the plots that need to have their axis configuration reviewed, because items have been added or removed
	QSet<QString> sourcesNeedingAxesReview;

	// delete the source plots that don't belong:
	foreach(QString sourceName, deleteTheseSources) {
		// delete the plot (will also delete any MPlotSeries within it)

		// WAIT: can't delete if it's the last/only plot. Instead, need to remove/delete all series from it and mark it as empty.
		if(dataSource2Plot_.count() == 1) {
			foreach(MPlotItem* plotItem, sourceAndScan2PlotItem_[sourceName]) {
				delete plotItem;
			}

			firstPlot_ = dataSource2Plot_[sourceName];
			firstPlotEmpty_ = true;
		}
		else
			dataSource2Plot_[sourceName]->deleteLater();

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
			newPlot = createDefaultPlot();

			newPlot->plot()->enableAxisNormalization(MPlot::Left, normalizationEnabled_,  normMin_,  normMax_);
			if(waterfallEnabled_)
				newPlot->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);
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
				delete sourceAndScan2PlotItem_[sourceName].take(scan);
				sourcesNeedingAxesReview << sourceName;
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
					sourcesNeedingAxesReview << sourceName;
					newItem->setDescription(scan->fullName());
					AMDataSource* dataSource = scan->dataSourceAt(di);
					dataSource2Plot_[sourceName]->plot()->addItem(newItem, (dataSource->rank() == 2 ? MPlot::Right : MPlot::Left));
					// zzzzzzzz Always add, even if 0? (requires checking everywhere for null plot items). Or only add if valid? (Going with latter... hope this is okay, in event someone tries at add 0d, 3d or 4d data source.
					sourceAndScan2PlotItem_[sourceName].insert(scan, newItem);

					dataSource2Plot_[sourceName]->plot()->axisBottom()->setAxisName(bottomAxisName(scan, dataSource));
					dataSource2Plot_[sourceName]->plot()->axisRight()->setAxisName(rightAxisName(scan, dataSource));
				}
			}
		}

		// only show the detailed legend (with the scan names) if there's more than one scan open. If there's just one, it's kinda obvious, so don't waste the space.
		if(model()->scanCount() > 1)
			dataSource2Plot_[sourceName]->plot()->legend()->enableDefaultLegend(true);
		else
			dataSource2Plot_[sourceName]->plot()->legend()->enableDefaultLegend(false);
	}

	foreach(QString sourceName, sourcesNeedingAxesReview) {
		if(dataSource2Plot_.contains(sourceName))
			reviewPlotAxesConfiguration(dataSource2Plot_[sourceName]);
	}

	return layoutChanges;
}

void AMScanViewMultiSourcesView::enableLogScale(bool logScaleOn)
{
	AMScanViewInternal::enableLogScale(logScaleOn);

	setDataRangeConstraint(MPlot::Left);

	if(firstPlotEmpty_)
		firstPlot_->plot()->enableLogScale(MPlot::Left, logScaleOn);
	QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
	while(i.hasNext()) {
		i.next();
		i.value()->plot()->enableLogScale(MPlot::Left, logScaleOn);
	}
}

void AMScanViewMultiSourcesView::enableNormalization(bool normalizationOn, double min, double max) {
	AMScanViewInternal::enableNormalization(normalizationOn, min, max);

	setDataRangeConstraint(MPlot::Left);

	if(firstPlotEmpty_)
		firstPlot_->plot()->enableAxisNormalization(MPlot::Left, normalizationOn,  min,  max);
	QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
	while(i.hasNext()) {
		i.next();
		i.value()->plot()->enableAxisNormalization(MPlot::Left, normalizationOn,  min,  max);
	}
}

void AMScanViewMultiSourcesView::enableWaterfallOffset(bool waterfallOn) {
	AMScanViewInternal::enableWaterfallOffset(waterfallOn);

	if(waterfallOn) {
		if(firstPlotEmpty_)
			firstPlot_->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);
		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->setAxisScaleWaterfall(MPlot::Left, waterfallOffset_);
		}
	}
	else {
		if(firstPlotEmpty_)
			firstPlot_->plot()->setAxisScaleWaterfall(MPlot::Left, 0);
		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->setAxisScaleWaterfall(MPlot::Left, 0);
		}
	}
}

void AMScanViewMultiSourcesView::setWaterfallOffset(double offset) {
	AMScanViewInternal::setWaterfallOffset(offset);

	if(waterfallEnabled_) {
		if(firstPlotEmpty_)
			firstPlot_->plot()->setAxisScaleWaterfall(MPlot::Left, offset);

		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->setAxisScaleWaterfall(MPlot::Left, offset);
		}
	}
}

void AMScanViewMultiSourcesView::setDataRangeConstraint(int id)
{
	switch(id){

	case MPlot::Left:

		double val;

		if (logScaleEnabled_ && !normalizationEnabled_){

			double min = MPLOT_POS_INFINITY;

			if(firstPlotEmpty_)
				val = MPLOT_NEG_INFINITY;
			QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
			while(i.hasNext()) {
				i.next();

				if (i.value()->plot()->minimumYSeriesValue() < min)
					min = i.value()->plot()->minimumYSeriesValue();
			}

			if (min <= 0)
				val = min;
			else
				val = 1;
		}
		else if (logScaleEnabled_ && normalizationEnabled_)
			val = 1e-4;
		else
			val = MPLOT_NEG_INFINITY;

		if(firstPlotEmpty_)
			firstPlot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(val, MPLOT_POS_INFINITY));
		QMapIterator<QString, MPlotGW*> i(dataSource2Plot_);
		while(i.hasNext()) {
			i.next();
			i.value()->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(MPlotAxisRange(val, MPLOT_POS_INFINITY));
		}

		break;
	}
}

#include <QPrinter>
#include <QFileInfo>
#include <QMessageBox>

void AMScanView::exportGraphicsFile(const QString& fileName)
{
	if (fileName.endsWith(".pdf")){
		QPrinter printer(QPrinter::HighResolution);

		printer.setPageSize(QPrinter::Letter);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOrientation(QPrinter::Landscape);

		if(fileName.contains("userData"))
				printer.setOutputFileName(AMUserSettings::defaultAbsoluteExportFolder() + "/" + QDateTime::currentDateTime().toString("dd-MM-yyyy_[hh:mm:ss]"));
		else
				printer.setOutputFileName(fileName);

		QPainter painter(&printer);
		gview_->render(&painter);

		painter.end();
	}

	else if (fileName.endsWith(".jpg")
			 || fileName.endsWith(".png")
			 || fileName.endsWith(".tiff")
			 || fileName.endsWith(".ppm")
			 || fileName.endsWith(".bmp")){

		QImage image(gview_->size(), QImage::Format_ARGB32_Premultiplied);
		QPainter painter(&image);
		gview_->render(&painter);
		painter.end();

		if(fileName.contains("userData"))
				image.save(AMUserSettings::defaultAbsoluteExportFolder() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss"));
		else
				image.save(fileName);;
	}
}

void AMScanView::printGraphics()
{
		QPrinter printer(QPrinter::HighResolution);
		printer.setPageSize(QPrinter::Letter);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOrientation(QPrinter::Landscape);
		printer.setOutputFileName(AMUserSettings::defaultAbsoluteExportFolder() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss"));

		QPrintDialog *dialog = new QPrintDialog(&printer, this);
			dialog->setWindowTitle(tr("Print Spectra"));
			if (dialog->exec() != QDialog::Accepted)
			return;

		QPainter painter(&printer);
		gview_->render(&painter);

		painter.end();


}

QString AMScanViewInternal::bottomAxisName(AMScan *scan, AMDataSource *dataSource)
{
	if (scan->scanRank() == 0) {
		AMAxisInfo ai = dataSource->axisInfoAt(0);
		QString rv = ai.description;
		if(!ai.units.isEmpty())
			rv.append(", " % ai.units);
		return rv;
	}

	if (scan->scanRank() == 1 || scan->scanRank() == 2 || scan->scanRank() == 3) {
		AMAxisInfo ai = scan->rawData()->scanAxisAt(0);	// this isn't really cool... Should stick to publicly exposed data from the data source.
		QString rv = ai.description;
		if(!ai.units.isEmpty())
			rv.append(", " % ai.units);
		return rv;
	}

	return QString();
}

QString AMScanViewInternal::rightAxisName(AMScan *scan, AMDataSource *dataSource)
{
	Q_UNUSED(scan)

	if (dataSource->rank() == 2) {
		AMAxisInfo ai = dataSource->axisInfoAt(1);
		QString rv = ai.description;
		if(!ai.units.isEmpty()) {
			rv.append(", " % ai.units);
		}
		return rv;
	}

	else
		return QString();
}

//void AMScanView::onToolsChanged()
//{
//	// Notify each view that they should review the tools applied.

//	foreach (AMScanViewInternal *view, views_) {
//		view->updatePlotTools();
//	}
//}

void AMScanView::onRowInserted(const QModelIndex &parent, int start, int end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	// inserting scans:
	if(!parent.isValid()) {
		if(scansModel_->scanCount() == AM_SCAN_VIEW_HIDE_SCANBARS_AFTER_N_SCANS) {
			setScanBarsVisible(false);
			// this will only happen once, the user can re-show them if they want.
		}
	}
}

void AMScanView::setScanBarsVisible(bool areVisible)
{
	modeBar_->showSourcesButton_->setChecked(areVisible);
	scanBars_->setVisible(areVisible);
}
