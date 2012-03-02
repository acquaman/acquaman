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


#include "CLSPGTDetector.h"

CLSPGTDetector::CLSPGTDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		CLSPGTDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = false;
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	initializeFromControlSet(readingsControls, settingsControls);
}

CLSPGTDetector::CLSPGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hv, AMControl *integrationTime, AMControl *integrationMode, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		CLSPGTDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = true;
	AMControlSet *readingsControls = new AMControlSet(this);
	AMControlSet *settingsControls = new AMControlSet(this);
	readingsControls->addControl(dataWaveform);
	settingsControls->addControl(hv);
	settingsControls->addControl(integrationTime);
	settingsControls->addControl(integrationMode);
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	initializeFromControlSet(readingsControls, settingsControls);
}

CLSPGTDetector::~CLSPGTDetector()
{
	/* NTBA March 14, 2011 David Chevrier
	   Need to take care of ownsControlSet_
	   */
}

const QMetaObject* CLSPGTDetector::getMetaObject() {
	return metaObject();
}

double CLSPGTDetector::reading() const{
	if(isConnected())
		return dataWaveformCtrl()->value();
	else
		return -1;
}

AMDetectorInfo* CLSPGTDetector::toInfo() const{
	return new CLSPGTDetectorInfo(*this);
}

CLSPGTDetectorInfo CLSPGTDetector::toPGTInfo() const{
	return CLSPGTDetectorInfo(*this);
}

bool CLSPGTDetector::setFromInfo(const AMDetectorInfo *info){
	const CLSPGTDetectorInfo *di = qobject_cast<const CLSPGTDetectorInfo*>(info);
	if(!di)
		return false;
	//integrationTimeCtrl()->move(di->integrationTime());
	//integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(di->integrationMode()));
	//hvCtrl()->move(di->hvSetpoint());
	return true;
}

bool CLSPGTDetector::setFromInfo(const CLSPGTDetectorInfo& info){
	Q_UNUSED(info)
	//integrationTimeCtrl()->move(info.integrationTime());
	//integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(info.integrationMode()));
	//hvCtrl()->move(info.hvSetpoint());
	return true;
}

bool CLSPGTDetector::isPoweredOn(){
	return poweredOn_;
}

bool CLSPGTDetector::activate(){
	hvCtrl()->move(180);
	return true;
}

AMBeamlineActionItem* CLSPGTDetector::turnOnAction(){
	return toggleOnAction_->createCopy();
}

AMControl* CLSPGTDetector::dataWaveformCtrl() const {
	if(isConnected())
		return readingsControls_->at(0);
	else
		return 0;
}

AMControl* CLSPGTDetector::hvCtrl() const {
	if(isConnected())
		return settingsControls_->at(0);
	else
		return 0;
}

AMControl* CLSPGTDetector::integrationTimeCtrl() const {
	if(isConnected())
		return settingsControls_->at(1);
	else
		return 0;
}

AMControl* CLSPGTDetector::integrationModeCtrl() const {
	if(isConnected())
		return settingsControls_->at(2);
	else
		return 0;
}



bool CLSPGTDetector::settingsMatchFbk(CLSPGTDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->integrationTime() - integrationTimeCtrl()->value()) > integrationTimeCtrl()->tolerance() )
		return rVal;
	else if( fabs( settings->integrationModeList().indexOf(settings->integrationMode()) - integrationModeCtrl()->value()) > integrationModeCtrl()->tolerance() )
		return rVal;
	else if( fabs(settings->hvSetpoint() - hvCtrl()->value()) > hvCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

QString CLSPGTDetector::description() const{
	return AMDetectorInfo::description();
}

void CLSPGTDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

bool CLSPGTDetector::setControls(CLSPGTDetectorInfo *pgtSettings){
	Q_UNUSED(pgtSettings)
//	hvCtrl()->move( pgtSettings->hvSetpoint() );
//	integrationTimeCtrl()->move( pgtSettings->integrationTime() );
//	integrationModeCtrl()->move( integrationModeCtrl()->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

void CLSPGTDetector::onControlsConnected(bool connected){
	Q_UNUSED(connected)
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void CLSPGTDetector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvCtrl()->value());
		setIntegrationTime(integrationTimeCtrl()->value());
		setIntegrationMode(integrationModeCtrl()->enumNameAt(integrationModeCtrl()->value()));
		bool lastPoweredOn = poweredOn_;
		poweredOn_ = (hvCtrl()->value() >= 170);
		if(lastPoweredOn != poweredOn_)
			emit poweredOnChanged(poweredOn_);
		emitSettingsChanged();
	}
}

void CLSPGTDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

bool CLSPGTDetector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL
	poweredOn_ = false;

	if(readingsControls->count() == 1 && settingsControls->count() == 3){
		readingsControls_ = readingsControls;
		settingsControls_ = settingsControls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
		connect(readingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(readingsControls_, SIGNAL(controlSetValuesChanged()), this, SLOT(onReadingsControlValuesChanged()));
		connect(settingsControls_, SIGNAL(controlSetValuesChanged()), this, SLOT(onSettingsControlValuesChanged()));
		onControlsConnected(readingsControls_->isConnected() && settingsControls_->isConnected());
		if(isConnected()){
			onReadingsControlValuesChanged();
			onSettingsControlValuesChanged();
		}

		return true;
	}

	return false;
}
