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


#include "AM2DScanView.h"

#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotTools.h"

#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMDataSourceImageData.h"
#include "dataman/datasource/AMDataSourceImageDatawDefault.h"
#include "dataman/AMScan.h"

#include "ui/dataman/AM3dDataSourceView.h"
#include "util/AMErrorMonitor.h"

#include <QSizePolicy>
#include <QStringBuilder>
#include <QFileInfo>
#include <QMessageBox>
#include <QPrintDialog>


 AM2DScanBar::~AM2DScanBar(){}
AM2DScanBar::AM2DScanBar(QWidget *parent)
	: QWidget(parent)
{
	dataPosition_ = new QLabel("");
	selectedRect_ = new QLabel("");
	valueLabel_ = new QLabel("");
	dataRange_ = new QLabel("");
	xUnits_ = "";
	yUnits_ = "";
	position_ = QPointF();
	rect_ = QRectF();
	value_ = -1;
	range_ = AMRange(0.0, 0.0);

	showSpectra_ = new QCheckBox("Show Spectra");
	showSpectra_->setChecked(false);
	showSpectra_->setEnabled(false);
	connect(showSpectra_, SIGNAL(toggled(bool)), this, SIGNAL(showSpectra(bool)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(dataPosition_);
	layout->addWidget(valueLabel_);
	layout->addStretch();
	layout->addWidget(dataRange_);
	layout->addWidget(showSpectra_, 0, Qt::AlignRight);

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

void AM2DScanBar::setValue(double value)
{
	value_ = value;
	valueLabel_->setText(value_ == -1 ? QString("") : QString("Value: %1").arg(value, 0, 'g', 3));
}

void AM2DScanBar::setRange(const AMRange &range)
{
	range_ = range;

	if (range_.minimum() == range_.maximum())
		dataRange_->setText("");

	else
		dataRange_->setText(QString("Data Range: %1, %2").arg(range_.minimum(), 0, 'g', 3).arg(range_.maximum(), 0, 'g', 3));
}

void AM2DScanBar::setMinimum(double min)
{
	range_.setMinimum(min);

	if (range_.minimum() == range_.maximum())
		dataRange_->setText("");

	else
		dataRange_->setText(QString("Data Range: %1, %2").arg(range_.minimum()).arg(range_.maximum()));
}

void AM2DScanBar::setMaximum(double max)
{
	range_.setMaximum(max);

	if (range_.minimum() == range_.maximum())
		dataRange_->setText("");

	else
		dataRange_->setText(QString("Data Range: %1, %2").arg(range_.minimum()).arg(range_.maximum()));
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

	currentExclusiveDataSource_ = 0;
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
	gMultiView_->setMinimumSize(400, 400);
	gMultiView_->graphicsWidget()->setGeometry(0, 0, 640, 640);

	multiViewLayout->addWidget(gMultiView_);

	multiScanBars_ = new AMScanViewSourceSelector();
	multiViewLayout->addWidget(multiScanBars_);

	multiViewBox_ = new QGroupBox;
	multiViewBox_->setLayout(multiViewLayout);
	multiViewBox_->setWindowTitle("Multi-Region Of Interest View");
	multiViewBox_->setMaximumSize(800, 800);

	gMultiViewLayout_ = new QGraphicsLinearLayout();
	gMultiViewLayout_->setSpacing(0);
	gMultiViewLayout_->setContentsMargins(0, 0, 0, 0);
	gMultiView_->graphicsWidget()->setLayout(gMultiViewLayout_);

	exclusiveView_ = new AM2DScanViewExclusiveView(this);
	multiView_ = new AM2DScanViewMultiSourcesView(this);
	spectrumView_ = new AMScanViewSingleSpectrumView(this);

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

	connect(scansModel_, SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));
	connect(scansModel_, SIGNAL(scanAdded(AMScan*)), this, SLOT(onScanAdded(AMScan*)));
	connect(exclusiveView_, SIGNAL(selectedRectChanged(QRectF)), this, SLOT(onSelectedRectChanged(QRectF)));
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
	AMnDIndex index = getIndex(point);

	exclusive2DScanBar_->setDataPosition(point);
	exclusive2DScanBar_->setValue(double(currentExclusiveDataSource_->value(index)));

	if (exclusive2DScanBar_->showSpectraEnabled())
		spectrumView_->onDataPositionChanged(index);
}

void AM2DScanView::onSelectedRectChanged(const QRectF &rect)
{
	AMnDIndex bottomLeft = getIndex(rect.bottomLeft());
	AMnDIndex topRight = getIndex(rect.topRight());

	exclusive2DScanBar_->setSelectedRect(rect);

	if (exclusive2DScanBar_->showSpectraEnabled() && !rect.isNull())
		spectrumView_->onSelectedRectChanged(bottomLeft, topRight);
}

void AM2DScanView::onExclusiveDataSourceChanged(const QString &name)
{
	if (currentExclusiveDataSource_){

		disconnect(currentExclusiveDataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onExclusiveDataSourceValueChanged(AMnDIndex,AMnDIndex)));
		currentExclusiveDataSource_ = 0;
	}

	currentExclusiveDataSource_ = currentScan_->dataSourceAt(currentScan_->indexOfDataSource(name));
	exclusive2DScanBar_->setValue(double(currentExclusiveDataSource_->value(getIndex(exclusive2DScanBar_->dataPosition()))));
	AMRange range = getCurrentExclusiveDataSourceRange();
	if (!(range.minimum() == -1 && range.maximum() == -1))
		exclusive2DScanBar_->setRange(range);
	connect(currentExclusiveDataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onExclusiveDataSourceValueChanged(AMnDIndex,AMnDIndex)));
}

