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


#include "SGMMCPDetector.h"

SGMMCPDetector::SGMMCPDetector(const QString &name, const QString &fullReadingName, const QString &hvBaseName, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod, QObject *parent) :
	SGMMCPDetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	allControls_ = new AMControlSet();
	poweredOn_ = false;

	readingControl_ = new AMReadOnlyPVControl(name+"Reading", fullReadingName, this);
	readingControl_->setDescription("SGM MCP Reading");
	readingControl_->setContextKnownDescription("Reading");
	hvControl_ = new AMPVControl(name+"HV", hvBaseName+":vmon", hvBaseName+":v0set", QString(), this, 5.0);
	hvControl_->setDescription("TFY High Voltage");
	hvControl_->setContextKnownDescription("Voltage");

	allControls_->addControl(readingControl_);
	allControls_->addControl(hvControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
	connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
	connect(readingControl_, SIGNAL(valueChanged(double)), this, SLOT(onReadingsControlValuesChanged()));
	connect(hvControl_, SIGNAL(valueChanged(double)), this, SLOT(onSettingsControlValuesChanged()));
	connect(hvControl_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorHVChanged()));

	if(isConnected()){
		onReadingsControlValuesChanged();
		onSettingsControlValuesChanged();
	}
}

SGMMCPDetector::~SGMMCPDetector(){
}

const QMetaObject* SGMMCPDetector::getMetaObject() {
	return metaObject();
}

QString SGMMCPDetector::dacqName() const{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(readingCtrl());
	if(isConnected() && tmpControl)
		return tmpControl->readPVName();
	else
		return "";
}

double SGMMCPDetector::reading() const{
	if(isConnected())
		return readingControl_->value();
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
		return readingControl_;
	return 0;
}

AMControl* SGMMCPDetector::hvCtrl() const {
	if(isConnected())
		return hvControl_;
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

void SGMMCPDetector::onDetectorHVToggleChanged(){
	emit detectorHVChanged();
}

void SGMMCPDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void SGMMCPDetector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void SGMMCPDetector::onControlsTimedOut(){
	setTimedOut();
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
