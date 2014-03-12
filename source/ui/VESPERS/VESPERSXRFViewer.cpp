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


#include "VESPERSXRFViewer.h"
#include "ui/VESPERS/VESPERSDeadTimeButton.h"
#include "MPlot/MPlotAxisScale.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "MPlot/MPlotTools.h"
#include "util/AMDataSourcePlotSettings.h"

#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QButtonGroup>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QGroupBox>
#include <QStringBuilder>

 VESPERSXRFViewer::~VESPERSXRFViewer(){}
VESPERSXRFViewer::VESPERSXRFViewer(QWidget *parent) :
	QWidget(parent)
{
	corrSum_ = 0;

	elapsedTime_ = new QLabel(tr(" s"));
	deadTime_ = new QLabel(tr(" %"));

	// Using a button group so I know which element I need to disable.
	deadTimeGroup_ = new QButtonGroup(this);
	deadTimeGroup_->setExclusive(false);

	VESPERSDeadTimeButton *temp;
	deadTimeLayout_ = new QHBoxLayout;
	deadTimeLayout_->setSpacing(0);
	deadTimeLayout_->addStretch();

	for (int i = 0; i < 4; i++){

		temp = new VESPERSDeadTimeButton(30.0, 60.0);
		temp->setCheckable(false);
		temp->setCurrent(0);
		temp->setFixedSize(20, 20);
		temp->hide();
		deadTimeGroup_->addButton(temp, i);
		deadTimeLayout_->addWidget(temp, 0, Qt::AlignCenter);
	}

	deadTimeLayout_->addStretch();

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

	rawSpectraButton_ = new QPushButton("Raw Spectra");
	rawSpectraButton_->setCheckable(true);
	rawSpectraButton_->setEnabled(false);

	correctedSpectraButton_ = new QPushButton("Corrected Spectra");
	correctedSpectraButton_->setCheckable(true);
	correctedSpectraButton_->setEnabled(false);

	correctedSumButton_ = new QPushButton("Corrected Sum");
	correctedSumButton_->setCheckable(true);
	correctedSumButton_->setEnabled(false);

	QButtonGroup *spectraGroup = new QButtonGroup(this);
	spectraGroup->addButton(correctedSumButton_, 0);
	spectraGroup->addButton(correctedSpectraButton_, 1);
	spectraGroup->addButton(rawSpectraButton_, 2);
	connect(spectraGroup, SIGNAL(buttonClicked(int)), this, SLOT(onSpectraGroupClicked(int)));
	correctedSumButton_->setChecked(true);

	waterfallSeparation_ = new QDoubleSpinBox;
	waterfallSeparation_->setPrefix(QString::fromUtf8("Δh = "));
	waterfallSeparation_->setMinimum(0.0);
	waterfallSeparation_->setMaximum(1.0);
	waterfallSeparation_->setSingleStep(0.01);
	waterfallSeparation_->setValue(0.2);
	waterfallSeparation_->setAlignment(Qt::AlignCenter);
	waterfallSeparation_->setEnabled(false);
	connect(waterfallSeparation_, SIGNAL(valueChanged(double)), this, SLOT(onWaterfallSeparationChanged(double)));

	minimum_ = new QDoubleSpinBox;
	minimum_->setRange(0, 30);
	minimum_->setDecimals(2);
	minimum_->setValue(3.11);
	minimum_->setSuffix(" keV");

	maximum_ = new QDoubleSpinBox;
	maximum_->setRange(0, 30);
	maximum_->setDecimals(2);
	maximum_->setValue(20.48);
	maximum_->setSuffix(" keV");

	connect(minimum_, SIGNAL(valueChanged(double)), this, SLOT(setMinimum(double)));
	connect(maximum_, SIGNAL(valueChanged(double)), this, SLOT(setMaximum(double)));

	QHBoxLayout *minMaxLayout = new QHBoxLayout;
	minMaxLayout->addWidget(new QLabel("Minimum:"));
	minMaxLayout->addWidget(minimum_);
	minMaxLayout->addWidget(new QLabel("Maximum:"));
	minMaxLayout->addWidget(maximum_);
	minMaxLayout->addStretch();

	table_ = new AMSelectablePeriodicTable(this);
	table_->buildPeriodicTable();
	connect(table_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));
	connect(table_, SIGNAL(elementDeselected(AMElement*)), this, SLOT(onElementDeselected(AMElement*)));
	tableView_ = new AMSelectablePeriodicTableView(table_);
	tableView_->buildPeriodicTableView();

	setPlotRange(3110, 20480);

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
	bottomRow->addWidget(correctedSumButton_);
	bottomRow->addWidget(correctedSpectraButton_);
	bottomRow->addWidget(rawSpectraButton_);
	bottomRow->addWidget(waterfallSeparation_);
	bottomRow->addWidget(logButton_);

	QVBoxLayout *plotAndBottom = new QVBoxLayout;
	plotAndBottom->addWidget(view_);
	plotAndBottom->addLayout(bottomRow);
	plotAndBottom->addLayout(minMaxLayout);
	plotAndBottom->addWidget(tableView_, 0, Qt::AlignCenter);

	QHBoxLayout *fullLayout = new QHBoxLayout;
	fullLayout->addLayout(plotAndBottom);
	fullLayout->addWidget(rightSideBox);

	setLayout(fullLayout);
}

