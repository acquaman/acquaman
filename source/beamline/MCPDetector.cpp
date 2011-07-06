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


#include "MCPDetector.h"

MCPDetector::MCPDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = false;
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	initializeFromControlSet(readingsControls, settingsControls);
}


MCPDetector::MCPDetector(const QString &name, AMControl *reading, AMControl *hv, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = true;
	AMControlSet *readingsControls = new AMControlSet(this);
	readingsControls->addControl(reading);
	AMControlSet *settingsControls = new AMControlSet(this);
	settingsControls->addControl(hv);
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	initializeFromControlSet(readingsControls, settingsControls);
}

MCPDetector::~MCPDetector(){
	/* NTBA March 14, 2011 David Chevrier
	   Need to take care of ownsControlSet_
	*/
}

const QMetaObject* MCPDetector::getMetaObject() {
	return metaObject();
}

double MCPDetector::reading() const{
	if(isConnected())
		return readingsControls_->at(0)->value();
	else
		return -1;
}

AMDetectorInfo* MCPDetector::toInfo() const{
	return new MCPDetectorInfo(*this);
}

MCPDetectorInfo MCPDetector::toMCPInfo() const{
	return MCPDetectorInfo(*this);
}

bool MCPDetector::isPoweredOn(){
	return poweredOn_;
}

AMControl* MCPDetector::readingCtrl() const {
	if(isConnected())
		return readingsControls_->at(0);
	return 0;
}

AMControl* MCPDetector::hvCtrl() const {
	if(isConnected())
		return settingsControls_->at(0);
	return 0;
}

bool MCPDetector::setFromInfo(const AMDetectorInfo *info){
	const MCPDetectorInfo *di = qobject_cast<const MCPDetectorInfo*>(info);
	if(!di)
		return false;
	hvCtrl()->move(di->hvSetpoint());
	return true;
}

bool MCPDetector::setFromInfo(const MCPDetectorInfo& info){
	hvCtrl()->move(info.hvSetpoint());
	return true;
}

bool MCPDetector::activate(){
	hvCtrl()->move(1600);
	return true;
}

AMBeamlineActionItem* MCPDetector::turnOnAction(){
	return toggleOnAction_->createCopy();
}

bool MCPDetector::settingsMatchFbk(MCPDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->hvSetpoint() - hvCtrl()->value()) > hvCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

QString MCPDetector::description() const{
	return AMDetectorInfo::description();
}

bool MCPDetector::setControls(MCPDetectorInfo *mcpSettings){
//	hvCtrl()->move( mcpSettings->hvSetpoint() );
	return true;
}

void MCPDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void MCPDetector::onControlsConnected(bool connected){
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void MCPDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

void MCPDetector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvCtrl()->value());
		bool lastPoweredOn = poweredOn_;
		poweredOn_ = (hvCtrl()->value() >= 1200);
		if(poweredOn_ != lastPoweredOn)
			emit poweredOnChanged(poweredOn_);
		emitSettingsChanged();
	}
}

void MCPDetector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL
	poweredOn_ = false;

	if(readingsControls->count() == 1 && settingsControls->count() == 1){
		readingsControls_ = readingsControls;
		settingsControls_ = settingsControls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
		connect(readingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(readingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onReadingsControlValuesChanged()));
		connect(settingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onSettingsControlValuesChanged()));
		onControlsConnected(readingsControls_->isConnected()&&settingsControls_->isConnected());
		if(isConnected()){
			onReadingsControlValuesChanged();
			onSettingsControlValuesChanged();
		}
	}
}
