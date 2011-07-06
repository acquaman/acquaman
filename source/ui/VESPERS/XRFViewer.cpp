#include "XRFViewer.h"
#include "ui/VESPERS/DeadTimeButton.h"
#include "MPlot/MPlotAxisScale.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "dataman/AMDataSourceSeriesData.h"
#include "MPlot/MPlotTools.h"

#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QButtonGroup>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QGroupBox>

XRFViewer::XRFViewer(QWidget *parent) :
	QWidget(parent)
{
	corrSum_ = 0;

	elapsedTime_ = new QLabel(tr(" s"));
	deadTime_ = new QLabel(tr(" %"));

	// Using a button group so I know which element I need to disable.
	deadTimeGroup_ = new QButtonGroup(this);
	deadTimeGroup_->setExclusive(false);

	DeadTimeButton *temp = new DeadTimeButton(30.0, 60.0);
	temp->setCheckable(true);
	temp->setFixedSize(20, 20);
	deadTimeGroup_->addButton(temp, 0);
	connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

	deadTimeLayout_ = new QHBoxLayout;
	deadTimeLayout_->setSpacing(0);
	deadTimeLayout_->addWidget(temp);

	setupPlot();

	QFont font(this->font());
	font.setBold(true);

	QPushButton *loadSpectraButton = new QPushButton("Load XRF Spectra File");
	connect(loadSpectraButton, SIGNAL(clicked()), this, SLOT(loadFile()));

	QLabel *elapsedTimeLabel = new QLabel(QString("Elapsed Time"));
	elapsedTimeLabel->setFont(font);
	QLabel *deadTimeLabel = new QLabel(QString("Dead Time"));
	deadTimeLabel->setFont(font);
	QLabel *roiLabel = new QLabel("Regions of Interest");
	roiLabel->setFont(font);

	roiList_ = new QTextEdit;
	roiList_->setReadOnly(true);
	roiList_->setFixedWidth(350);

	logButton_ = new QPushButton("Log");
	logButton_->setCheckable(true);
	logButton_->setChecked(false);
	connect(logButton_, SIGNAL(toggled(bool)), this, SLOT(onLogEnabled(bool)));

	QPushButton *rawSpectraButton = new QPushButton("Raw Spectra");
	rawSpectraButton->setCheckable(true);

	QPushButton *correctedSpectraButton = new QPushButton("Corrected Spectra");
	correctedSpectraButton->setCheckable(true);

	QPushButton *correctedSumButton = new QPushButton("Corrected Sum");
	correctedSumButton->setCheckable(true);

	QButtonGroup *spectraGroup = new QButtonGroup(this);
	spectraGroup->addButton(correctedSumButton, 0);
	spectraGroup->addButton(correctedSpectraButton, 1);
	spectraGroup->addButton(rawSpectraButton, 2);
	connect(spectraGroup, SIGNAL(buttonClicked(int)), this, SLOT(onSpectraGroupClicked(int)));
	correctedSumButton->setChecked(true);

	waterfallSeparation_ = new QDoubleSpinBox;
	waterfallSeparation_->setPrefix(QString::fromUtf8("Δh = "));
	waterfallSeparation_->setMinimum(0.0);
	waterfallSeparation_->setMaximum(1.0);
	waterfallSeparation_->setSingleStep(0.01);
	waterfallSeparation_->setValue(0.2);
	waterfallSeparation_->setAlignment(Qt::AlignCenter);
	waterfallSeparation_->setEnabled(false);
	connect(waterfallSeparation_, SIGNAL(valueChanged(double)), this, SLOT(onWaterfallSeparationChanged(double)));

	QFormLayout *rightSide = new QFormLayout;
	rightSide->setHorizontalSpacing(60);
	rightSide->addRow(loadSpectraButton);
	rightSide->addRow(elapsedTimeLabel, elapsedTime_);
	rightSide->addRow(deadTimeLabel, deadTime_);
	rightSide->addRow(deadTimeLayout_);
	rightSide->addRow(roiLabel);
	rightSide->addRow(roiList_);

	QGroupBox *rightSideBox = new QGroupBox;
	rightSideBox->setLayout(rightSide);
	rightSideBox->setMaximumWidth(400);

	QHBoxLayout *bottomRow = new QHBoxLayout;
	bottomRow->addWidget(correctedSumButton);
	bottomRow->addWidget(correctedSpectraButton);
	bottomRow->addWidget(rawSpectraButton);
	bottomRow->addWidget(waterfallSeparation_);
	bottomRow->addWidget(logButton_);

	QVBoxLayout *plotAndBottom = new QVBoxLayout;
	plotAndBottom->addWidget(view_);
	plotAndBottom->addLayout(bottomRow);

	QHBoxLayout *fullLayout = new QHBoxLayout;
	fullLayout->addLayout(plotAndBottom);
	fullLayout->addWidget(rightSideBox);

	setLayout(fullLayout);
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

	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	view_->setPlot(plot_);
	view_->setMinimumSize(700, 450);

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
	Q_UNUSED(elementId)

}

void XRFViewer::onLogEnabled(bool logged)
{
	logged ? logButton_->setText("Linear") : logButton_->setText("Log");
	plot_->axisScaleLeft()->setLogScaleEnabled(logged);
}