void VESPERSXRFViewer::onElementSelected(AMElement *element)
{
	QList<AMEmissionLine> lines = element->emissionLines();
	QColor color = AMDataSourcePlotSettings::nextColor();
	MPlotPoint *newLine;

	foreach(AMEmissionLine line, lines){

		if (range_.withinRange(line.energy())
				&& line.lineName().contains("1") && line.name().compare("-"))	{

			newLine = new MPlotPoint(QPointF(line.energy(), 0));
			newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(color), QBrush(color));
			newLine->setDescription(line.greekName());
			plot_->addItem(newLine);
		}
	}
}

void VESPERSXRFViewer::onElementDeselected(AMElement *element)
{
	QString symbol = element->symbol();

	foreach(MPlotItem *item, plot_->plotItems()){

		if (item->description().contains(symbol))
			if (plot_->removeItem(item))
				delete item;
	}
}

void VESPERSXRFViewer::setPlotRange(double low, double high)
{
	range_ = AMRange(low, high);
	tableView_->setEnergyRange(low, high);

	foreach(AMElement *element, table_->selectedElements())
		onElementDeselected(element);

	foreach(AMElement *element, table_->selectedElements())
		onElementSelected(element);
}

void VESPERSXRFViewer::setMinimum(double min)
{
	setPlotRange(min*1000, range_.maximum());
}

void VESPERSXRFViewer::setMaximum(double max)
{
	setPlotRange(range_.minimum(), max*1000);
}

void VESPERSXRFViewer::setupPlot()
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
	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));
}

void VESPERSXRFViewer::onLogEnabled(bool logged)
{
	logged ? logButton_->setText("Linear") : logButton_->setText("Log");
	plot_->axisScaleLeft()->setLogScaleEnabled(logged);
}

