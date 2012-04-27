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


#include "CLSOceanOptics65000Detector.h"

CLSOceanOptics65000Detector::CLSOceanOptics65000Detector(const QString &name, const QString &baseName, AMDetector::ReadMethod readMethod, QObject *parent) :
	CLSOceanOptics65000DetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	allControls_ = new AMControlSet();

	dataWaveformControl_ = new AMReadOnlyWaveformBinningPVControl(name+"Spectrum", baseName+":DarkCorrectedSpectra", 0, 1024, this);
	dataWaveformControl_->setDescription("OceanOptics 65000 Spectrum");
	dataWaveformControl_->setContextKnownDescription("Spectrum");
	integrationTimeControl_ = new AMPVControl(name+"IntegrationTime", baseName+":IntegrationTime:Value", baseName+":IntegrationTime:Value", "", this, 0.1);
	integrationTimeControl_->setDescription("OceanOptics 65000 Integration Time");
	integrationTimeControl_->setContextKnownDescription("Integration Time");

	allControls_->addControl(dataWaveformControl_);
	allControls_->addControl(integrationTimeControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
	connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
	connect(dataWaveformControl_, SIGNAL(valueChanged(double)), this, SLOT(onReadingsControlValuesChanged()));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onSettingsControlValuesChanged()));
}

CLSOceanOptics65000Detector::~CLSOceanOptics65000Detector(){
}

const QMetaObject* CLSOceanOptics65000Detector::getMetaObject() {
	return metaObject();
}

QString CLSOceanOptics65000Detector::dacqName() const{
	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(dataWaveformControl_);
	if(isConnected() && tmpControl)
		return tmpControl->readPVName();
	else
		return "";
}

double CLSOceanOptics65000Detector::reading() const{
	if(isConnected())
		return dataWaveformControl()->value();
	else
		return -1;
}

AMDetectorInfo* CLSOceanOptics65000Detector::toInfo() const{
	return new CLSOceanOptics65000DetectorInfo(*this);
}

CLSOceanOptics65000DetectorInfo CLSOceanOptics65000Detector::toOceanOptics65000Info() const{
	return CLSOceanOptics65000DetectorInfo(*this);
}

bool CLSOceanOptics65000Detector::setFromInfo(const AMDetectorInfo *info){
	const CLSOceanOptics65000DetectorInfo *di = qobject_cast<const CLSOceanOptics65000DetectorInfo*>(info);
	if(!di)
		return false;
	integrationTimeControl()->move(di->integrationTime());
	return true;
}

bool CLSOceanOptics65000Detector::setFromInfo(const CLSOceanOptics65000DetectorInfo& info){
	integrationTimeControl()->move(info.integrationTime());
	return true;
}

AMControl* CLSOceanOptics65000Detector::dataWaveformControl() const{
	if(isConnected())
		return dataWaveformControl_;
	else
		return 0; //NULL
}

AMControl* CLSOceanOptics65000Detector::integrationTimeControl() const{
	if(isConnected())
		return integrationTimeControl_;
	else
		return 0; //NULL
}

bool CLSOceanOptics65000Detector::settingsMatchFbk(CLSOceanOptics65000DetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->integrationTime() - integrationTimeControl()->value()) > integrationTimeControl()->tolerance() )
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

QString CLSOceanOptics65000Detector::description() const{
	return AMDetectorInfo::description().replace(' ', '\n');
}

void CLSOceanOptics65000Detector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

bool CLSOceanOptics65000Detector::setControls(CLSOceanOptics65000DetectorInfo *settings){
	integrationTimeControl()->move( settings->integrationTime() );
	return true;
}

void CLSOceanOptics65000Detector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void CLSOceanOptics65000Detector::onControlsTimedOut(){
	setTimedOut();
}

void CLSOceanOptics65000Detector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setIntegrationTime(integrationTimeControl()->value());
		emitSettingsChanged();
	}
}

void CLSOceanOptics65000Detector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}
