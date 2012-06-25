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

CLSOceanOptics65000DetailedDetectorView::CLSOceanOptics65000DetailedDetectorView(CLSOceanOptics65000Detector *detector, bool configureOnly, QWidget *parent) :
	AMDetailedDetectorView(configureOnly, parent)
{
	gl_ = 0;
	readingCE_ = 0;
	integrationTimeCE_ = 0;
	detector_ = 0;
	configurationSettings_ = 0;
	setDetector(detector, configureOnly_);
}

AMDetector* CLSOceanOptics65000DetailedDetectorView::detector(){
	return detector_;
}

AMDetectorInfo* CLSOceanOptics65000DetailedDetectorView::configurationSettings() const{
	return configurationSettings_;
}

void CLSOceanOptics65000DetailedDetectorView::onControlSetpointRequested(){
	if(detector_ && detector_->isConnected()){
		configurationSettings_->setIntegrationTime(integrationTimeCE_->setpoint());
		emit settingsConfigureRequested();
	}
}

bool CLSOceanOptics65000DetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;
	detector_ = static_cast<CLSOceanOptics65000Detector*>(detector);
	if(configurationSettings_)
		configurationSettings_->deleteLater();
	// This NEWs a OceanOptics65000DetectorInfo, I'm responsible for it now
	configurationSettings_ = qobject_cast<CLSOceanOptics65000DetectorInfo*>(detector_->toNewInfo());
	configureOnly_ = configureOnly;
	if(!gl_){
		gl_ = new QGridLayout();
		setLayout(gl_);
	}
	if(readingCE_){
		gl_->removeWidget(readingCE_);
		delete readingCE_;
		readingCE_ = 0;
	}
	if(integrationTimeCE_){
		gl_->removeWidget(integrationTimeCE_);
		delete integrationTimeCE_;
		integrationTimeCE_ = 0;
	}
	readingCE_ = new AMExtendedControlEditor(detector_->dataWaveformControl());
	readingCE_->setControlFormat('f', 0);
	integrationTimeCE_ = new AMExtendedControlEditor(detector_->integrationTimeControl(), 0, false, configureOnly_);
	gl_->addWidget(new QLabel("QE65000"),	0, 0, 1, 1, 0);
	gl_->addWidget(readingCE_,		0, 1, 1, 2, 0);
	gl_->addWidget(new QLabel("Time"),	1, 0, 1, 1, 0);
	gl_->addWidget(integrationTimeCE_,	1, 1, 1, 2, 0);
	connect(integrationTimeCE_, SIGNAL(setpointRequested(double)), this, SLOT(onControlSetpointRequested()));
	return true;
}
