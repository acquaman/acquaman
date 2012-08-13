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


#include "CLSOceanOptics65000DetectorView.h"

#include "util/VESPERS/GeneralUtilities.h"
#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

CLSOceanOptics65000BriefDetectorView::CLSOceanOptics65000BriefDetectorView(CLSOceanOptics65000Detector *detector, bool configureOnly, QWidget *parent) :
		AMBriefDetectorView(configureOnly, parent)
{
	hl_ = 0;
	readingCE_ = 0;
	detector_ = 0;
	setDetector(detector, configureOnly_);
}

AMDetector* CLSOceanOptics65000BriefDetectorView::detector(){
	return detector_;
}

bool CLSOceanOptics65000BriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;
	detector_ = static_cast<CLSOceanOptics65000Detector*>(detector);
	configureOnly_ = configureOnly;
	if(!hl_){
		hl_ = new QHBoxLayout();
		setLayout(hl_);
	}
	if(readingCE_){
		hl_->removeWidget(readingCE_);
		delete readingCE_;
		readingCE_ = 0;
	}
	readingCE_ = new AMExtendedControlEditor(detector_->dataWaveformControl(), 0, true);
	readingCE_->setControlFormat('f', 0);
	hl_->addWidget(readingCE_);
	return true;
}

//CLSOceanOptics65000DetailedDetectorView::CLSOceanOptics65000DetailedDetectorView(CLSOceanOptics65000Detector *detector, bool configureOnly, QWidget *parent) :
//	AMDetailedDetectorView(configureOnly, parent)
//{
//	gl_ = 0;
//	readingCE_ = 0;
//	integrationTimeCE_ = 0;
//	detector_ = 0;
//	configurationSettings_ = 0;
//	setDetector(detector, configureOnly_);
//}

//AMDetector* CLSOceanOptics65000DetailedDetectorView::detector(){
//	return detector_;
//}

//AMDetectorInfo* CLSOceanOptics65000DetailedDetectorView::configurationSettings() const{
//	return configurationSettings_;
//}

//void CLSOceanOptics65000DetailedDetectorView::onControlSetpointRequested(){
//	if(detector_ && detector_->isConnected()){
//		configurationSettings_->setIntegrationTime(integrationTimeCE_->setpoint());
//		emit settingsConfigureRequested();
//	}
//}

//bool CLSOceanOptics65000DetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
//	if(!detector)
//		return false;
//	detector_ = static_cast<CLSOceanOptics65000Detector*>(detector);
//	if(configurationSettings_)
//		configurationSettings_->deleteLater();
//	// This NEWs a OceanOptics65000DetectorInfo, I'm responsible for it now
//	configurationSettings_ = qobject_cast<CLSOceanOptics65000DetectorInfo*>(detector_->toNewInfo());
//	configureOnly_ = configureOnly;
//	if(!gl_){
//		gl_ = new QGridLayout();
//		setLayout(gl_);
//	}
//	if(readingCE_){
//		gl_->removeWidget(readingCE_);
//		delete readingCE_;
//		readingCE_ = 0;
//	}
//	if(integrationTimeCE_){
//		gl_->removeWidget(integrationTimeCE_);
//		delete integrationTimeCE_;
//		integrationTimeCE_ = 0;
//	}
//	readingCE_ = new AMExtendedControlEditor(detector_->dataWaveformControl());
//	readingCE_->setControlFormat('f', 0);
//	integrationTimeCE_ = new AMExtendedControlEditor(detector_->integrationTimeControl(), 0, false, configureOnly_);
//	gl_->addWidget(new QLabel("QE65000"),	0, 0, 1, 1, 0);
//	gl_->addWidget(readingCE_,		0, 1, 1, 2, 0);
//	gl_->addWidget(new QLabel("Time"),	1, 0, 1, 1, 0);
//	gl_->addWidget(integrationTimeCE_,	1, 1, 1, 2, 0);
//	connect(integrationTimeCE_, SIGNAL(setpointRequested(double)), this, SLOT(onControlSetpointRequested()));
//	return true;
//}

