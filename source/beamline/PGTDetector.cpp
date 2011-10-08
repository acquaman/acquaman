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


#include "PGTDetector.h"

/*
CLSPGTDwellTimeCoordinator::CLSPGTDwellTimeCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;
	internalRealTimeSet_ = false;
	internalLiveTimeSet_ = false;
	internalPeakTimeSet_ = false;
	internalDwellTimeSet_ = false;
	internalDwellModeSet_ = false;
	dwellMode_ = 3;
	dwellTime_ = 0;

	realTimeControl_ = new AMPVControl("PGTRealTime", "MCA1611-01:Preset:Real", "MCA1611-01:Preset:Real", "", this, 0.1);
	liveTimeControl_ = new AMPVControl("PGTLiveTime", "MCA1611-01:Preset:Live", "MCA1611-01:Preset:Live", "", this, 0.1);
	peakTimeControl_ = new AMPVControl("PGTPeakTime", "MCA1611-01:Preset:Peak", "MCA1611-01:Preset:Peak", "", this, 0.1);
	dwellTimeControl_ = new AMPVControl("PGTDwellTime", "BL1611-ID-1:addOns:PGTDwellTime", "BL1611-ID-1:addOns:PGTDwellTime", "", this, 0.1);
	dwellModeControl_ = new AMPVControl("PGTDwellMode", "BL1611-ID-1:addOns:PGTDwellMode", "BL1611-ID-1:addOns:PGTDwellMode", "", this, 0.1);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(realTimeControl_);
	allControls_->addControl(liveTimeControl_);
	allControls_->addControl(peakTimeControl_);
	allControls_->addControl(dwellTimeControl_);
	allControls_->addControl(dwellModeControl_);

	connect(realTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onRealTimeControlChanged(double)));
	connect(liveTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onLiveTimeControlChanged(double)));
	connect(peakTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onPeakTimeControlChanged(double)));
	connect(dwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeControlChanged(double)));
	connect(dwellModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeModeChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
}

void CLSPGTDwellTimeCoordinator::onRealTimeControlChanged(double realTime){
	Q_UNUSED(realTime);
	if(!connectedOnce_ || internalRealTimeSet_){
		internalRealTimeSet_ = false;
		return;
	}
	dwellMode_ = determineMode();
	dwellTime_ = determineTime();
	//qDebug() << "Real time says mode " << dwellMode_ << " time " << dwellTime_;
	internalSetDwellMode(dwellMode_);
	internalSetDwellTime(dwellTime_);
}

void CLSPGTDwellTimeCoordinator::onLiveTimeControlChanged(double liveTime){
	Q_UNUSED(liveTime);
	if(!connectedOnce_ || internalLiveTimeSet_){
		internalLiveTimeSet_ = false;
		return;
	}
	dwellMode_ = determineMode();
	dwellTime_ = determineTime();
	//qDebug() << "Live time says mode " << dwellMode_ << " time " << dwellTime_;
	internalSetDwellMode(dwellMode_);
	internalSetDwellTime(dwellTime_);
}

void CLSPGTDwellTimeCoordinator::onPeakTimeControlChanged(double peakTime){
	Q_UNUSED(peakTime);
	if(!connectedOnce_ || internalPeakTimeSet_){
		internalPeakTimeSet_ = false;
		return;
	}
	dwellMode_ = determineMode();
	dwellTime_ = determineTime();
	//qDebug() << "Peak time says mode " << dwellMode_ << " time " << dwellTime_;
	internalSetDwellMode(dwellMode_);
	internalSetDwellTime(dwellTime_);
}

void CLSPGTDwellTimeCoordinator::onDwellTimeControlChanged(double dwellTime){
	if(!connectedOnce_ || internalDwellTimeSet_){
		internalDwellTimeSet_ = false;
		return;
	}
	//qDebug() << "Dwell time changing from " << dwellTime_ << " to " << dwellTime;
	dwellTime_ = dwellTime;
	switch(dwellMode_){
	case 0:
		internalSetRealTime(dwellTime_);
		break;
	case 1:
		internalSetLiveTime(dwellTime_);
		break;
	case 2:
		internalSetPeakTime(dwellTime_);
		break;
	case 3:
		break;
	}
}

void CLSPGTDwellTimeCoordinator::onDwellTimeModeChanged(double dwellMode){
	if(!connectedOnce_ || internalDwellModeSet_){
		internalDwellModeSet_ = false;
		return;
	}
	//qDebug() << "Switch dwell mode from " << dwellMode_ << " to " << dwellMode << " with time " << dwellTime_;
	switch(dwellMode_){
	case 0:
		internalSetRealTime(0.0);
		break;
	case 1:
		internalSetLiveTime(0.0);
		break;
	case 2:
		internalSetPeakTime(0.0);
		break;
	case 3:
		if(dwellMode != 0)
			internalSetRealTime(0.0);
		if(dwellMode != 1)
			internalSetLiveTime(0.0);
		if(dwellMode != 2)
			internalSetPeakTime(0.0);
		break;
	}
	dwellMode_ = dwellMode;
	switch(dwellMode_){
	case 0:
		internalSetRealTime(dwellTime_);
		break;
	case 1:
		internalSetLiveTime(dwellTime_);
		break;
	case 2:
		internalSetPeakTime(dwellTime_);
		break;
	case 3:
		break;
	}
}

void CLSPGTDwellTimeCoordinator::onAllControlsConnected(bool connected){
	//qDebug() << "Checking all PGT dwells";
	if(connected){
		connectedOnce_ = true;
		//qDebug() << "All PGT dwells connected";
		dwellMode_ = determineMode();
		dwellTime_ = determineTime();
		internalSetDwellMode(dwellMode_);
		internalSetDwellTime(dwellTime_);
	}
}

void CLSPGTDwellTimeCoordinator::internalSetRealTime(double realTime){
	if(realTimeControl_->withinTolerance(realTime))
		return;
	internalRealTimeSet_ = true;
	realTimeControl_->move(realTime);
}

void CLSPGTDwellTimeCoordinator::internalSetLiveTime(double liveTime){
	if(liveTimeControl_->withinTolerance(liveTime))
		return;
	internalLiveTimeSet_ = true;
	liveTimeControl_->move(liveTime);
}

void CLSPGTDwellTimeCoordinator::internalSetPeakTime(double peakTime){
	if(peakTimeControl_->withinTolerance(peakTime))
		return;
	internalPeakTimeSet_ = true;
	peakTimeControl_->move(peakTime);
}

void CLSPGTDwellTimeCoordinator::internalSetDwellTime(double dwellTime){
	if(dwellTimeControl_->withinTolerance(dwellTime))
		return;
	internalDwellTimeSet_ = true;
	dwellTimeControl_->move(dwellTime);
}

void CLSPGTDwellTimeCoordinator::internalSetDwellMode(int dwellMode){
	if(dwellModeControl_->withinTolerance(dwellMode))
		return;
	internalDwellModeSet_ = true;
	dwellModeControl_->move(dwellMode);
}

int CLSPGTDwellTimeCoordinator::determineMode() const{
	if(allControls_->isConnected()){
		double realTime = realTimeControl_->value();
		double liveTime = liveTimeControl_->value();
		double peakTime = peakTimeControl_->value();

		if( (realTime > 0.1 && liveTime > 0.1) || (realTime > 0.1 && peakTime > 0.1) || (peakTime > 0.1 && liveTime > 0.1) )
			return 3;
		else if(realTime > 0.1)
			return 0;
		else if(liveTime > 0.1)
			return 1;
		else if(peakTime > 0.1)
			return 2;
		else
			return 3;
	}
	return 3;
}

double CLSPGTDwellTimeCoordinator::determineTime() const{
	if(allControls_->isConnected()){
		double realTime = realTimeControl_->value();
		double liveTime = liveTimeControl_->value();
		double peakTime = peakTimeControl_->value();

		if( (realTime > 0.1 && liveTime > 0.1) || (realTime > 0.1 && peakTime > 0.1) || (peakTime > 0.1 && liveTime > 0.1) )
			return 0.0;
		else if(realTime > 0.1)
			return realTime;
		else if(liveTime > 0.1)
			return liveTime;
		else if(peakTime > 0.1)
			return peakTime;
		else
			return 0.0;
	}
	return 0.0;
}
*/

