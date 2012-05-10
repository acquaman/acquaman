/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AM2DScanView.h"

#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotTools.h"

#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMDataSourceImageData.h"
#include "dataman/datasource/AMDataSourceImageDatawDefault.h"
#include "dataman/AMScan.h"

#include "util/AMErrorMonitor.h"

#include <QSizePolicy>
#include <QStringBuilder>
#include <QCheckBox>
#include <QFileInfo>

AM2DScanBar::AM2DScanBar(QWidget *parent)
	: QWidget(parent)
{
	dataPosition_ = new QLabel("Current Position:");
	selectedRect_ = new QLabel("");
	xUnits_ = "";
	yUnits_ = "";
	position_ = QPointF();
	rect_ = QRectF();

	QCheckBox *showSpectra = new QCheckBox("Show Spectra");
	showSpectra->setChecked(false);
	connect(showSpectra, SIGNAL(toggled(bool)), this, SIGNAL(showSpectra(bool)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(dataPosition_);
	layout->addStretch();
	layout->addWidget(showSpectra, 0, Qt::AlignRight);

	QVBoxLayout *barLayout = new QVBoxLayout;
	barLayout->addLayout(layout);
	barLayout->addWidget(selectedRect_);

	setLayout(barLayout);
}

void AM2DScanBar::setDataPosition(const QPointF &point)
{
	position_ = point;

	QString text = "Current Position: (";
	text.append(QString::number(point.x(), 'f', 3));

	if (!xUnits().isEmpty())
		text.append(QString(" %1").arg(xUnits()));

	text.append(", ");
	text.append(QString::number(point.y(), 'f', 3));

	if (!yUnits().isEmpty())
		text.append(QString(" %1").arg(yUnits()));

	text.append(")");

	dataPosition_->setText(text);
}

void AM2DScanBar::setSelectedRect(const QRectF &rect)
{
	rect_ = rect;

	if (rect_.isNull()){

		selectedRect_->setText("");
	}

	else{

		QString text = QString("Highlighted Region - Start: (%1 %5, %2 %6) End: (%3 %7, %4 %8)")
				.arg(QString::number(rect_.bottomLeft().x(), 'f', 3))
				.arg(QString::number(rect_.bottomLeft().y(), 'f', 3))
				.arg(QString::number(rect_.topRight().x(), 'f', 3))
				.arg(QString::number(rect_.topRight().y(), 'f', 3))
				.arg(xUnits())
				.arg(xUnits())
				.arg(yUnits())
				.arg(yUnits());

		selectedRect_->setText(text);
	}
}

// AM2DScanView
/////////////////////////////////////

AM2DScanView::AM2DScanView(AMScanSetModel* model, QWidget *parent)
	: QWidget(parent)
{
	scansModel_ = model;
	if(scansModel_ == 0)
		scansModel_ = new AMScanSetModel(this);

	spectrumViewIsVisible_ = false;

	setupUI();
	makeConnections();

	exclusiveScanBars_->setModel(scansModel_);
	multiScanBars_->setModel(scansModel_);

	exclusiveModeAnim_ = new QPropertyAnimation(gExclusiveView_->graphicsWidget(), "geometry", this);
	exclusiveModeAnim_->setDuration(500);
	exclusiveModeAnim_->setEasingCurve(QEasingCurve::InOutCubic);

	multiViewModeAnim_ = new QPropertyAnimation(gMultiView_->graphicsWidget(), "geometry", this);
	multiViewModeAnim_->setDuration(500);
	multiViewModeAnim_->setEasingCurve(QEasingCurve::InOutCubic);
}

AM2DScanView::~AM2DScanView()
{
}

void AM2DScanView::setupUI()
{
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setMargin(6);
	vl->setSpacing(0);

	gExclusiveView_ = new AMGraphicsViewAndWidget();
	gExclusiveView_->setMinimumSize(400,300);
	gExclusiveView_->graphicsWidget()->setGeometry(0,0,640, 480);

	vl->addWidget(gExclusiveView_);

	exclusiveScanBars_ = new AMScanViewSourceSelector();
	exclusiveScanBars_->setExclusiveModeOn();
	vl->addWidget(exclusiveScanBars_);

	exclusive2DScanBar_ = new AM2DScanBar;
	vl->addWidget(exclusive2DScanBar_);

	setLayout(vl);

	// setup linear layout within main graphics area:
	gExclusiveLayout_ = new QGraphicsLinearLayout();
	gExclusiveLayout_->setSpacing(0);
	gExclusiveLayout_->setContentsMargins(0,0,0,0);
	gExclusiveView_->graphicsWidget()->setLayout(gExclusiveLayout_);

	QVBoxLayout *multiViewLayout = new QVBoxLayout;
	multiViewLayout->setMargin(6);
	multiViewLayout->setSpacing(0);

	gMultiView_ = new AMGraphicsViewAndWidget();
	gMultiView_->setMinimumSize(400, 300);
	gMultiView_->graphicsWidget()->setGeometry(0, 0, 640, 640);

	multiViewLayout->addWidget(gMultiView_);

	multiScanBars_ = new AMScanViewSourceSelector();
	multiViewLayout->addWidget(multiScanBars_);

	multiViewBox_ = new QGroupBox;
	multiViewBox_->setLayout(multiViewLayout);
	multiViewBox_->setWindowTitle("Multi-Region Of Interest View");

	gMultiViewLayout_ = new QGraphicsLinearLayout();
	gMultiViewLayout_->setSpacing(0);
	gMultiViewLayout_->setContentsMargins(0, 0, 0, 0);
	gMultiView_->graphicsWidget()->setLayout(gMultiViewLayout_);

	exclusiveView_ = new AM2DScanViewExclusiveView(this);
	multiView_ = new AM2DScanViewMultiSourcesView(this);
	spectrumView_ = new AM2DScanViewSingleSpectrumView(this);

	spectrumViewBox_ = new QGroupBox;
	QHBoxLayout *spectrumViewBoxLayout = new QHBoxLayout;
	spectrumViewBoxLayout->addWidget(spectrumView_);
	spectrumViewBox_->setLayout(spectrumViewBoxLayout);
	spectrumViewBox_->setWindowTitle("View Single Spectrum");

	gExclusiveLayout_->addItem(exclusiveView_);
	gMultiViewLayout_->addItem(multiView_);
}

void AM2DScanView::makeConnections()
{
	// connect resize event from graphicsView to resize the stuff inside the view
	connect(gExclusiveView_, SIGNAL(resized(QSizeF)), this, SLOT(resizeExclusiveViews()), Qt::QueuedConnection);
	connect(gMultiView_, SIGNAL(resized(QSizeF)), this, SLOT(resizeMultiViews()), Qt::QueuedConnection);

	connect(exclusiveView_, SIGNAL(dataPositionChanged(QPointF)), exclusive2DScanBar_, SLOT(setDataPosition(QPointF)));
	connect(exclusiveView_, SIGNAL(selectedRectChanged(QRectF)), exclusive2DScanBar_, SLOT(setSelectedRect(QRectF)));
	connect(exclusive2DScanBar_, SIGNAL(showSpectra(bool)), this, SLOT(setSpectrumViewVisibility(bool)));
	connect(exclusiveView_, SIGNAL(dataPositionChanged(QPointF)), this, SLOT(onDataPositionChanged(QPointF)));
}

void AM2DScanView::setSpectrumViewVisibility(bool visible)
{
	spectrumViewIsVisible_ = visible;
	spectrumViewBox_->setVisible(spectrumViewIsVisible_);
}

void AM2DScanView::onDataPositionChanged(const QPointF &point)
{
	int x = -1;
	int y = -1;
	AMDataSource *datasource = currentScan_->dataSourceAt(currentScan_->indexOfDataSource(scansModel_->exclusiveDataSourceName()));

	// This assumes 2D maps where the size is greater than 1x1, 1xN, or Nx1.
	double delX = (double(datasource->axisValue(0, 1)) - double(datasource->axisValue(0, 0)))/2;
	double delY = (double(datasource->axisValue(1, 1)) - double(datasource->axisValue(1, 0)))/2;

	for (int i = 0; i < currentScan_->scanSize(0); i++)
		if (fabs(point.x() - double(datasource->axisValue(0, i))) < delX)
			x = i;

	for (int i = 0; i < currentScan_->scanSize(1); i++)
		if (fabs(point.y() - double(datasource->axisValue(1, i))) < delY)
			y = i;

	QString filename = currentScan_->additionalFilePaths().first();
	QFileInfo sourceFileInfo(currentScan_->additionalFilePaths().first());
	if(sourceFileInfo.isRelative())
		filename = AMUserSettings::userDataFolder % filename;

	spectrumView_->onDataPositionChanged(AMnDIndex(x, y), currentScan_->scanSize(0), filename);
}

void AM2DScanView::setCurrentScan(AMScan *scan)
{
	currentScan_ = scan;

	if (currentScan_)
		exclusive2DScanBar_->setUnits(scan->rawData()->scanAxisAt(0).units, scan->rawData()->scanAxisAt(1).units);

	else
		exclusive2DScanBar_->setUnits("", "");
}

void AM2DScanView::resizeExclusiveViews()
{
	QSize viewSize = gExclusiveView_->size();
	QSizeF mainWidgetSize = QSizeF(viewSize.width(), viewSize.height());

	gExclusiveView_->setSceneRect(QRectF(QPointF(0,0), viewSize ));

	QPointF pos = QPointF(-viewSize.width()*0, 0);

	exclusiveModeAnim_->stop();
	exclusiveModeAnim_->setStartValue(gExclusiveView_->graphicsWidget()->geometry());
	exclusiveModeAnim_->setEndValue(QRectF(pos, mainWidgetSize));
	exclusiveModeAnim_->start();
}

void AM2DScanView::resizeMultiViews()
{
	QSize viewSize = gMultiView_->size();
	QSizeF mainWidgetSize = QSizeF(viewSize.width(), viewSize.height());

	gMultiView_->setSceneRect(QRectF(QPointF(0,0), viewSize ));

	QPointF pos = QPointF(-viewSize.width()*0, 0);

	multiViewModeAnim_->stop();
	multiViewModeAnim_->setStartValue(gMultiView_->graphicsWidget()->geometry());
	multiViewModeAnim_->setEndValue(QRectF(pos, mainWidgetSize));
	multiViewModeAnim_->start();
}

void AM2DScanView::addScan(AMScan *newScan)
{
	scansModel_->addScan(newScan);
}

// remove a scan from the view:
void AM2DScanView::removeScan(AMScan* scan)
{
	scansModel_->removeScan(scan);
}

void AM2DScanView::showEvent(QShowEvent *e)
{
	if (!multiViewBox_->isVisible())
		multiViewBox_->show();

	if (!spectrumViewBox_->isVisible() && spectrumViewIsVisible_)
		spectrumViewBox_->show();

	QWidget::showEvent(e);
}

void AM2DScanView::hideEvent(QHideEvent *e)
{
	if (multiViewBox_->isVisible())
		multiViewBox_->hide();

	if (spectrumViewBox_->isVisible())
		spectrumViewBox_->hide();

	QWidget::hideEvent(e);
}

void AM2DScanView::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
		emit dataPositionChanged(e->globalPos());

	QWidget::mousePressEvent(e);
}

void AM2DScanView::setAxisInfoForSpectrumView(const AMAxisInfo &info, bool propogateToPlotRange)
{
	spectrumView_->setAxisInfo(info, propogateToPlotRange);
}

void AM2DScanView::setPlotRange(double low, double high)
{
	spectrumView_->setPlotRange(low, high);
}

// AM2DScanViewInternal
////////////////////////////////////////////////

AM2DScanViewInternal::AM2DScanViewInternal(AM2DScanView* masterView)
	: QGraphicsWidget(),
	  masterView_(masterView)
{
	connect(model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

	QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Preferred);
	sp.setHorizontalStretch(1);
	setSizePolicy(sp);
	// note that the _widget_'s size policy will be meaningless after a top-level layout is set. (the sizePolicy() of the layout is used instead.)  Therefore, subclasses with top-level layouts need to copy this sizePolicy() to their layout before setting it.
}

AMScanSetModel *AM2DScanViewInternal::model() const
{
	return masterView_->model();
}

MPlotGW * AM2DScanViewInternal::createDefaultPlot()
{
	MPlotGW* rv = new MPlotGW();
	rv->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	rv->plot()->axisBottom()->setTicks(5);
	rv->plot()->axisTop()->setTicks(5);
	rv->plot()->axisLeft()->showGrid(false);

	rv->plot()->axisBottom()->showAxisName(true);
	rv->plot()->axisLeft()->showAxisName(true);

	rv->plot()->setMarginLeft(13);
	rv->plot()->setMarginBottom(15);
	rv->plot()->setMarginTop(15);

	rv->plot()->axisScaleBottom()->setPadding(0);
	rv->plot()->axisScaleLeft()->setPadding(0);

	return rv;
}

// Helper function to create an appropriate MPlotItem and connect it to the data, depending on the dimensionality of \c dataSource.  Returns 0 if we can't handle this dataSource and no item was created (ex: unsupported dimensionality; we only handle 1D or 2D data for now.)
MPlotItem* AM2DScanViewInternal::createPlotItemForDataSource(const AMDataSource* dataSource, const AMDataSourcePlotSettings& plotSettings)
{
	MPlotItem* rv = 0;

	if(dataSource == 0) {
		AMErrorMon::alert(this, AM2DSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_NULL_DATA_SOURCE, "Asked to create a plot item for a null data source.");
		return 0;
	}

	switch(dataSource->rank()) {	// depending on the rank, we'll need an XY-series or an image to display it. 3D and 4D, etc. we don't handle for now.

	case 2: {
		MPlotImageBasicwDefault* image = new MPlotImageBasicwDefault();
		image->setModel(new AMDataSourceImageDatawDefault(dataSource, 0), true);
		image->setColorMap(plotSettings.colorMap);
		image->setZValue(-1000);
		rv = image;
		break; }
	default:
		AMErrorMon::alert(this, AM2DSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_UNHANDLED_RANK, "Asked to create a plot item for a rank that we don't handle.");
		rv = 0;
		break;
	}

	return rv;
}

// AMScanViewExclusiveView
/////////////////////////////

AM2DScanViewExclusiveView::AM2DScanViewExclusiveView(AM2DScanView* masterView)
	: AM2DScanViewInternal(masterView)
{
	// create our main plot:
	plot_ = createDefaultPlot();

	MPlotDataPositionTool *positionTool = new MPlotDataPositionTool;
	plot_->plot()->addTool(positionTool);
	positionTool->addDataPositionIndicator(plot_->plot()->axisScaleBottom(), plot_->plot()->axisScaleLeft());

	connect(plot_->plot()->signalSource(), SIGNAL(dataPositionChanged(uint,QPointF)), this, SLOT(onDataPositionChanged(uint,QPointF)));
	connect(plot_->plot()->signalSource(), SIGNAL(selectedDataRectChanged(uint,QRectF)), this, SLOT(onSelectedRectChanged(uint,QRectF)));

	QGraphicsLinearLayout* gl = new QGraphicsLinearLayout();
	gl->setContentsMargins(0,0,0,0);
	gl->setSpacing(0);
	gl->addItem(plot_);
	gl->setSizePolicy(sizePolicy());
	setLayout(gl);

	// the list of plotItems_ needs one element for each scan.
	for(int scanIndex = 0; scanIndex < model()->scanCount(); scanIndex++)
		addScan(scanIndex);

	connect(model(), SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));

	refreshTitle();
}

