#include "CSRMainWindow.h"

#include <QVBoxLayout>

#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include <QDebug>
CSRMainWindow::CSRMainWindow(CSRDataModel *model, QWidget *parent)
	: QWidget(parent)
{
	model_ = model;

	setupPlot();

	rangeMinimum_ = new QSpinBox;
	rangeMinimum_->setRange(0, 2e7-1);
	rangeMinimum_->setValue(0);
	rangeMinimum_->setPrefix("Min: ");

	rangeMaximum_ = new QSpinBox;
	rangeMaximum_->setRange(0, 2e7-1);
	rangeMaximum_->setValue(3000);
	rangeMaximum_->setPrefix("Max: ");

	connect(rangeMinimum_, SIGNAL(editingFinished()), this, SLOT(onRangeMinimumChanged()));
	connect(rangeMaximum_, SIGNAL(editingFinished()), this, SLOT(onRangeMaximumChanged()));

	QHBoxLayout *editLayout = new QHBoxLayout;
	editLayout->addWidget(rangeMinimum_);
	editLayout->addWidget(rangeMaximum_);

	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addWidget(new QLabel(QString("Mean: %1, Std dev: %2\n").arg(model_->mean(), 0, 'g').arg(model_->standardDeviation(), 0, 'g')));
	mainLayout->addWidget(plotView_);
	mainLayout->addLayout(editLayout);

	setLayout(mainLayout);
}

void CSRMainWindow::setupPlot()
{
	// Create the plot window.
	plotView_ = new MPlotWidget;
	plotView_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	plot_ = new MPlot;
	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Time, ns");
	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setAxisName("Counts");

	// Set the margins for the plot.
	plot_->setMarginLeft(10);
	plot_->setMarginBottom(15);
	plot_->setMarginRight(2);
	plot_->setMarginTop(2);

	// Assumes that the dataSource() contains the spectrum most desired to view.
	MPlotSeriesBasic *csrSeries = new MPlotSeriesBasic;
	seriesData_ = new MPlotVectorSeriesData;
	QVector<double> x = QVector<double>(3000, 0);
	QVector<double> y = QVector<double>(3000, 0);
	model_->timeData(0, 2999, x.data());
	model_->data(0, 2999, y.data());
	seriesData_->setValues(x, y);
	csrSeries->setModel(seriesData_, true);
	csrSeries->setMarker(MPlotMarkerShape::None);
	csrSeries->setDescription("CSR");
	csrSeries->setLinePen(QPen(Qt::red));
	plot_->addItem(csrSeries);

//	MPlotSeriesBasic *smoothSeries = new MPlotSeriesBasic;
//	MPlotVectorSeriesData *smoothSeriesData = new MPlotVectorSeriesData;
//	model_->smoothData(y.data());
//	smoothSeriesData->setValues(x, y);
//	smoothSeries->setModel(smoothSeriesData, true);
//	smoothSeries->setMarker(MPlotMarkerShape::None);
//	smoothSeries->setDescription("Smooth CSR");
//	smoothSeries->setLinePen(QPen(Qt::blue));
//	plot_->addItem(smoothSeries);

//	MPlotPoint *newLine = new MPlotPoint(QPointF(x.first(), model_->mean()));
//	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
//	newLine->setDescription("Mean");
//	plot_->addItem(newLine);
//	items_ << newLine;

	MPlotPoint *newLine = new MPlotPoint(QPointF(x.first(), -1*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("Std Dev");
	plot_->addItem(newLine);
	items_ << newLine;

//	newLine = new MPlotPoint(QPointF(x.first(), -2*model_->standardDeviation()));
//	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
//	newLine->setDescription("2x Std Dev");
//	plot_->addItem(newLine);
//	items_ << newLine;

//	newLine = new MPlotPoint(QPointF(x.first(), -3*model_->standardDeviation()));
//	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
//	newLine->setDescription("3x Std Dev");
//	plot_->addItem(newLine);
//	items_ << newLine;

	newLine = new MPlotPoint(QPointF(x.first(), -5*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("5x Std Dev");
	plot_->addItem(newLine);
	items_ << newLine;

	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	plotView_->setPlot(plot_);
	plotView_->setFixedHeight(450);
	plotView_->setFixedWidth(600);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);
}

void CSRMainWindow::onRangeMinimumChanged()
{
	int minimum = rangeMinimum_->value();
	int maximum = rangeMaximum_->value();

	if (minimum >= maximum)
		return;

	QVector<double> x = QVector<double>(maximum-minimum+1, 0);
	QVector<double> y = QVector<double>(maximum-minimum+1, 0);
	model_->timeData(minimum, maximum, x.data());
	model_->data(minimum, maximum, y.data());
	seriesData_->setValues(x, y);

	foreach (MPlotItem *item, plot_->plotItems())
		item->setX(x.first());

	plot_->axisScaleBottom()->setDataRange(MPlotAxisRange(x.first(), x.last()));
}

void CSRMainWindow::onRangeMaximumChanged()
{
	int minimum = rangeMinimum_->value();
	int maximum = rangeMaximum_->value();

	if (minimum >= maximum)
		return;

	QVector<double> x = QVector<double>(maximum-minimum+1, 0);
	QVector<double> y = QVector<double>(maximum-minimum+1, 0);
	model_->timeData(minimum, maximum, x.data());
	model_->data(minimum, maximum, y.data());
	seriesData_->setValues(x, y);

	foreach (MPlotItem *item, plot_->plotItems())
		item->setX(x.first());

	plot_->axisScaleBottom()->setDataRange(MPlotAxisRange(x.first(), x.last()));
}