CLSOceanOptics65000DetailedDetectorView::CLSOceanOptics65000DetailedDetectorView(CLSOceanOptics65000Detector *detector, bool configureOnly, QWidget *parent) :
	AMDetailedDetectorView(configureOnly, parent)
{
	detector_ = 0;
	configurationSettings_ = 0;
	setDetector(detector, configureOnly);
}

AMDetector* CLSOceanOptics65000DetailedDetectorView::detector(){
	return detector_;
}

AMDetectorInfo* CLSOceanOptics65000DetailedDetectorView::configurationSettings() const{
	return configurationSettings_;
}

MPlot* CLSOceanOptics65000DetailedDetectorView::plot() const{
	return plot_;
}

bool CLSOceanOptics65000DetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	/* NTBA - April 3rd, 2012 (David Chevrier)
	Need to implement the configureOnly part.
	*/
	Q_UNUSED(configureOnly)
	if(!detector)
		return false;

	detector_ = static_cast<CLSOceanOptics65000Detector*>(detector);
	if(configurationSettings_)
		configurationSettings_->deleteLater();
	configurationSettings_ = qobject_cast<CLSOceanOptics65000DetectorInfo*>(detector_->toNewInfo());
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
	connect(detector_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));
	connect(detector_, SIGNAL(totalCountsChanged(double)), this, SLOT(onTotalCountsChanged(double)));

	statusLabel_ = new QLabel();
	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));

	integrationTimeCE_ = new AMExtendedControlEditor(detector_->integrationTimeControl(), 0, false, false);
	totalCountsDSB_ = new QDoubleSpinBox();
	totalCountsDSB_->setMinimum(0);
	totalCountsDSB_->setMaximum(10000000);
	totalCountsDSB_->setEnabled(false);
	startAcquisitionButton_ = new QPushButton("Start");
	startAcquisitionButton_->setEnabled(false);
	connect(startAcquisitionButton_, SIGNAL(clicked()), this, SLOT(onStartAcquisitionButtonClicked()));

	if(detector_->isConnected()){
		startAcquisitionButton_->setEnabled(true);
		totalCountsDSB_->setValue(detector_->spectraTotalCounts());
	}

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

	MPlotSeriesBasic *spectrumData_ = new MPlotSeriesBasic;
	spectrumData_->setModel(new AMDataSourceSeriesData(detector_->spectrumDataSource()), true);
	spectrumData_->setMarker(MPlotMarkerShape::None);
	spectrumData_->setDescription(detector_->spectrumDataSource()->name());
	spectrumData_->setLinePen(QPen(Qt::blue));
	plot_->addItem(spectrumData_);

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
	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));

	QVBoxLayout *mainVL = new QVBoxLayout();
	QHBoxLayout *mainHL = new QHBoxLayout();

	mainHL->addWidget(startAcquisitionButton_);
	mainHL->addWidget(integrationTimeCE_);
	QVBoxLayout *tempVL = new QVBoxLayout();
	tempVL->addWidget(new QLabel("Counts"));
	tempVL->addWidget(totalCountsDSB_);
	mainHL->addLayout(tempVL);
	//mainHL->addWidget(totalCountsDSB_);
	mainHL->addWidget(statusLabel_);

	mainVL->addLayout(mainHL);
	mainVL->addWidget(view_);

	setLayout(mainVL);

	return true;
}

void CLSOceanOptics65000DetailedDetectorView::onConnectedChanged(bool connected){
	setEnabled(connected);
	if(connected){
		totalCountsDSB_->setValue(detector_->spectraTotalCounts());
		startAcquisitionButton_->setEnabled(true);
	}
}

void CLSOceanOptics65000DetailedDetectorView::onStatusChanged(bool status){
	if(status)
		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));
	else
		statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
}

void CLSOceanOptics65000DetailedDetectorView::onTotalCountsChanged(double totalCounts){
	totalCountsDSB_->setValue(totalCounts);
}

void CLSOceanOptics65000DetailedDetectorView::onStartAcquisitionButtonClicked(){
	detector_->start();
}
