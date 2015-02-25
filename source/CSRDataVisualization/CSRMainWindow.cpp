#include "CSRMainWindow.h"

#include <QVBoxLayout>

#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"

CSRMainWindow::CSRMainWindow(CSRDataModel *model, QWidget *parent)
	: QWidget(parent)
{
	model_ = model;

	setupPlot();

	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addWidget(new QLabel(QString("Mean: %1, Std dev: %2\n").arg(model_->mean(), 0, 'g').arg(model_->standardDeviation(), 0, 'g')));
	mainLayout->addWidget(plotView_);

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
	MPlotVectorSeriesData *seriesData = new MPlotVectorSeriesData;
	QVector<double> x = QVector<double>(2e7, 0);
	QVector<double> y = QVector<double>(2e7, 0);
	model_->timeData(x.data());
	model_->data(y.data());
	seriesData->setValues(x, y);
	csrSeries->setModel(seriesData, true);
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

	MPlotPoint *newLine = new MPlotPoint(QPointF(x.first(), model_->mean()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("Mean");
	plot_->addItem(newLine);

	newLine = new MPlotPoint(QPointF(x.first(), -1*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("Std Dev");
	plot_->addItem(newLine);

	newLine = new MPlotPoint(QPointF(x.first(), -2*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("2x Std Dev");
	plot_->addItem(newLine);

	newLine = new MPlotPoint(QPointF(x.first(), -3*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("3x Std Dev");
	plot_->addItem(newLine);

	newLine = new MPlotPoint(QPointF(x.first(), -4*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("4x Std Dev");
	plot_->addItem(newLine);

	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	plotView_->setPlot(plot_);
	plotView_->setMinimumHeight(450);
	plotView_->setMinimumWidth(600);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);

	// Set the autoscale constraints.
	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(MPLOT_NEG_INFINITY, MPLOT_POS_INFINITY));
}
