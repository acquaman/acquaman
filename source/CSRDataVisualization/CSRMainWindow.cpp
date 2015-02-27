#include "CSRMainWindow.h"

#include <QVBoxLayout>
#include <QGroupBox>

#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include <QDebug>
CSRMainWindow::CSRMainWindow(CSRDataModel *model, QWidget *parent)
	: QWidget(parent)
{
	model_ = model;

	setupDataPlot();
	setupDisplayPlot();

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

	revolution_ = new QSpinBox;
	revolution_->setRange(0, 876);
	revolution_->setValue(0);
	revolution_->setPrefix("#: ");

	connect(revolution_, SIGNAL(editingFinished()), this, SLOT(onRevolutionChanged()));

	QHBoxLayout *editLayout = new QHBoxLayout;
	editLayout->addWidget(rangeMinimum_);
	editLayout->addWidget(rangeMaximum_);
	editLayout->addWidget(revolution_);

	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addWidget(new QLabel(QString("Mean: %1, Std dev: %2\n").arg(model_->mean(), 0, 'g').arg(model_->standardDeviation(), 0, 'g')));
	mainLayout->addWidget(dataPlotView_);
	mainLayout->addLayout(editLayout);

	setLayout(mainLayout);

	dataSelectionBox_ = new QComboBox;

	for (int i = 0; i < dataSeries_.size(); i++)
		dataSelectionBox_->addItem(dataSeries_.at(i)->description());

	connect(dataSelectionBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataSelectionChanged(int)));
	dataSelectionBox_->setCurrentIndex(0);

	QGroupBox *displayPlotBox = new QGroupBox;
	QVBoxLayout *displayPlotLayout = new QVBoxLayout;
	displayPlotLayout->addWidget(dataSelectionBox_, 0, Qt::AlignLeft);
	displayPlotLayout->addWidget(displayPlotView_);
	displayPlotBox->setLayout(displayPlotLayout);
	displayPlotBox->show();
}

CSRMainWindow::~CSRMainWindow()
{
	foreach (MPlotItem *item, dataPlot_->plotItems()){

		dataPlot_->removeItem(item);
		delete item;
	}

	foreach (MPlotItem *item, displayPlot_->plotItems()){

		displayPlot_->removeItem(item);
		delete item;
	}
}

void CSRMainWindow::setupDataPlot()
{
	// Create the plot window.
	dataPlotView_ = new MPlotWidget;
	dataPlotView_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	dataPlot_ = new MPlot;
	dataPlot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	dataPlot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	dataPlot_->axisBottom()->setAxisName("Time, s");
	dataPlot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	dataPlot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	dataPlot_->axisLeft()->setAxisName("Counts");

	// Set the margins for the plot.
	dataPlot_->setMarginLeft(10);
	dataPlot_->setMarginBottom(15);
	dataPlot_->setMarginRight(2);
	dataPlot_->setMarginTop(2);

	// Assumes that the dataSource() contains the spectrum most desired to view.
	MPlotSeriesBasic *csrSeries = new MPlotSeriesBasic;
	dataSeriesData_ = new MPlotVectorSeriesData;
	QVector<double> x = QVector<double>(3000, 0);
	QVector<double> y = QVector<double>(3000, 0);
	model_->timeData(0, 2999, x.data());
	model_->data(0, 2999, y.data());
	dataSeriesData_->setValues(x, y);
	csrSeries->setModel(dataSeriesData_, true);
	csrSeries->setMarker(MPlotMarkerShape::None);
	csrSeries->setDescription("CSR");
	csrSeries->setLinePen(QPen(Qt::red));
	dataPlot_->addItem(csrSeries);

	MPlotPoint *newLine = new MPlotPoint(QPointF(x.first(), -5*model_->standardDeviation()));
	newLine->setMarker(MPlotMarkerShape::HorizontalBeam, 1e6, QPen(Qt::blue), QBrush(Qt::blue));
	newLine->setDescription("5x Std Dev");
	dataPlot_->addItem(newLine);
	items_ << newLine;

	// Enable autoscaling of both axes.
	dataPlot_->axisScaleLeft()->setAutoScaleEnabled();
	dataPlot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	dataPlot_->addTool(new MPlotDragZoomerTool());
	dataPlot_->addTool(new MPlotWheelZoomerTool());
	dataPlotView_->setPlot(dataPlot_);
	dataPlotView_->setFixedHeight(450);
	dataPlotView_->setFixedWidth(600);

	// Set the number of ticks.  A balance between readability and being practical.
	dataPlot_->axisBottom()->setTicks(3);
	dataPlot_->axisTop()->setTicks(0);
	dataPlot_->axisLeft()->setTicks(4);
	dataPlot_->axisRight()->setTicks(0);
}