AM2DScanViewExclusiveView::~AM2DScanViewExclusiveView()
{
	// PlotSeries's will be deleted as children items of the plot.
	delete plot_;
}

void AM2DScanViewExclusiveView::onRowInserted(const QModelIndex& parent, int start, int end)
{
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
void AM2DScanViewExclusiveView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
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
void AM2DScanViewExclusiveView::onRowRemoved(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	refreshTitle();
}

// when data changes.
void AM2DScanViewExclusiveView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
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
void AM2DScanViewExclusiveView::onExclusiveDataSourceChanged(const QString& exclusiveDataSourceName)
{
	Q_UNUSED(exclusiveDataSourceName)

	for(int i=0; i<model()->scanCount(); i++)
		reviewScan(i);

	refreshTitle();
}

void AM2DScanViewExclusiveView::onDataPositionChanged(uint index, const QPointF &point)
{
	Q_UNUSED(index)

	emit dataPositionChanged(point);
}

void AM2DScanViewExclusiveView::onSelectedRectChanged(uint index, const QRectF &rect)
{
	Q_UNUSED(index)

	emit selectedRectChanged(rect);
}

void AM2DScanViewExclusiveView::refreshTitle() {

	int numScansShown = 0;
	foreach(MPlotItem* s, plotItems_)
		if(s)
			numScansShown++;

	QString scanCount = (numScansShown == 1) ? " (1 scan)" : QString(" (%1 scans)").arg(numScansShown);
	/// \todo This should show a data source description; not a name. However, we don't know that all the descriptions (for all the data sources matching the exclusive data source) are the same....
	plot_->plot()->legend()->setTitleText(model()->exclusiveDataSourceName() + scanCount);	// result ex: "tey (3 scans)"
}

// Helper function to handle adding a scan (at row scanIndex in the model)
void AM2DScanViewExclusiveView::addScan(int scanIndex)
{
	QString dataSourceName = model()->exclusiveDataSourceName();

	int dataSourceIndex = model()->scanAt(scanIndex)->indexOfDataSource(dataSourceName);
	AMDataSource* dataSource = model()->dataSourceAt(scanIndex, dataSourceIndex);	// dataSource will = 0 if this scan doesn't have the exclusive data source in it.

	MPlotItem* newItem = 0;

	if(dataSource)
		newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(scanIndex, dataSourceIndex));

	if(newItem) {

		newItem->setDescription(model()->scanAt(scanIndex)->fullName());
		plot_->plot()->addItem(newItem, MPlot::Left);
	}

	plotItems_.insert(scanIndex, newItem);
	plotItemDataSources_.insert(scanIndex, dataSource);
}

