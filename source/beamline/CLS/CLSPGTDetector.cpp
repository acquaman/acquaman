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


#include "CLSPGTDetector.h"

CLSPGTDetector::CLSPGTDetector(const QString &name, const QString &baseName, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMOldDetector::ReadMethod readMethod, QObject *parent) :
	CLSPGTDetectorInfo(name, name, parent), AMOldDetector(name, readMethod)
{
	baseName_ = baseName;
	toggleOnAction_ = toggleOnAction;
	toggleOffAction_ = toggleOffAction;
	allControls_ = new AMControlSet();
	poweredOn_ = false;

	dataWaveformControl_ = new AMReadOnlyWaveformBinningPVControl(name+"Spectrum", baseName+":GetChannels", 0, 1024, this);
	dataWaveformControl_->setDescription("SDD Spectrum");
	dataWaveformControl_->setContextKnownDescription("Spectrum");
	hvControl_ = new AMPVControl(name+"HV", baseName+":Bias:VoltActual:fbk", baseName+":Bias:Volt", QString(), this, 0.5);
	hvControl_->setDescription("SDD High Voltage");
	hvControl_->setContextKnownDescription("Voltage");
	integrationTimeControl_ = new AMPVControl(name+"IntegrationTime", "BL1611-ID-1:AddOns:PGTDwellTime", "BL1611-ID-1:AddOns:PGTDwellTime", "", this, 0.1);
	integrationTimeControl_->setDescription("SDD Integration Time");
	integrationTimeControl_->setContextKnownDescription("Integration Time");
	integrationModeControl_ = new AMPVControl(name+"IntegrationMode", "BL1611-ID-1:AddOns:PGTDwellMode", "BL1611-ID-1:AddOns:PGTDwellMode", "", this, 0.1);
	integrationModeControl_->setDescription("SDD Integration Mode");
	integrationModeControl_->setContextKnownDescription("Integration Mode");
	dwellTriggerControl_ = new AMPVControl(name+"DwellTrigger", "BL1611-ID-1:AddOns:PGTDwellTrigger", "BL1611-ID-1:AddOns:PGTDwellTrigger", "", this, 0.1);
	dwellTriggerControl_->setDescription("SDD Dwell Trigger");
	dwellTriggerControl_->setContextKnownDescription("Dwell Trigger");

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(dataWaveformControl_);
	spectrumDataSource_ = new AM1DProcessVariableDataSource(tmpControl->readPV(), "Spectrum", this);

	allControls_->addControl(dataWaveformControl_);
	allControls_->addControl(hvControl_);
	allControls_->addControl(integrationTimeControl_);
	allControls_->addControl(integrationModeControl_);
	allControls_->addControl(dwellTriggerControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
	connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(dataWaveformControl_, SIGNAL(valueChanged(double)), this, SLOT(onReadingsControlValuesChanged()));
	connect(tmpControl, SIGNAL(valueChanged(double)), this, SIGNAL(totalCountsChanged(double)));
	connect(hvControl_, SIGNAL(valueChanged(double)), this, SLOT(onSettingsControlValuesChanged()));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onSettingsControlValuesChanged()));
	connect(integrationModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onSettingsControlValuesChanged()));
	connect(dwellTriggerControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTriggerChanged(double)));

	if(isConnected()){
		onReadingsControlValuesChanged();
		onSettingsControlValuesChanged();
	}
}

CLSPGTDetector::~CLSPGTDetector()
{
}

const QMetaObject* CLSPGTDetector::getMetaObject() {
	return metaObject();
}

QString CLSPGTDetector::dacqName() const{
	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(dataWaveformControl_);
	if(isConnected() && tmpControl)
		return tmpControl->readPVName();
	else
		return "";
}

QStringList CLSPGTDetector::dacqBegin() const{
	QStringList retVal;
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateWaveform").arg("0");
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateROI").arg("0");
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateStatus").arg("0");
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateElapsed").arg("0");
	return retVal;
}

QStringList CLSPGTDetector::dacqMove() const{
	QStringList retVal;
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":ClearSpectrum.PROC").arg("1");
	retVal << QString("%1||=||%2%3||=||%4").arg("WaitPV").arg(baseName_).arg(":ROI:0").arg("0");
	return retVal;
}

QStringList CLSPGTDetector::dacqDwell() const{
	QStringList retVal;
//	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":StartAcquisition.PROC").arg("1");
	return retVal;
}

QStringList CLSPGTDetector::dacqFinish() const{
	QStringList retVal;
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateWaveform").arg("1");
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateROI").arg("1");
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateStatus").arg("1");
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":AcqUpdateElapsed").arg("1");
	return retVal;
}

double CLSPGTDetector::reading() const{
	if(isConnected())
		return dataWaveformCtrl()->value();
	else
		return -1;
}

AMOldDetectorInfo* CLSPGTDetector::toInfo() const{
	return new CLSPGTDetectorInfo(*this);
}

CLSPGTDetectorInfo CLSPGTDetector::toPGTInfo() const{
	return CLSPGTDetectorInfo(*this);
}

bool CLSPGTDetector::setFromInfo(const AMOldDetectorInfo *info){
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

bool CLSPGTDetector::status() const{
	if(isConnected() && dwellTriggerControl_->withinTolerance(1))
		return true;
	return false;
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
		return dataWaveformControl_;
	else
		return 0;
}

AMControl* CLSPGTDetector::hvCtrl() const {
	if(isConnected())
		return hvControl_;
	else
		return 0;
}

AMControl* CLSPGTDetector::integrationTimeCtrl() const {
	if(isConnected())
		return integrationTimeControl_;
	else
		return 0;
}

AMControl* CLSPGTDetector::integrationModeCtrl() const {
	if(isConnected())
		return integrationModeControl_;
	else
		return 0;
}

AMControl* CLSPGTDetector::dwellTriggerControl() const{
	if(isConnected())
		return dwellTriggerControl_;
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
	return AMOldDetectorInfo::description();
}

QVector<int> CLSPGTDetector::spectraValues()
{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(dataWaveformControl_);
	return tmpControl->readPV()->lastIntegerValues();
}

int CLSPGTDetector::spectraTotalCounts()
{
	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(dataWaveformControl_);
	return tmpControl->value();
}

AMDataSource* CLSPGTDetector::spectrumDataSource() const{
	return spectrumDataSource_;
}

void CLSPGTDetector::setDescription(const QString &description){
	AMOldDetectorInfo::setDescription(description);
}

bool CLSPGTDetector::setControls(CLSPGTDetectorInfo *pgtSettings){
	Q_UNUSED(pgtSettings)
//	hvCtrl()->move( pgtSettings->hvSetpoint() );
//	integrationTimeCtrl()->move( pgtSettings->integrationTime() );
//	integrationModeCtrl()->move( integrationModeCtrl()->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

void CLSPGTDetector::start(){
	if(isConnected())
		dwellTriggerControl_->move(1);
}

void CLSPGTDetector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void CLSPGTDetector::onControlsTimedOut(){
	setTimedOut();
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

void CLSPGTDetector::onDwellTriggerChanged(double value){
	Q_UNUSED(value)
	if(!isConnected())
		return;

	if(dwellTriggerControl_->withinTolerance(1))
		emit statusChanged(true);
	else
		emit statusChanged(false);
}

void CLSPGTDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}