void CSRMainWindow::setupDisplayPlot()
{
	// Create the plot window.
	displayPlotView_ = new MPlotWidget;
	displayPlotView_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	displayPlot_ = new MPlot;
	displayPlot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	displayPlot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	displayPlot_->axisBottom()->setAxisName("Revolutions, #");
	displayPlot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	displayPlot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	displayPlot_->axisLeft()->setAxisName("Arb.");

	// Set the margins for the plot.
	displayPlot_->setMarginLeft(10);
	displayPlot_->setMarginBottom(15);
	displayPlot_->setMarginRight(2);
	displayPlot_->setMarginTop(2);

	// Assumes that the dataSource() contains the spectrum most desired to view.
	MPlotSeriesBasic *csrSeries = new MPlotSeriesBasic;
	integralSeriesData_ = new MPlotVectorSeriesData;
	QVector<double> x = QVector<double>(877, 0);

	for (int i = 0; i < 877; i++)
		x[i] = i;

	QVector<double> y = model_->integral();

	integralSeriesData_->setValues(x, y);
	csrSeries->setModel(integralSeriesData_, true);
	csrSeries->setMarker(MPlotMarkerShape::None);
	csrSeries->setDescription("Integral");
	csrSeries->setLinePen(QPen(Qt::red));
	dataSeries_ << csrSeries;

	csrSeries = new MPlotSeriesBasic;
	mainToMainSeriesData_ = new MPlotVectorSeriesData;
	x = QVector<double>(876, 0);

	for (int i = 0; i < 876; i++)
		x[i] = i;

	y = model_->mainPeakSeparation();

	mainToMainSeriesData_->setValues(x, y);
	csrSeries->setModel(mainToMainSeriesData_, true);
	csrSeries->setMarker(MPlotMarkerShape::None);
	csrSeries->setDescription("Main Peak Separation");
	csrSeries->setLinePen(QPen(Qt::blue));
	dataSeries_ << csrSeries;

	csrSeries = new MPlotSeriesBasic;
	mainPeakMaximaSeriesData_ = new MPlotVectorSeriesData;
	x = QVector<double>(877, 0);

	for (int i = 0; i < 877; i++)
		x[i] = i;

	y = model_->mainPeakMaxima();

	mainPeakMaximaSeriesData_->setValues(x, y);
	csrSeries->setModel(mainPeakMaximaSeriesData_, true);
	csrSeries->setMarker(MPlotMarkerShape::None);
	csrSeries->setDescription("Main Peak Maxima");
	csrSeries->setLinePen(QPen(Qt::red));
	dataSeries_ << csrSeries;

	csrSeries = new MPlotSeriesBasic;
	wakefieldPeakMaximaSeriesData_ = new MPlotVectorSeriesData;
	x = QVector<double>(877, 0);

	for (int i = 0; i < 877; i++)
		x[i] = i;

	y = model_->wakefieldPeakMaxima();

	wakefieldPeakMaximaSeriesData_->setValues(x, y);
	csrSeries->setModel(wakefieldPeakMaximaSeriesData_, true);
	csrSeries->setMarker(MPlotMarkerShape::None);
	csrSeries->setDescription("Wakefield Maxima");
	csrSeries->setLinePen(QPen(Qt::red));
	dataSeries_ << csrSeries;

	// Enable autoscaling of both axes.
	displayPlot_->axisScaleLeft()->setAutoScaleEnabled();
	displayPlot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	displayPlot_->addTool(new MPlotDragZoomerTool());
	displayPlot_->addTool(new MPlotWheelZoomerTool());
	displayPlotView_->setPlot(displayPlot_);
//	displayPlotView_->setFixedHeight(450);
//	displayPlotView_->setFixedWidth(600);

	// Set the number of ticks.  A balance between readability and being practical.
	displayPlot_->axisBottom()->setTicks(3);
	displayPlot_->axisTop()->setTicks(0);
	displayPlot_->axisLeft()->setTicks(4);
	displayPlot_->axisRight()->setTicks(0);
}


void CSRMainWindow::onRangeMinimumChanged()
{
	updatePlot();
}

void CSRMainWindow::onRangeMaximumChanged()
{
	updatePlot();
}

void CSRMainWindow::updatePlot()
{
	int minimum = rangeMinimum_->value();
	int maximum = rangeMaximum_->value();

	if (minimum >= maximum)
		return;

	QVector<double> x = QVector<double>(maximum-minimum+1, 0);
	QVector<double> y = QVector<double>(maximum-minimum+1, 0);
	model_->timeData(minimum, maximum, x.data());
	model_->data(minimum, maximum, y.data());
	dataSeriesData_->setValues(x, y);

	foreach (MPlotPoint *item, items_)
		item->setValue(QPointF(x.first(), item->value().y()));

	model_->findPeaks(minimum, maximum);

	QList<double> peakPositions = model_->peakPositions();

	foreach (MPlotPoint *point, peakMarkers_){

		dataPlot_->removeItem(point);
		delete point;
	}

	peakMarkers_.clear();

	for (int i = 0, size = peakPositions.size(); i < size; i++){

		MPlotPoint *newLine = new MPlotPoint(QPointF(peakPositions.at(i), 0));
		newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(Qt::black), QBrush(Qt::black));
		newLine->setDescription(QString("Peak - %1 s").arg(peakPositions.at(i)));
		dataPlot_->addItem(newLine);
		peakMarkers_ << newLine;
	}

	foreach (MPlotPoint *point, maximaMarkers_){

		dataPlot_->removeItem(point);
		delete point;
	}

	maximaMarkers_.clear();

	QList<QPointF> maximaPoints = model_->maximaPoints();

	for (int i = 0, size = maximaPoints.size(); i < size; i++){

		MPlotPoint *newLine = new MPlotPoint(maximaPoints.at(i));
		newLine->setMarker(MPlotMarkerShape::Square, 6, QPen(Qt::black), QBrush(Qt::black));
		newLine->setDescription(QString("Maxima (%1,%2)").arg(maximaPoints.at(i).x()).arg(maximaPoints.at(i).y()));
		dataPlot_->addItem(newLine);
		maximaMarkers_ << newLine;
	}
}

void CSRMainWindow::onRevolutionChanged()
{
	rangeMinimum_->setValue(revolution_->value()*22805);
	rangeMaximum_->setValue(qMin((revolution_->value()+1)*22805-1, int(2e7-1)));
	updatePlot();
}

void CSRMainWindow::onDataSelectionChanged(int index)
{
	MPlotItem *displayItem = displayPlot_->item(0);
	displayPlot_->removeItem(displayItem);
	displayPlot_->addItem(dataSeries_.at(index));
}
