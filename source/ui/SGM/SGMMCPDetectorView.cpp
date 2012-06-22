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


#include "SGMMCPDetectorView.h"

SGMMCPBriefDetectorView::SGMMCPBriefDetectorView(SGMMCPDetector *detector, bool configureOnly, QWidget *parent) :
	AMBriefDetectorView(configureOnly, parent)
{
	hl_ = 0;
	readingCE_ = 0;
	powerState_ = 0;
	detector_ = 0;
	powerOnState_ = QIcon(":/power_green.png");
	powerOffState_ = QIcon(":/power_red.png");
	setDetector(detector, configureOnly_);
}

AMDetector* SGMMCPBriefDetectorView::detector(){
	return detector_;
}

void SGMMCPBriefDetectorView::onPoweredOnChanged(bool poweredOn){
	if(poweredOn)
		powerState_->setIcon(powerOnState_);
	else
		powerState_->setIcon(powerOffState_);
}

bool SGMMCPBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(detector_)
		disconnect(detector_, SIGNAL(poweredOnChanged(bool)), this, SLOT(onPoweredOnChanged(bool)));
	if(!detector)
		return false;
	detector_ = static_cast<SGMMCPDetector*>(detector);
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
	if(powerState_){
		hl_->removeWidget(powerState_);
		delete powerState_;
		powerState_ = 0;
	}
	readingCE_ = new AMExtendedControlEditor(detector_->readingCtrl(), 0, true);
	powerState_ = new QToolButton();
	powerState_->setIcon(powerOffState_);
//	powerState_->setEnabled(false);
	hl_->addWidget(readingCE_);
	hl_->addWidget(powerState_);
	connect(detector_, SIGNAL(poweredOnChanged(bool)), this, SLOT(onPoweredOnChanged(bool)));
	if(detector_->isConnected())
		onPoweredOnChanged(detector_->isPoweredOn());
	return true;
}


SGMMCPDetailedDetectorView::SGMMCPDetailedDetectorView(SGMMCPDetector *detector, bool configureOnly, QWidget *parent) :
	AMDetailedDetectorView(configureOnly, parent)
{
	gl_ = 0;
	readingCE_ = 0;
	hvCE_ = 0;
	detector_ = 0;
	configurationSettings_ = 0;
	setDetector(detector, configureOnly_);
}

AMDetector* SGMMCPDetailedDetectorView::detector(){
	return detector_;
}

AMDetectorInfo* SGMMCPDetailedDetectorView::configurationSettings() const{
	return configurationSettings_;
}

void SGMMCPDetailedDetectorView::onControlSetpointRequested(){
	if(detector_ && detector_->isConnected()){
		configurationSettings_->setHVSetpoint(hvCE_->setpoint());
		emit settingsConfigureRequested();
	}
}

bool SGMMCPDetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;
	detector_ = static_cast<SGMMCPDetector*>(detector);
	if(configurationSettings_)
		configurationSettings_->deleteLater();
	configurationSettings_ = qobject_cast<SGMMCPDetectorInfo*>(detector_->toNewInfo());
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
	if(hvCE_){
		gl_->removeWidget(hvCE_);
		delete hvCE_;
		hvCE_ = 0;
	}
	readingCE_ = new AMExtendedControlEditor(detector_->readingCtrl(), 0, true);
	hvCE_ = new AMExtendedControlEditor(detector_->hvCtrl(), 0, false, configureOnly_);
	gl_->addWidget(new QLabel("TFY"),	0, 0, 1, 1, 0);
	gl_->addWidget(readingCE_,		0, 1, 1, 2, 0);
	gl_->addWidget(new QLabel("HV"),	1, 0, 1, 1, 0);
	gl_->addWidget(hvCE_,			1, 1, 1, 2, 0);
	connect(hvCE_, SIGNAL(setpointRequested(double)), this, SLOT(onControlSetpointRequested()));
	return true;
}
