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


#include "CLSAmptekSDD123DetectorView.h"

//#include "util/VESPERS/GeneralUtilities.h"
//#include "MPlot/MPlotAxisScale.h"
//#include "MPlot/MPlotTools.h"
//#include "dataman/datasource/AMDataSourceSeriesData.h"

//CLSAmptekSDD123BriefDetectorView::CLSAmptekSDD123BriefDetectorView(CLSAmptekSDD123Detector *detector, bool configureOnly, QWidget *parent) :
//	AMBriefOldDetectorView(configureOnly, parent)
//{
//	detector_ = 0;
//	setDetector(detector, configureOnly);
//}

//AMOldDetector* CLSAmptekSDD123BriefDetectorView::detector(){
//	return detector_;
//}

//bool CLSAmptekSDD123BriefDetectorView::setDetector(AMOldDetector *detector, bool configureOnly){
//	/* NTBA - April 3rd, 2012 (David Chevrier)
//	Need to implement the configureOnly part.
//	*/
//	Q_UNUSED(configureOnly)
//	if(!detector)
//		return false;

//	detector_ = static_cast<CLSAmptekSDD123Detector*>(detector);

//	QLabel *name = new QLabel(detector_->name());

//	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
//	connect(detector_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));

//	QHBoxLayout *briefLayout = new QHBoxLayout;
//	briefLayout->addWidget(name);
//	setLayout(briefLayout);

//	return true;
//}

//CLSAmptekSDD123DetailedDetectorView::CLSAmptekSDD123DetailedDetectorView(CLSAmptekSDD123Detector *detector, bool configureOnly, QWidget *parent) :
//	AMDetailedOldDetectorView(configureOnly, parent)
//{
//	detector_ = 0;
//	configurationSettings_ = 0;
//	setDetector(detector, configureOnly);
//}

//AMOldDetector* CLSAmptekSDD123DetailedDetectorView::detector(){
//	return detector_;
//}

//AMOldDetectorInfo* CLSAmptekSDD123DetailedDetectorView::configurationSettings() const{
//	return configurationSettings_;
//}

//MPlot* CLSAmptekSDD123DetailedDetectorView::plot() const{
//	return plot_;
//}

//bool CLSAmptekSDD123DetailedDetectorView::setDetector(AMOldDetector *detector, bool configureOnly){
//	/* NTBA - April 3rd, 2012 (David Chevrier)
//	Need to implement the configureOnly part.
//	*/
//	Q_UNUSED(configureOnly)
//	if(!detector)
//		return false;

//	detector_ = static_cast<CLSAmptekSDD123Detector*>(detector);
//	if(configurationSettings_)
//		configurationSettings_->deleteLater();
//	configurationSettings_ = qobject_cast<CLSAmptekSDD123DetectorInfo*>(detector_->toNewInfo());
//	connect(detector_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
//	connect(detector_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));
//	connect(detector_, SIGNAL(integrationTimeChanged(double)), this, SLOT(onIntegrationTimeChanged(double)));
//	connect(detector_, SIGNAL(detectorTemperatureChanged(double)), this, SLOT(onDetectorTemperatureChanged(double)));
//	connect(detector_, SIGNAL(mcaChannelsChanged(double)), this, SLOT(onMCAChannelsChanged(double)));
//	connect(detector_, SIGNAL(totalCountsChanged(double)), this, SLOT(onTotalCountsChanged(double)));

//	statusLabel_ = new QLabel();
//	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));

//	integrationTimeDSB_ = new QDoubleSpinBox();
//	integrationTimeDSB_->setMinimum(0.1);
//	integrationTimeDSB_->setMaximum(10);
//	totalCountsDSB_ = new QDoubleSpinBox();
//	totalCountsDSB_->setMinimum(0);
//	totalCountsDSB_->setMaximum(500000);
//	totalCountsDSB_->setEnabled(false);
//	detectorTemperatureLabel_ = new QLabel();
//	mcaChannelLabel_ = new QLabel();
//	startAcquisitionButton_ = new QPushButton("Start");
//	startAcquisitionButton_->setEnabled(false);
//	enabledCheckBox_ = new QCheckBox("Enabled");


//	if(detector_->isConnected()){
//		integrationTimeDSB_->setValue(detector_->integrationTime());
//		detectorTemperatureLabel_->setText(QString("%1").arg(detector_->detectorTemperature()));
//		mcaChannelLabel_->setText(QString("%1").arg(detector_->channels()));
//		startAcquisitionButton_->setEnabled(true);
//		totalCountsDSB_->setValue(detector_->spectraTotalCounts());
//		enabledCheckBox_->setChecked(detector_->isEnabled());
//	}
//	else{
//		detectorTemperatureLabel_->setText("?");
//		mcaChannelLabel_->setText("?");
//	}

//	connect(startAcquisitionButton_, SIGNAL(clicked()), this, SLOT(onStartAcquisitionButtonClicked()));
//	connect(integrationTimeDSB_, SIGNAL(editingFinished()), this, SLOT(onIntegrationTimeDSBEditingFinished()));
//	connect(enabledCheckBox_, SIGNAL(toggled(bool)), detector_, SLOT(setEnabled(bool)));
//	connect(detector_, SIGNAL(enabledChanged(bool)), enabledCheckBox_, SLOT(setChecked(bool)));

