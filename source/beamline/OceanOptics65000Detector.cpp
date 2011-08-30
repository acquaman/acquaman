/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "OceanOptics65000Detector.h"

OceanOptics65000Detector::OceanOptics65000Detector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMDetector::ReadMethod readMethod, QObject *parent) :
		OceanOptics65000DetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = false;
	initializeFromControlSet(readingsControls, settingsControls);
}

OceanOptics65000Detector::OceanOptics65000Detector(const QString &name, AMControl *dataWaveform, AMControl *integrationTime, AMDetector::ReadMethod readMethod, QObject *parent) :
		OceanOptics65000DetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = true;
	AMControlSet *readingsControls = new AMControlSet(this);
	AMControlSet *settingsControls = new AMControlSet(this);
	readingsControls->addControl(dataWaveform);
	settingsControls->addControl(integrationTime);
	initializeFromControlSet(readingsControls, settingsControls);
}

OceanOptics65000Detector::~OceanOptics65000Detector(){
	/* NTBA May 23, 2011 David Chevrier
	   Need to take care of ownsControlSet_
	   */
}

const QMetaObject* OceanOptics65000Detector::getMetaObject() {
	return metaObject();
}

double OceanOptics65000Detector::reading() const{
	if(isConnected())
		return dataWaveformControl()->value();
	else
		return -1;
}

AMDetectorInfo* OceanOptics65000Detector::toInfo() const{
	return new OceanOptics65000DetectorInfo(*this);
}

OceanOptics65000DetectorInfo OceanOptics65000Detector::toOceanOptics65000Info() const{
	return OceanOptics65000DetectorInfo(*this);
}

bool OceanOptics65000Detector::setFromInfo(const AMDetectorInfo *info){
	const OceanOptics65000DetectorInfo *di = qobject_cast<const OceanOptics65000DetectorInfo*>(info);
	if(!di)
		return false;
	integrationTimeControl()->move(di->integrationTime());
	return true;
}

bool OceanOptics65000Detector::setFromInfo(const OceanOptics65000DetectorInfo& info){
	integrationTimeControl()->move(info.integrationTime());
	return true;
}

AMControl* OceanOptics65000Detector::dataWaveformControl() const{
	if(isConnected())
		return readingsControls_->at(0);
	else
		return 0; //NULL
}

AMControl* OceanOptics65000Detector::integrationTimeControl() const{
	if(isConnected())
		return settingsControls_->at(0);
	else
		return 0; //NULL
}

bool OceanOptics65000Detector::settingsMatchFbk(OceanOptics65000DetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->integrationTime() - integrationTimeControl()->value()) > integrationTimeControl()->tolerance() )
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

QString OceanOptics65000Detector::description() const{
	return AMDetectorInfo::description().replace(' ', '\n');
}

void OceanOptics65000Detector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

bool OceanOptics65000Detector::setControls(OceanOptics65000DetectorInfo *settings){
	integrationTimeControl()->move( settings->integrationTime() );
	return true;
}

void OceanOptics65000Detector::onControlsConnected(bool connected){
	Q_UNUSED(connected)
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void OceanOptics65000Detector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setIntegrationTime(integrationTimeControl()->value());
		emitSettingsChanged();
	}
}

void OceanOptics65000Detector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

bool OceanOptics65000Detector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL

	if(readingsControls->count() == 1 && settingsControls->count() == 1){
		readingsControls_ = readingsControls;
		settingsControls_ = settingsControls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
		connect(readingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(readingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onReadingsControlValuesChanged()));
		connect(settingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onSettingsControlValuesChanged()));
		onControlsConnected(readingsControls_->isConnected() && settingsControls_->isConnected());
		if(isConnected()){
			onReadingsControlValuesChanged();
			onSettingsControlValuesChanged();
			return true;
		}
	}
	return false;
}