void AM2DScanView::onExclusiveDataSourceValueChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	exclusive2DScanBar_->setRange(getCurrentExclusiveDataSourceRange(start, end));
}

AMnDIndex AM2DScanView::getIndex(const QPointF &point) const
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

	return AMnDIndex(x, y);
}

AMRange AM2DScanView::getCurrentExclusiveDataSourceRange(const AMnDIndex &start, const AMnDIndex &end) const
{
	if (start == end && start.rank() == 2 && end.rank() == 2){

		AMRange range = exclusive2DScanBar_->range();
		double val = double(currentExclusiveDataSource_->value(start));

		if ((val != -1 && range.minimum() > val) || range.minimum() == -1)
			range.setMinimum(val);

		if (range.maximum() < val)
			range.setMaximum(val);

		return range;
	}

	else {

		int totalSize = 0;
		AMnDIndex startIndex = start;
		AMnDIndex endIndex = end;

		if (startIndex.rank() == 0 || endIndex.rank() == 0){

			startIndex = AMnDIndex(0, 0);
			endIndex = AMnDIndex(currentExclusiveDataSource_->size(0)-1, currentExclusiveDataSource_->size(1)-1);
			totalSize = startIndex.totalPointsTo(endIndex);
		}

		else
			totalSize = start.totalPointsTo(end);

		if (totalSize > 0){

			QVector<double> data(totalSize);
			currentExclusiveDataSource_->values(startIndex, endIndex, data.data());

			double min = data.at(0);
			double max = data.at(0);

			foreach (double value, data){

				if ((value != -1 && min > value) || min == -1)
					min = value;

				if (max < value)
					max = value;
			}

			return AMRange(min, max);
		}

		else
			return AMRange(-1.0, -1.0);
	}
}


void AM2DScanView::setSingleSpectrumDataSource(const QString &name)
{
	spectrumView_->setDataSourceByName(name);
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
	gExclusiveView_->graphicsWidget()->setGeometry(QRectF(QPointF(0,0), mainWidgetSize));
}