void VESPERSXRFViewer::onSpectraGroupClicked(int id)
{
	if ((corrSum_ == 0 && id == 0) || (corrDataSeries_.isEmpty() && id == 1) || (rawDataSeries_.isEmpty() && id == 2))
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

double VESPERSXRFViewer::getMaximumHeight(MPlotItem *data)
{
	double max = 0;
	MPlotAbstractSeries *temp = qgraphicsitem_cast<MPlotAbstractSeries *>(data);

	if (!temp)
		return max;

	VESPERSSpectrumData *model = (VESPERSSpectrumData *)temp->model();

	for (int i = 0; i < model->count(); i++){

		if (max < model->y(i))
			max = model->y(i);
	}

	return max;
}

QColor VESPERSXRFViewer::getColor(int index)
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

void VESPERSXRFViewer::loadFile()
{
	// Clean up first.
	for (int i = 0; i < deadTimeGroup_->buttons().size(); i++)
		deadTimeGroup_->button(i)->hide();
	for (int i = 0; i < rawDataSeries_.size(); i++)
		delete rawDataSeries_.at(i);
	for (int i = 0; i < corrDataSeries_.size(); i++)
		delete corrDataSeries_.at(i);

	delete corrSum_;
	corrSum_ = 0;
	rawDataSeries_.clear();
	corrDataSeries_.clear();
	roiList_->clear();

	rawSpectraButton_->setEnabled(false);
	correctedSpectraButton_->setEnabled(false);
	correctedSumButton_->setEnabled(false);

	// End of clean up.

	QString filename;

	if ((filename = QFileDialog::getOpenFileName(this, tr("Load Spectra"), "", tr("Spectra Data (*.dat);;All Files (*)"))).isNull())
		return;

	switch(checkDataFile(filename)){

	case None:
		QMessageBox::warning(this, tr("XRF Spectra Viewer"), tr("This data file is not a VESPERS data file and is not supported."));
		break;
	case SpectrumSnapshot:
		loadSpectrumSnapshotFile(filename);
		break;
	case VespersXRF:
		loadVespersXRFFile(filename);
		break;
	case AcquamanXRF:
		loadAcquamanXRFFile(filename);
		break;
	}
}

VESPERSXRFViewer::FileType VESPERSXRFViewer::checkDataFile(QString filename)
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("XRF Spectra Viewer"),
							 tr("Cannot open file %1:\n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return None;
	}

	QTextStream in(&file);
	QString current(in.readLine());
	file.close();

	FileType type;

	if (current.contains("#"))
		type = SpectrumSnapshot;
	else if (current.contains("Number of Elements:"))
		type = AcquamanXRF;
	else if (current.split("\t").size() == 2 || current.split("\t").size() == 6)
		type = VespersXRF;
	else
		type = None;

	return type;
}

void VESPERSXRFViewer::loadSpectrumSnapshotFile(QString filename)
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("XRF Spectra Viewer"),
							 tr("Cannot open file %1:\n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return;
	}

	QTextStream in(&file);

	in.readLine();
	in.readLine();
	in.readLine();

	QString current(in.readLine());
	elapsedTime_->setText(current.split(" ").at(4) + " s");
	deadTime_->setText("--");
	roiList_->setText("Not available.");

	QList<double> data;

	current = in.readLine();
	double firstNum = current.split(" ").first().toDouble()*1000;
	data << current.split(" ").last().toDouble();
	current = in.readLine();
	double secondNum = current.split(" ").first().toDouble()*1000;
	data << current.split(" ").last().toDouble();

	while(!(current = in.readLine()).isEmpty())
		data << current.split(" ").last().toDouble();

	file.close();

	// Put the data into MPlot data series.
	VESPERSSpectrumData *tempData = new VESPERSSpectrumData(data);
	tempData->setScale(secondNum-firstNum);
	corrSum_ = new MPlotSeriesBasic;
	corrSum_->setModel(tempData, true);
	corrSum_->setMarker(MPlotMarkerShape::None);
	corrSum_->setDescription("Corrected Sum");
	corrSum_->setLinePen(QPen(getColor(0)));

	plot_->axisBottom()->setAxisName("Energy, eV");

	correctedSumButton_->setEnabled(true);

	onSpectraGroupClicked(0);
}