PGTDetector::PGTDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	ownsControlSets_ = false;
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	initializeFromControlSet(readingsControls, settingsControls);
}

PGTDetector::PGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hv, AMControl *integrationTime, AMControl *integrationMode, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name, readMethod)
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

PGTDetector::~PGTDetector()
{
	/* NTBA March 14, 2011 David Chevrier
	   Need to take care of ownsControlSet_
	   */
}

const QMetaObject* PGTDetector::getMetaObject() {
	return metaObject();
}

double PGTDetector::reading() const{
	if(isConnected())
		return dataWaveformCtrl()->value();
	else
		return -1;
}

AMDetectorInfo* PGTDetector::toInfo() const{
	return new PGTDetectorInfo(*this);
}

PGTDetectorInfo PGTDetector::toPGTInfo() const{
	return PGTDetectorInfo(*this);
}

bool PGTDetector::setFromInfo(const AMDetectorInfo *info){
	const PGTDetectorInfo *di = qobject_cast<const PGTDetectorInfo*>(info);
	if(!di)
		return false;
	//integrationTimeCtrl()->move(di->integrationTime());
	//integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(di->integrationMode()));
	//hvCtrl()->move(di->hvSetpoint());
	return true;
}

bool PGTDetector::setFromInfo(const PGTDetectorInfo& info){
	Q_UNUSED(info)
	//integrationTimeCtrl()->move(info.integrationTime());
	//integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(info.integrationMode()));
	//hvCtrl()->move(info.hvSetpoint());
	return true;
}