void AM2DScanView::resizeMultiViews()
{
	QSize viewSize = gMultiView_->size();
	QSizeF mainWidgetSize = QSizeF(viewSize.width(), viewSize.height());

	gMultiView_->setSceneRect(QRectF(QPointF(0,0), viewSize ));
	gMultiView_->graphicsWidget()->setGeometry(QRectF(QPointF(0,0), mainWidgetSize));
}

void AM2DScanView::onScanAdded(AMScan *scan)
{
	QList<AMDataSource *> sources;

	int scanRank = scan->scanRank();

	foreach (AMDataSource *source, scan->rawDataSources()->toList())
		if (source->rank()-scanRank == 1)
			sources.append(source);

	foreach (AMDataSource *source, scan->analyzedDataSources()->toList())
		if (source->rank()-scanRank == 1)
			sources.append(source);

	exclusive2DScanBar_->setShowSpectraEnabled(!sources.isEmpty());
	spectrumView_->setTitle(QString("%1 #%2").arg(scan->name()).arg(scan->number()));
	spectrumView_->setDataSources(sources);
}

void AM2DScanView::setAxisInfoForSpectrumView(const AMAxisInfo &info, bool propogateToPlotRange)
{
	spectrumView_->setAxisInfo(info, propogateToPlotRange);
}

void AM2DScanView::addScan(AMScan *newScan)
{
	if (scansModel_->scanCount() == 0)
		scansModel_->addScan(newScan);

	else
		QMessageBox::information(this, "Only one scan in 2D map mode", "Due to the nature of visualizing 2D maps.  You are only allowed to view one 2D scan per editor.");
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
	if (e->button() == Qt::RightButton && exclusiveView_->boundingRect().contains(e->pos()))
		emit dataPositionChanged(e->globalPos());

	QWidget::mousePressEvent(e);
}

void AM2DScanView::setEnergyRange(double low, double high)
{
	spectrumView_->setEnergyRange(low, high);
}

void AM2DScanView::addSingleSpectrumEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	spectrumView_->addEmissionLineNameFilter(newNameFilter);
}

void AM2DScanView::removeSingleSpectrumEmissionLineNameFilter(int index)
{
	spectrumView_->removeEmissionLineNameFilter(index);
}

void AM2DScanView::removeSingleSpectrumEmissionLineNameFilter(const QRegExp &filter)
{
	spectrumView_->removeEmissionLineNameFilter(filter);
}

void AM2DScanView::addSingleSpectrumPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	spectrumView_->addPileUpPeakNameFilter(newNameFilter);
}

void AM2DScanView::removeSingleSpectrumPileUpPeakNameFilter(int index)
{
	spectrumView_->removePileUpPeakNameFilter(index);
}

void AM2DScanView::removeSingleSpectrumPileUpPeakNameFilter(const QRegExp &filter)
{
	spectrumView_->removePileUpPeakNameFilter(filter);
}

void AM2DScanView::addSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	spectrumView_->addCombinationPileUpPeakNameFilter(newNameFilter);
}

void AM2DScanView::removeSingleSpectrumCombinationPileUpPeakNameFilter(int index)
{
	spectrumView_->removeCombinationPileUpPeakNameFilter(index);
}

void AM2DScanView::removeSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &filter)
{
	spectrumView_->removeCombinationPileUpPeakNameFilter(filter);
}

#include <QPrinter>
#include <QFileInfo>
#include <QMessageBox>