//	// Create the plot window.
//	view_ = new MPlotWidget;
//	view_->enableAntiAliasing(true);

//	// Create the plot and setup all the axes.
//	plot_ = new MPlot;
//	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
//	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
//	plot_->axisBottom()->setAxisName("Energy, eV");
//	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
//	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
//	plot_->axisLeft()->setAxisName("Counts");

//	// Set the margins for the plot.
//	plot_->setMarginLeft(10);
//	plot_->setMarginBottom(15);
//	plot_->setMarginRight(2);
//	plot_->setMarginTop(2);

//	MPlotSeriesBasic *spectrumData_ = new MPlotSeriesBasic;
//	spectrumData_->setModel(new AMDataSourceSeriesData(detector_->spectrumDataSource()), true);
//	spectrumData_->setMarker(MPlotMarkerShape::None);
//	spectrumData_->setDescription(detector_->spectrumDataSource()->name());
//	spectrumData_->setLinePen(QPen(Qt::blue));
//	plot_->addItem(spectrumData_);

//	// Enable autoscaling of both axes.
//	plot_->axisScaleLeft()->setAutoScaleEnabled();
//	plot_->axisScaleBottom()->setAutoScaleEnabled();

//	// Enable some convenient zoom tools.
//	plot_->addTool(new MPlotDragZoomerTool());
//	plot_->addTool(new MPlotWheelZoomerTool());
//	view_->setPlot(plot_);
//	view_->setMinimumHeight(450);

//	// Set the number of ticks.  A balance between readability and being practical.
//	plot_->axisBottom()->setTicks(3);
//	plot_->axisTop()->setTicks(0);
//	plot_->axisLeft()->setTicks(4);
//	plot_->axisRight()->setTicks(0);

//	// Set the autoscale constraints.
//	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));

//	QVBoxLayout *mainVL = new QVBoxLayout();
//	QHBoxLayout *mainHL = new QHBoxLayout();

//	QVBoxLayout *tmpVL;

//	mainHL->addWidget(startAcquisitionButton_);
//	tmpVL = new QVBoxLayout();
//	tmpVL->addWidget(new QLabel("Dwell"));
//	tmpVL->addWidget(integrationTimeDSB_);
//	mainHL->addLayout(tmpVL);
//	tmpVL = new QVBoxLayout();
//	tmpVL->addWidget(new QLabel("Counts"));
//	tmpVL->addWidget(totalCountsDSB_);
//	mainHL->addLayout(tmpVL);
//	tmpVL = new QVBoxLayout();
//	tmpVL->addWidget(new QLabel("Temp"));
//	tmpVL->addWidget(detectorTemperatureLabel_);
//	mainHL->addLayout(tmpVL);
//	tmpVL = new QVBoxLayout();
//	tmpVL->addWidget(new QLabel("Channels"));
//	tmpVL->addWidget(mcaChannelLabel_);
//	mainHL->addLayout(tmpVL);
//	mainHL->addWidget(statusLabel_);
//	mainHL->addWidget(enabledCheckBox_);

//	mainVL->addLayout(mainHL);
//	mainVL->addWidget(view_);

//	setLayout(mainVL);

//	return true;
//}

//void CLSAmptekSDD123DetailedDetectorView::onConnectedChanged(bool connected){
//	setEnabled(connected);
//	if(connected){
//		integrationTimeDSB_->setValue(detector_->integrationTime());
//		totalCountsDSB_->setValue(detector_->spectraTotalCounts());
//		detectorTemperatureLabel_->setText(QString("%1").arg(detector_->detectorTemperature()));
//		mcaChannelLabel_->setText(QString("%1").arg(detector_->channels()));
//		startAcquisitionButton_->setEnabled(true);
//	}
//}

//void CLSAmptekSDD123DetailedDetectorView::onStatusChanged(bool status){
//	if(status)
//		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));
//	else
//		statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
//}

//void CLSAmptekSDD123DetailedDetectorView::onIntegrationTimeChanged(double integrationTime){
//	integrationTimeDSB_->setValue(integrationTime);
//}

//void CLSAmptekSDD123DetailedDetectorView::onDetectorTemperatureChanged(double detectorTemperature){
//	detectorTemperatureLabel_->setText(QString("%1").arg(detectorTemperature));
//}

//void CLSAmptekSDD123DetailedDetectorView::onMCAChannelsChanged(double mcaChannels){
//	mcaChannelLabel_->setText(QString("%1").arg(mcaChannels));
//}

//void CLSAmptekSDD123DetailedDetectorView::onTotalCountsChanged(double totalCounts){
//	totalCountsDSB_->setValue(totalCounts);
//}

//void CLSAmptekSDD123DetailedDetectorView::onStartAcquisitionButtonClicked(){
//	detector_->start();
//}

//void CLSAmptekSDD123DetailedDetectorView::onIntegrationTimeDSBEditingFinished(){
//	detector_->setIntegrationTime(integrationTimeDSB_->value());
//}