bool PGTDetector::isPoweredOn(){
	return poweredOn_;
}

bool PGTDetector::activate(){
	hvCtrl()->move(180);
	return true;
}

AMBeamlineActionItem* PGTDetector::turnOnAction(){
	return toggleOnAction_->createCopy();
}

AMControl* PGTDetector::dataWaveformCtrl() const {
	if(isConnected())
		return readingsControls_->at(0);
	else
		return 0;
}

AMControl* PGTDetector::hvCtrl() const {
	if(isConnected())
		return settingsControls_->at(0);
	else
		return 0;
}

AMControl* PGTDetector::integrationTimeCtrl() const {
	if(isConnected())
		return settingsControls_->at(1);
	else
		return 0;
}

AMControl* PGTDetector::integrationModeCtrl() const {
	if(isConnected())
		return settingsControls_->at(2);
	else
		return 0;
}



bool PGTDetector::settingsMatchFbk(PGTDetectorInfo *settings){
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

QString PGTDetector::description() const{
	return AMDetectorInfo::description();
}

void PGTDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

bool PGTDetector::setControls(PGTDetectorInfo *pgtSettings){
	Q_UNUSED(pgtSettings)
//	hvCtrl()->move( pgtSettings->hvSetpoint() );
//	integrationTimeCtrl()->move( pgtSettings->integrationTime() );
//	integrationModeCtrl()->move( integrationModeCtrl()->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

void PGTDetector::onControlsConnected(bool connected){
	Q_UNUSED(connected)
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void PGTDetector::onSettingsControlValuesChanged(){
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

void PGTDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

bool PGTDetector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL
	poweredOn_ = false;

	if(readingsControls->count() == 1 && settingsControls->count() == 3){
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
		}

		return true;
	}

	return false;
}