void XRFViewer::onSpectraGroupClicked(int id)
{
	if (corrSum_ == 0 || corrDataSeries_.isEmpty() || rawDataSeries_.isEmpty())
		return;

	while(plot_->numItems())
		plot_->removeItem(plot_->item(0));
	plot_->setAxisScaleWaterfall(MPlot::Left, 0);

	switch(id){

	case 0:
		plot_->addItem(corrSum_);
		waterfallSeparation_->setEnabled(false);
		break;
	case 1:
		for (int i = 0; i < corrDataSeries_.size(); i++)
			plot_->addItem(corrDataSeries_.at(i));
		plot_->setAxisScaleWaterfall(MPlot::Left, waterfallSeparation_->value()*getMaximumHeight(plot_->item(0)));
		waterfallSeparation_->setEnabled(true);
		break;
	case 2:
		for (int i = 0; i < rawDataSeries_.size(); i++)
			plot_->addItem(rawDataSeries_.at(i));
		plot_->setAxisScaleWaterfall(MPlot::Left, waterfallSeparation_->value()*getMaximumHeight(plot_->item(0)));
		waterfallSeparation_->setEnabled(true);
		break;
	}
}

double XRFViewer::getMaximumHeight(MPlotItem *data)
{
	double max = 0;
	MPlotAbstractSeries *temp = qgraphicsitem_cast<MPlotAbstractSeries *>(data);

	if (!temp)
		return max;

	SpectrumData *model = (SpectrumData *)temp->model();

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

void XRFViewer::loadFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
													tr("Load Spectra"),
													"",
													tr("Spectra Data (*.dat);;All Files (*)"));

	QFile file(fileName);

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("XRF Spectra Viewer"),
							 tr("Cannot open file %1:\n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return;
	}

	// Clean up first.
	while(!deadTimeLayout_->isEmpty())
		deadTimeLayout_->removeItem(deadTimeLayout_->itemAt(0));
	while(!deadTimeGroup_->buttons().isEmpty())
		delete deadTimeGroup_->buttons().takeLast();
	for (int i = 0; i < rawDataSeries_.size(); i++){

		delete rawDataSeries_.at(i);
		delete corrDataSeries_.at(i);
	}

	rawDataSeries_.clear();
	corrDataSeries_.clear();
	roiList_->clear();

	QTextStream in(&file);

	int elements = in.readLine().split("\t").last().toInt();
	double scale = in.readLine().split("\t").last().toDouble();
	elapsedTime_->setText(in.readLine().split("\t").last());

	double icr = 0;
	double ocr = 0;
	double worstDeadTime = 0;
	double dt = 0;
	DeadTimeButton *temp;
	deadTimeLayout_->addStretch();

	for (int i = 0; i < elements; i++){

		icr = in.readLine().split("\t").last().toDouble();
		ocr = in.readLine().split("\t").last().toDouble();
		dt = (icr/ocr - 1)*100;

		if (dt > worstDeadTime)
			worstDeadTime = dt;

		temp = new DeadTimeButton(30.0, 60.0);
		temp->setCurrent(dt);
		temp->setFixedSize(20, 20);
		deadTimeGroup_->addButton(temp, i);
		deadTimeLayout_->addWidget(temp);
	}

	deadTimeLayout_->addStretch();
	deadTime_->setText(QString::number(worstDeadTime, 'f', 2) + " %");

	QString current;
	in.readLine();
	roiList_->append(in.readLine()+"\n");

	while ((current = in.readLine()).compare("Spectra") != 0)
		roiList_->append(current);

	QList<QList<double> > rawData;
	QList<QList<double> > corrData;
	QList<double> corrSumData;

	for (int i = 0; i < elements; i++){

		rawData << QList<double>();
		corrData << QList<double>();
	}

	in.readLine();
	in.readLine();

	while(!(current = in.readLine()).isEmpty()){

		QStringList temp(current.split("\t"));

		for (int i = 0; i < elements; i++){

			rawData[i] << temp.at(i).toDouble();
			corrData[i] << temp.at(i+elements).toDouble();
		}
		corrSumData << temp.last().toInt();
	}

	file.close();

	MPlotSeriesBasic *rawSeries;
	MPlotSeriesBasic *corrSeries;
	SpectrumData *tempData;

	for (int i = 0; i < elements; i++){

		tempData = new SpectrumData(rawData.at(i));
		tempData->setScale(scale);
		rawSeries = new MPlotSeriesBasic;
		rawSeries->setModel(tempData, true);
		rawSeries->setMarker(MPlotMarkerShape::None);
		rawSeries->setDescription(QString("Raw %1").arg(i+1));
		rawSeries->setLinePen(QPen(getColor(i+1)));
		rawDataSeries_ << rawSeries;

		tempData = new SpectrumData(corrData.at(i));
		tempData->setScale(scale);
		corrSeries = new MPlotSeriesBasic;
		corrSeries->setModel(tempData, true);
		corrSeries->setMarker(MPlotMarkerShape::None);
		corrSeries->setDescription(QString("Corrected %1").arg(i+1));
		corrSeries->setLinePen(QPen(getColor(i+1)));
		corrDataSeries_ << corrSeries;
	}

	tempData = new SpectrumData(corrSumData);
	tempData->setScale(scale);
	corrSum_ = new MPlotSeriesBasic;
	corrSum_->setModel(tempData, true);
	corrSum_->setMarker(MPlotMarkerShape::None);
	corrSum_->setDescription("Corrected Sum");
	corrSum_->setLinePen(QPen(getColor(0)));

	onSpectraGroupClicked(0);
}