void AM2DScanView::exportGraphicsFile(const QString& fileName)
{
	if (fileName.endsWith(".pdf")){
		QPrinter printer(QPrinter::HighResolution);

		printer.setPageSize(QPrinter::Letter);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOrientation(QPrinter::Landscape);

		if(fileName.contains("userData"))
				printer.setOutputFileName(AMUserSettings::defaultAbsoluteExportFolder() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss"));
		else
				printer.setOutputFileName(fileName);

		QPainter painter(&printer);
		gExclusiveView_->render(&painter);

		painter.end();
	}

	else if (fileName.endsWith(".jpg")
			 || fileName.endsWith(".png")
			 || fileName.endsWith(".tiff")
			 || fileName.endsWith(".ppm")
			 || fileName.endsWith(".bmp")){

		QImage image(gExclusiveView_->size(), QImage::Format_ARGB32_Premultiplied);
		QPainter painter(&image);
		gExclusiveView_->render(&painter);
		painter.end();

		if(fileName.contains("userData"))
				image.save(AMUserSettings::defaultAbsoluteExportFolder() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss"));
		else
				image.save(fileName);
	}
}

void AM2DScanView::printGraphics()
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
		gExclusiveView_->render(&painter);

		painter.end();

}

// AM2DScanViewInternal
////////////////////////////////////////////////

 AM2DScanViewInternal::~AM2DScanViewInternal(){}
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

// Helper function to create an appropriate MPlotItem and connect it to the data, depending on the dimensionality of \c dataSource.  Returns 0 if we can't handle this dataSource and no item was created (ex: unsupported dimensionality, we only handle 1D or 2D data for now.)
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
		image->setDefaultValue(-1.0);
		AMDataSourceImageDatawDefault *model = new AMDataSourceImageDatawDefault(-1);
		model->setDataSource(dataSource);
		image->setModel(model, true);
		image->setColorMap(plotSettings.colorMap);
		image->setZValue(-1000);
		rv = image;
		break; }
	default:
		AMErrorMon::alert(this, AM2DSCANVIEW_CANNOT_CREATE_PLOT_ITEM_FOR_UNHANDLED_RANK, QString("Asked to create a plot item name %1 which has a rank that we don't handle.").arg(dataSource->name()));
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
	plot_->plot()->colorLegend()->setVisible(true);

	MPlotDataPositionTool *positionTool = new MPlotDataPositionTool(true);
	plot_->plot()->addTool(positionTool);
	positionTool->setDataPositionIndicator(plot_->plot()->axisScaleBottom(), plot_->plot()->axisScaleLeft());

	connect(plot_->plot()->signalSource(), SIGNAL(dataPositionChanged(QPointF)), this, SIGNAL(dataPositionChanged(QPointF)));
	connect(plot_->plot()->signalSource(), SIGNAL(selectedDataRectChanged(QRectF)), this, SIGNAL(selectedRectChanged(QRectF)));

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
	plot_->deleteLater();
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

void AM2DScanViewExclusiveView::refreshTitle() {

	int numScansShown = 0;
	foreach(MPlotItem* s, plotItems_)
		if(s)
			numScansShown++;

	QString scanCount = (numScansShown == 1) ? " (1 scan)" : QString(" (%1 scans)").arg(numScansShown);
	/// \todo This should show a data source description, not a name. However, we don't know that all the descriptions (for all the data sources matching the exclusive data source) are the same....
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

				case 3:
					// This assumes the axis that is ignored is the third axis.  This probably should be made more general for the future!
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
					AMDataSourceImageDatawDefault* newData = new AMDataSourceImageDatawDefault(-1);
					newData->setDataSource(dataSource);
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
		plot->deleteLater();
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

			// go through all data sources that this scan has, check if they exist, remove and delete plot series if they do.
			for(int ci=0; ci<scan->dataSourceCount(); ci++) {

				QString dataSourceName = scan->dataSourceAt(ci)->name();

				if(dataSource2Plot_.contains(dataSourceName) && sourceAndScan2PlotItem_[dataSourceName].contains(scan)) {
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

					case 3:
						// This assumes the axis that is ignored is the third axis.  This probably should be made more general for the future!
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

void AM2DScanViewMultiSourcesView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	foreach (MPlotGW *plot, dataSource2Plot_.values()){

		if (plot->mapRectToParent(plot->rect()).contains(event->pos()))
			model()->setExclusiveDataSourceByName(dataSource2Plot_.key(plot));
	}

	AM2DScanViewInternal::mousePressEvent(event);
}