// Helper function to review a scan when a data source is added or the exclusive data source changes.
void AM2DScanViewExclusiveView::reviewScan(int scanIndex)
{
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

			MPlotItem* newItem;
			plotItems_[scanIndex] = newItem = createPlotItemForDataSource(dataSource, model()->plotSettings(scanIndex, dataSourceIndex));

			if(newItem) {

				newItem->setDescription(model()->scanAt(scanIndex)->fullName());
				plot_->plot()->addItem(newItem, MPlot::Left);
				AMScan *scan = model()->scanAt(scanIndex);

				switch(scan->scanRank()){

				case 1:
					plot_->plot()->axisBottom()->setAxisName(scan->rawData()->scanAxisAt(0).units.isEmpty() ? scan->rawData()->scanAxisAt(0).description : scan->rawData()->scanAxisAt(0).description % ", " % scan->rawData()->scanAxisAt(0).units);
					break;

				case 2:
					plot_->plot()->axisBottom()->setAxisName(scan->rawData()->scanAxisAt(0).units.isEmpty() ? scan->rawData()->scanAxisAt(0).description : scan->rawData()->scanAxisAt(0).description % ", " % scan->rawData()->scanAxisAt(0).units);
					plot_->plot()->axisLeft()->setAxisName(scan->rawData()->scanAxisAt(1).units.isEmpty() ? scan->rawData()->scanAxisAt(1).description : scan->rawData()->scanAxisAt(1).description % ", " % scan->rawData()->scanAxisAt(1).units);
					break;
				}
			}

			plotItemDataSources_[scanIndex] = dataSource;
		}

		else {	// We already have one.  Review and update the existing plot item. (When would this be called? // A: When the exclusive data source changes, for one thing. need to change old series/image to represent new data)

			plotItems_.at(scanIndex)->setDescription(model()->scanAt(scanIndex)->fullName());

			switch(dataSource->rank()) {

			case 2: {

				MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItems_.at(scanIndex));
				if(plotItemDataSources_.at(scanIndex) != dataSource) {
					AMDataSourceImageDatawDefault* newData = new AMDataSourceImageDatawDefault(dataSource, 0);
					image->setModel(newData, true);
					plotItemDataSources_[scanIndex] = dataSource;
				}
				image->setColorMap(model()->plotColorMap(scanIndex, dataSourceIndex));

				break;
			}

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

// AM2DScanViewSourcesView
/////////////////////////////////////////

AM2DScanViewMultiSourcesView::AM2DScanViewMultiSourcesView(AM2DScanView* masterView)
	: AM2DScanViewInternal(masterView)
{
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

AM2DScanViewMultiSourcesView::~AM2DScanViewMultiSourcesView()
{
	/* NOT necessary to delete all plotSeries. As long as they are added to a plot, they will be deleted when the plot is deleted (below).*/
	foreach(MPlotGW* plot, dataSource2Plot_)
		delete plot;
}

void AM2DScanViewMultiSourcesView::onRowInserted(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(reviewDataSources())
		reLayout();
}


// before scans or data sources are deleted in the model:
void AM2DScanViewMultiSourcesView::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
	// removing a scan(s):    (start through end are the scan index)
	if(!parent.isValid()) {

		for(int si = end; si >= start; si--) {

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

		for(int di = end; di >= start; di--) {

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
void AM2DScanViewMultiSourcesView::onRowRemoved(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(reviewDataSources())
		reLayout();
}

// when data changes: (Things we care about: color, linePen, and visible)
void AM2DScanViewMultiSourcesView::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
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

		for(int di = topLeft.row(); di <= bottomRight.row(); di++) {

			QString sourceName = model()->scanAt(si)->dataSourceAt(di)->name();

			if(dataSource2Plot_.contains(sourceName) && sourceAndScan2PlotItem_[sourceName].contains(scan)) {
				MPlotItem* plotItem = sourceAndScan2PlotItem_[sourceName][scan];

				switch(model()->dataSourceAt(si, di)->rank()) {

				case 2: {

					MPlotAbstractImage* image = static_cast<MPlotAbstractImage*>(plotItem);
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

// re-do the layout
void AM2DScanViewMultiSourcesView::reLayout()
{
	for(int li = 0; li < layout_->count(); li++)
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

bool AM2DScanViewMultiSourcesView::reviewDataSources() {

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
		else
			newPlot = createDefaultPlot();

		dataSource2Plot_.insert(sourceName, newPlot);
		sourceAndScan2PlotItem_.insert(sourceName, QHash<AMScan*, MPlotItem*>());
		newPlot->plot()->legend()->setTitleText(sourceName);
		newPlot->plot()->axisTop()->setAxisName(sourceName);
		newPlot->plot()->axisTop()->showAxisName();
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
				sourcesNeedingAxesReview << sourceName;
			}
		}

		// loop through all scans, adding series to this plot if required...
		for(int si = 0; si < model()->scanCount(); si++) {

			AMScan* scan = model()->scanAt(si);

			int di = scan->indexOfDataSource(sourceName);

			// if this scan contains this data source, and it's visible, and we don't have a series for it yet... make and add the new series
			if(di >= 0 && model()->isVisible(si, di) && !sourceAndScan2PlotItem_[sourceName].contains(scan)) {

				MPlotItem* newItem = createPlotItemForDataSource(scan->dataSourceAt(di), model()->plotSettings(si, di));
				if(newItem) {
					sourcesNeedingAxesReview << sourceName;
					newItem->setDescription(scan->fullName());
					dataSource2Plot_[sourceName]->plot()->addItem(newItem, MPlot::Left);

					AMScan *scan = model()->scanAt(si);

					switch(scan->scanRank()){

					case 1:
						dataSource2Plot_[sourceName]->plot()->axisBottom()->setAxisName(scan->rawData()->scanAxisAt(0).units.isEmpty() ? scan->rawData()->scanAxisAt(0).description : scan->rawData()->scanAxisAt(0).description % ", " % scan->rawData()->scanAxisAt(0).units);
						break;

					case 2:
						dataSource2Plot_[sourceName]->plot()->axisBottom()->setAxisName(scan->rawData()->scanAxisAt(0).units.isEmpty() ? scan->rawData()->scanAxisAt(0).description : scan->rawData()->scanAxisAt(0).description % ", " % scan->rawData()->scanAxisAt(0).units);
						dataSource2Plot_[sourceName]->plot()->axisLeft()->setAxisName(scan->rawData()->scanAxisAt(1).units.isEmpty() ? scan->rawData()->scanAxisAt(1).description : scan->rawData()->scanAxisAt(1).description % ", " % scan->rawData()->scanAxisAt(1).units);
						break;
					}

					// zzzzzzzz Always add, even if 0? (requires checking everywhere for null plot items). Or only add if valid? (Going with latter... hope this is okay, in event someone tries at add 0d, 3d or 4d data source.
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

	return layoutChanges;
}

// AM2DScanViewSingleSpectrumView
//////////////////////////////////////////////////

AM2DScanViewSingleSpectrumView::AM2DScanViewSingleSpectrumView(QWidget *parent)
	: QWidget(parent)
{
	qRegisterMetaType<QVector<double> >("QVector<double>");
	connect(&fetcher_, SIGNAL(fetchedSpectrum(QVector<double>)), this, SLOT(updatePlot(QVector<double>)));

	x_.resize(0);

	setupPlot();

	model_ = new MPlotVectorSeriesData;
	MPlotSeriesBasic *series = new MPlotSeriesBasic(model_);
	series->setMarker(MPlotMarkerShape::None);
	series->setDescription("Spectrum");
	plot_->plot()->addItem(series);
	plot_->setMinimumSize(600, 400);
	plot_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	table_ = new AMSelectablePeriodicTable(this);
	connect(table_, SIGNAL(elementSelected(int)), this, SLOT(onElementSelected(int)));
	connect(table_, SIGNAL(elementDeselected(int)), this, SLOT(onElementDeselected(int)));
	tableView_ = new AMSelectablePeriodicTableView(table_);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(plot_);
//	layout->addStretch();
	layout->addWidget(tableView_, 0, Qt::AlignCenter);

	setLayout(layout);
}

void AM2DScanViewSingleSpectrumView::setupPlot()
{
	MPlot *plot = new MPlot;
	plot_ = new MPlotWidget(this);
	plot_->setPlot(plot);

	plot_->plot()->plotArea()->setBrush(QBrush(Qt::white));
	plot_->plot()->axisBottom()->setTicks(5);
	plot_->plot()->axisLeft()->setTicks(5);
	plot_->plot()->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->plot()->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->plot()->axisBottom()->showAxisName(true);
	plot_->plot()->axisLeft()->showAxisName(false);

	// Set the margins for the plot.
	plot_->plot()->setMarginLeft(10);
	plot_->plot()->setMarginBottom(15);
	plot_->plot()->setMarginRight(2);
	plot_->plot()->setMarginTop(2);

	plot_->plot()->addTool(new MPlotDragZoomerTool());
	plot_->plot()->addTool(new MPlotWheelZoomerTool());
}

void AM2DScanViewSingleSpectrumView::onElementSelected(int atomicNumber)
{
	QString symbol = table_->elementByAtomicNumber(atomicNumber)->symbol();
	QList<QPair<QString, QString> > lines = table_->elementByAtomicNumber(atomicNumber)->emissionLines();
	QColor color = AMDataSourcePlotSettings::nextColor();
	MPlotPoint *newLine;
	QPair<QString, QString> line;

	foreach(line, lines){

		if (line.second.toDouble() >= range_.first && line.second.toDouble() <= range_.second
				&& line.first.contains("1") && line.first.compare("-"))	{

			newLine = new MPlotPoint(QPointF(line.second.toDouble(), 0));
			newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(color), QBrush(color));
			newLine->setDescription(symbol % " " % line.first);
			plot_->plot()->addItem(newLine);
		}
	}
}

void AM2DScanViewSingleSpectrumView::onElementDeselected(int atomicNumber)
{
	QString symbol = table_->elementByAtomicNumber(atomicNumber)->symbol();
	MPlot *plot = plot_->plot();

	foreach(MPlotItem *item, plot->plotItems()){

		if (item->description().contains(symbol))
			if (plot->removeItem(item))
				delete item;
	}
}

void AM2DScanViewSingleSpectrumView::setPlotRange(double low, double high)
{
	range_ = qMakePair(low, high);
	tableView_->setRange(low, high);

	foreach(int atomicNumber, table_->selectedElements())
		onElementDeselected(atomicNumber);

	foreach(int atomicNumber, table_->selectedElements())
		onElementSelected(atomicNumber);
}

void AM2DScanViewSingleSpectrumView::onDataPositionChanged(AMnDIndex index, int rowLength, const QString &filename)
{
	if (isVisible())
		fetcher_.fetch(index, rowLength, filename, x_.size());
}

void AM2DScanViewSingleSpectrumView::setAxisInfo(AMAxisInfo info, bool propogateToPlotRange)
{
	if (info.units.isEmpty())
		plot_->plot()->axisBottom()->setAxisName(info.name);

	else
		plot_->plot()->axisBottom()->setAxisName(info.name % ", " % info.units);

	x_.resize(info.size);

	for (int i = 0; i < info.size; i++)
		x_[i] = double(info.start) + i*double(info.increment);

	if (propogateToPlotRange)
		setPlotRange(double(info.start), double(info.start) + info.size*double(info.increment));
}

void AM2DScanViewSingleSpectrumView::updatePlot(QVector<double> spectrum)
{
	if (x_.size() == 0){

		x_.resize(spectrum.size());

		for (int i = 0; i < x_.size(); i++)
			x_[i] = i;
	}

	QVector<double> y = spectrum;
	model_->setValues(x_, y);
}
