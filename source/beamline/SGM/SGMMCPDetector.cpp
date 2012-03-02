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


#include "SGMMCPDetector.h"

SGMMCPDetector::SGMMCPDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		SGMMCPDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = false;
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	initializeFromControlSet(readingsControls, settingsControls);
}


SGMMCPDetector::SGMMCPDetector(const QString &name, AMControl *reading, AMControl *hv, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		SGMMCPDetectorInfo(name, name, parent), AMDetector(name, readMethod)
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

SGMMCPDetector::~SGMMCPDetector(){
	/* NTBA March 14, 2011 David Chevrier
	   Need to take care of ownsControlSet_
	*/
}

const QMetaObject* SGMMCPDetector::getMetaObject() {
	return metaObject();
}

double SGMMCPDetector::reading() const{
	if(isConnected())
		return readingsControls_->at(0)->value();
	else
		return -1;
}

AMDetectorInfo* SGMMCPDetector::toInfo() const{
	return new SGMMCPDetectorInfo(*this);
}

SGMMCPDetectorInfo SGMMCPDetector::toMCPInfo() const{
	return SGMMCPDetectorInfo(*this);
}

bool SGMMCPDetector::isPoweredOn(){
	return poweredOn_;
}

AMControl* SGMMCPDetector::readingCtrl() const {
	if(isConnected())
		return readingsControls_->at(0);
	return 0;
}

AMControl* SGMMCPDetector::hvCtrl() const {
	if(isConnected())
		return settingsControls_->at(0);
	return 0;
}

bool SGMMCPDetector::setFromInfo(const AMDetectorInfo *info){
	const SGMMCPDetectorInfo *di = qobject_cast<const SGMMCPDetectorInfo*>(info);
	if(!di)
		return false;
	hvCtrl()->move(di->hvSetpoint());
	return true;
}

bool SGMMCPDetector::setFromInfo(const SGMMCPDetectorInfo& info){
	hvCtrl()->move(info.hvSetpoint());
	return true;
}

bool SGMMCPDetector::activate(){
	if(!hvCtrl()->withinTolerance(1600))
		hvCtrl()->move(1600);
	return true;
}

AMBeamlineActionItem* SGMMCPDetector::turnOnAction(){
	return toggleOnAction_->createCopy();
}

bool SGMMCPDetector::settingsMatchFbk(SGMMCPDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->hvSetpoint() - hvCtrl()->value()) > hvCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

QString SGMMCPDetector::description() const{
	return AMDetectorInfo::description();
}

bool SGMMCPDetector::setControls(SGMMCPDetectorInfo *mcpSettings){
	Q_UNUSED(mcpSettings)
//	hvCtrl()->move( mcpSettings->hvSetpoint() );
	return true;
}

void SGMMCPDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void SGMMCPDetector::onControlsConnected(bool connected){
	Q_UNUSED(connected)
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void SGMMCPDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

void SGMMCPDetector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvCtrl()->value());
		bool lastPoweredOn = poweredOn_;
		poweredOn_ = (hvCtrl()->value() >= 1200);
		if(poweredOn_ != lastPoweredOn)
			emit poweredOnChanged(poweredOn_);
		emitSettingsChanged();
	}
}

void SGMMCPDetector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL
	poweredOn_ = false;

	if(readingsControls->count() == 1 && settingsControls->count() == 1){
		readingsControls_ = readingsControls;
		settingsControls_ = settingsControls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
		connect(readingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(readingsControls_, SIGNAL(controlSetValuesChanged()), this, SLOT(onReadingsControlValuesChanged()));
		connect(settingsControls_, SIGNAL(controlSetValuesChanged()), this, SLOT(onSettingsControlValuesChanged()));
		onControlsConnected(readingsControls_->isConnected()&&settingsControls_->isConnected());
		if(isConnected()){
			onReadingsControlValuesChanged();
			onSettingsControlValuesChanged();
		}
	}
}
