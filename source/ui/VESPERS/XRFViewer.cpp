#include "XRFViewer.h"
#include "DeadTimeButton.h"
#include "MPlot/MPlotAxisScale.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "dataman/AMDataSourceSeriesData.h"
#include "MPlot/MPlotTools.h"

#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>

XRFViewer::XRFViewer(QWidget *parent) :
	QWidget(parent)
{
	elapsedTime_ = new QLabel(tr(" s"));
	deadTime_ = new QLabel(tr(" %"));

	// Using a button group so I know which element I need to disable.
	DeadTimeButton *temp;
	deadTimeGroup_ = new QButtonGroup(this);
	deadTimeGroup_->setExclusive(false);
	QHBoxLayout *deadTimeLayout = new QHBoxLayout;
	deadTimeLayout->setSpacing(0);

	for (int i = 0; i < detector_->elements(); i++){

		temp = new DeadTimeButton(15.0, 30.0);
		temp->setCheckable(true);
		temp->setFixedSize(20, 20);
		deadTimeLayout->addWidget(temp);
		deadTimeGroup_->addButton(temp, i);
	}

	if (detector_->elements() == 1)
		deadTimeGroup_->button(0)->setCheckable(false);
	else
		connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

	isWaterfall_ = false;

	setupPlot();

	QFont font(this->font());
	font.setBold(true);

	QLabel *elapsedTimeLabel = new QLabel(QString("Elapsed Time"));
	elapsedTimeLabel->setFont(font);
	QLabel *deadTimeLabel = new QLabel(QString("Dead Time"));
	deadTimeLabel->setFont(font);

	QLabel *logLabel = new QLabel(QString("Scale Axis"));
	logLabel->setFont(font);

	logButton_ = new QPushButton("Log");
	logButton_->setCheckable(true);
	logButton_->setChecked(false);
	connect(logButton_, SIGNAL(toggled(bool)), this, SLOT(onLogEnabled(bool)));

	waterfallButton_ = new QPushButton("Raw Spectra");
	waterfallButton_->setCheckable(true);
	connect(waterfallButton_, SIGNAL(toggled(bool)), this, SLOT(onWaterfallToggled(bool)));

	waterfallSeparation_ = new QDoubleSpinBox;
	waterfallSeparation_->setPrefix(QString::fromUtf8("Δh = "));
	waterfallSeparation_->setMinimum(0.0);
	waterfallSeparation_->setMaximum(1.0);
	waterfallSeparation_->setSingleStep(0.01);
	waterfallSeparation_->setValue(0.2);
	waterfallSeparation_->setAlignment(Qt::AlignCenter);
	waterfallSeparation_->setEnabled(false);
	connect(waterfallSeparation_, SIGNAL(valueChanged(double)), this, SLOT(onWaterfallSeparationChanged(double)));
	connect(waterfallButton_, SIGNAL(toggled(bool)), waterfallSeparation_, SLOT(setEnabled(bool)));
}

void XRFViewer::setupPlot()
{
	// Create the plot window.
	view_ = new MPlotWidget;
	view_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	plot_ = new MPlot;
	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Energy, eV");
	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setAxisName("Counts");

	// Set the margins for the plot.
	plot_->setMarginLeft(10);
	plot_->setMarginBottom(15);
	plot_->setMarginRight(2);
	plot_->setMarginTop(2);

	MPlotSeriesBasic *series;

	for (int i = 0; i < detector_->elements(); i++){

		series = new MPlotSeriesBasic;
		series->setModel(new AMDataSourceSeriesData(detector_->spectrumDataSource(i)));
		series->setMarker(MPlotMarkerShape::None);
		series->setDescription(detector_->spectrumDataSource(i)->name());
		series->setLinePen(QPen(getColor(i+1)));
		rawDataSeries_ << series;
	}

	corrSum_ = new MPlotSeriesBasic;
	corrSum_->setModel(new AMDataSourceSeriesData(detector_->correctedSumDataSource()));
	corrSum_->setMarker(MPlotMarkerShape::None);
	corrSum_->setDescription(detector_->correctedSumDataSource()->name());
	corrSum_->setLinePen(QPen(getColor(0)));
	plot_->addItem(corrSum_);

	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	view_->setPlot(plot_);
	view_->setMinimumHeight(450);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);

	// Set the autoscale constraints.
	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, 1e20));
}

void XRFViewer::elementClicked(int elementId)
{
	// If the button is checked then it means the element should be disabled.
	/*if (deadTimeGroup_->button(elementId)->isChecked())

	else
		*/
}

void XRFViewer::onLogEnabled(bool logged)
{
	logged ? logButton_->setText("Linear") : logButton_->setText("Log");
	plot_->axisScaleLeft()->setLogScaleEnabled(logged);
}

void XRFViewer::onWaterfallToggled(bool isWaterfall)
{
	isWaterfall ? waterfallButton_->setText("Sum Spectra") : waterfallButton_->setText("Raw Spectra");
	isWaterfall_ = isWaterfall;

	if (isWaterfall){

		plot_->removeItem(corrSum_);

		for (int i = 0; i < detector_->elements(); i++)
			plot_->insertItem(rawDataSeries_.at(i), i);

		plot_->setAxisScaleWaterfall(MPlot::Left, waterfallSeparation_->value()*getMaximumHeight(plot_->item(0)));
	}
	else{

		for (int i = 0; i < detector_->elements(); i++)
			plot_->removeItem(rawDataSeries_.at(i));

		plot_->insertItem(corrSum_, 0);
		plot_->setAxisScaleWaterfall(MPlot::Left, 0);
	}
}

double XRFViewer::getMaximumHeight(MPlotItem *data)
{
	double max = 0;
	MPlotAbstractSeries *temp = qgraphicsitem_cast<MPlotAbstractSeries *>(data);

	if (!temp)
		return max;

	AMDataSourceSeriesData *model = (AMDataSourceSeriesData *)temp->model();

	for (int i = 0; i < model->count(); i++){

		if (max < model->y(i))
			max = model->y(i);
	}

	return max;
}

QColor XRFViewer::getColor(int index)
{
	int i = index%10;

	switch(i){
	case 0:
		return Qt::red;
	case 1:
		return Qt::blue;
	case 2:
		return Qt::green;
	case 3:
		return Qt::darkCyan;
	case 4:
		return Qt::black;
	case 5:
		return QColor(255, 127, 0);
	case 6:
		return Qt::magenta;
	case 7:
		return Qt::cyan;
	case 8:
		return Qt::darkRed;
	case 9:
		return Qt::darkBlue;
	}

	return QColor(100, 100, 100);
}