void VESPERSXRFViewer::loadVespersXRFFile(QString filename)
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("XRF Spectra Viewer"),
							 tr("Cannot open file %1:\n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return;
	}

	elapsedTime_->setText("--");
	deadTime_->setText("--");
	roiList_->setText("Not available.");

	QTextStream in(&file);

	QString current(in.readLine());

	// If the data file is from the single element.
	if (current.split("\t").size() == 2){

		QList<double> rawData;

		while(!(current = in.readLine()).isEmpty())
			rawData << current.split("\t").last().toDouble();

		MPlotSeriesBasic *rawSeries = new MPlotSeriesBasic;
		rawSeries->setModel(new VESPERSSpectrumData(rawData), true);
		rawSeries->setMarker(MPlotMarkerShape::None);
		rawSeries->setDescription(QString("Raw 1"));
		rawSeries->setLinePen(QPen(getColor(1)));
		rawDataSeries_ << rawSeries;

		rawSpectraButton_->setEnabled(true);

		onSpectraGroupClicked(2);
	}
	// If the data file is from the four element.
	else{

		QList<QList<double> > rawData;
		QList<double> corrSumData;

		for (int i = 0; i < 4; i++)
			rawData << QList<double>();

		while(!(current = in.readLine()).isEmpty()){

			QStringList temp(current.split("\t"));

			for (int i = 0; i < 4; i++)
				rawData[i] << temp.at(i+2).toDouble();

			corrSumData << temp.at(1).toDouble();
		}

		MPlotSeriesBasic *rawSeries;
		VESPERSSpectrumData *tempData;

		for (int i = 0; i < 4; i++){

			tempData = new VESPERSSpectrumData(rawData.at(i));
			rawSeries = new MPlotSeriesBasic;
			rawSeries->setModel(tempData, true);
			rawSeries->setMarker(MPlotMarkerShape::None);
			rawSeries->setDescription(QString("Raw %1").arg(i+1));
			rawSeries->setLinePen(QPen(getColor(i+1)));
			rawDataSeries_ << rawSeries;
		}

		tempData = new VESPERSSpectrumData(corrSumData);
		corrSum_ = new MPlotSeriesBasic;
		corrSum_->setModel(tempData, true);
		corrSum_->setMarker(MPlotMarkerShape::None);
		corrSum_->setDescription("Corrected Sum");
		corrSum_->setLinePen(QPen(getColor(0)));

		plot_->axisBottom()->setAxisName("Channel #");

		rawSpectraButton_->setEnabled(true);
		correctedSumButton_->setEnabled(true);

		onSpectraGroupClicked(0);
	}

	file.close();
}

void VESPERSXRFViewer::loadAcquamanXRFFile(QString filename)
{
	QFile file(filename);

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("XRF Spectra Viewer"),
							 tr("Cannot open file %1:\n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return;
	}

	QTextStream in(&file);

	int elements = in.readLine().split("\t").last().toInt();
	double scale = in.readLine().split("\t").last().toDouble();
	elapsedTime_->setText(in.readLine().split("\t").last());

	double icr = 0;
	double ocr = 0;
	double worstDeadTime = 0;
	double dt = 0;
	VESPERSDeadTimeButton *temp;

	for (int i = 0; i < elements; i++){

		icr = in.readLine().split("\t").last().toDouble();
		ocr = in.readLine().split("\t").last().toDouble();
		dt = (1 - ocr/icr)*100;

		if (dt > worstDeadTime)
			worstDeadTime = dt;

		temp = qobject_cast<VESPERSDeadTimeButton *>(deadTimeGroup_->button(i));
		temp->setCurrent(dt);
		temp->show();
	}

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
		corrSumData << temp.last().toDouble();
	}

	file.close();

	MPlotSeriesBasic *rawSeries;
	MPlotSeriesBasic *corrSeries;
	VESPERSSpectrumData *tempData;

	for (int i = 0; i < elements; i++){

		tempData = new VESPERSSpectrumData(rawData.at(i));
		tempData->setScale(scale);
		rawSeries = new MPlotSeriesBasic;
		rawSeries->setModel(tempData, true);
		rawSeries->setMarker(MPlotMarkerShape::None);
		rawSeries->setDescription(QString("Raw %1").arg(i+1));
		rawSeries->setLinePen(QPen(getColor(i+1)));
		rawDataSeries_ << rawSeries;

		tempData = new VESPERSSpectrumData(corrData.at(i));
		tempData->setScale(scale);
		corrSeries = new MPlotSeriesBasic;
		corrSeries->setModel(tempData, true);
		corrSeries->setMarker(MPlotMarkerShape::None);
		corrSeries->setDescription(QString("Corrected %1").arg(i+1));
		corrSeries->setLinePen(QPen(getColor(i+1)));
		corrDataSeries_ << corrSeries;
	}

	tempData = new VESPERSSpectrumData(corrSumData);
	tempData->setScale(scale);
	corrSum_ = new MPlotSeriesBasic;
	corrSum_->setModel(tempData, true);
	corrSum_->setMarker(MPlotMarkerShape::None);
	corrSum_->setDescription("Corrected Sum");
	corrSum_->setLinePen(QPen(getColor(0)));

	rawSpectraButton_->setEnabled(true);
	correctedSpectraButton_->setEnabled(true);
	correctedSumButton_->setEnabled(true);

	plot_->axisBottom()->setAxisName("Energy, eV");

	onSpectraGroupClicked(0);
}
 VESPERSSpectrumData::~VESPERSSpectrumData(){}
