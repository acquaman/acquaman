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


#include "CLSAmptekSDD123Detector.h"

#include "actions/AMBeamlineControlMoveAction.h"

CLSAmptekSDD123Detector::CLSAmptekSDD123Detector(const QString &name, const QString &baseName, AMDetector::ReadMethod readMethod, QObject *parent) :
	CLSAmptekSDD123DetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	baseName_ = baseName;
	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	setElements(1);
	wasConnected_ = false;

	allControls_ = new AMControlSet(this);
	startAcquisitionControl_ = new AMPVControl(name+"StartAcquisition", baseName+":spectrum:startAcquisition", baseName+":spectrum:startAcquisition", QString(), this, 0.5);
	startAcquisitionControl_->setAllowsMovesWhileMoving(true);
	statusControl_ = new AMReadOnlyPVControl(name+"Status", baseName+":spectrum:state", this);
	mcaChannelsControl_ = new AMReadOnlyPVControl(name+"MCAChannels", baseName+":parameters:MCAChannels", this);
	integrationTimeControl_ = new AMReadOnlyPVControl(name+"IntegrationTime", baseName+":parameters:PresetTime", this);
	detectorTemperatureControl_ = new AMReadOnlyPVControl(name+"DetectorTemperature", baseName+":parameters:DetectorTemperature", this);
	spectrumControl_ = new AMReadOnlyPVControl(name+"Spectrum", baseName+":spectrum", this);
	binnedSpectrumControl_ = new AMReadOnlyWaveformBinningPVControl(name+"BinnedSpectrum", baseName+":spectrum", 0, 1024, this);
	isRequestedControl_ = new AMPVControl(name+"IsRequested", baseName+":isRequested", baseName+":isRequested", QString(), this, 0.5);

	allControls_->addControl(startAcquisitionControl_);
	allControls_->addControl(statusControl_);
	allControls_->addControl(mcaChannelsControl_);
	allControls_->addControl(integrationTimeControl_);
	allControls_->addControl(detectorTemperatureControl_);
	allControls_->addControl(spectrumControl_);
	allControls_->addControl(binnedSpectrumControl_);
	allControls_->addControl(isRequestedControl_);
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	spectrumDataSource_ = new AM1DProcessVariableDataSource(tmpControl->readPV(), "Spectrum", this);

	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusChanged(double)));
	connect(mcaChannelsControl_, SIGNAL(valueChanged(double)), this, SLOT(setChannelsAsDouble(double)));
	connect(mcaChannelsControl_, SIGNAL(valueChanged(double)), this, SIGNAL(mcaChannelsChanged(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(setIntegrationTime(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(integrationTimeChanged(double)));
	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SLOT(setDetectorTemperature(double)));
	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorTemperatureChanged(double)));
	connect(binnedSpectrumControl_, SIGNAL(valueChanged(double)), this, SIGNAL(totalCountsChanged(double)));
}

CLSAmptekSDD123Detector::~CLSAmptekSDD123Detector()
{

}

const QMetaObject* CLSAmptekSDD123Detector::getMetaObject(){
	return metaObject();
}

QString CLSAmptekSDD123Detector::dacqName() const{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	if(isConnected() && tmpControl)
		return tmpControl->readPVName();
	else
		return "";
}

QStringList CLSAmptekSDD123Detector::dacqMove() const{
	QStringList retVal;
	retVal << QString("%1||=||%2%3||=||%4").arg("WaitPV").arg(baseName_).arg(":spectrum:state").arg("Done");
	return retVal;
}

QStringList CLSAmptekSDD123Detector::dacqDwell() const{
	QStringList retVal;
	retVal << QString("%1||=||%2%3||=||%4").arg("SetPV").arg(baseName_).arg(":spectrum:startAcquisition").arg("1");
	return retVal;
}

QString CLSAmptekSDD123Detector::description() const{
	return CLSAmptekSDD123DetectorInfo::description();
}

AMDetectorInfo* CLSAmptekSDD123Detector::toInfo() const{
	return new CLSAmptekSDD123DetectorInfo(*this);
}

CLSAmptekSDD123DetectorInfo CLSAmptekSDD123Detector::toCLSAmptekSDD123Info() const{
	return CLSAmptekSDD123DetectorInfo(*this);
}

bool CLSAmptekSDD123Detector::setFromInfo(const AMDetectorInfo *info)
{
	const CLSAmptekSDD123DetectorInfo *detectorInfo = qobject_cast<const CLSAmptekSDD123DetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	return true;
}

void CLSAmptekSDD123Detector::fromCLSAmptekSDD123Info(const CLSAmptekSDD123DetectorInfo &info)
{
	Q_UNUSED(info)
}

bool CLSAmptekSDD123Detector::wasConnected() const{
	return wasConnected_;
}

bool CLSAmptekSDD123Detector::isEnabled() const{
	if(!allControls_->isConnected() || isRequestedControl_->withinTolerance(0))
		return false;

	return true;
}

bool CLSAmptekSDD123Detector::status() const{
	if(statusControl_->withinTolerance(1))
		return true;
	return false;
}

QVector<int> CLSAmptekSDD123Detector::spectraValues()
{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	return tmpControl->readPV()->lastIntegerValues();
}

int CLSAmptekSDD123Detector::spectraTotalCounts()
{
	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(binnedSpectrumControl_);
	return tmpControl->value();
}

AMDataSource* CLSAmptekSDD123Detector::spectrumDataSource() const{
	return spectrumDataSource_;
}

QDebug CLSAmptekSDD123Detector::qDebugPrint(QDebug &d) const{
	d << "CLSAmptekSDD123Detector " << name();
	return d;
}

AMBeamlineActionItem* CLSAmptekSDD123Detector::createEnableAction(bool setEnabled) {

	if(!allControls_->isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(isRequestedControl_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(setEnabled == true ? 1 : 0);

	return action;
}

void CLSAmptekSDD123Detector::start(){
	startAcquisitionControl_->move(1);
}

void CLSAmptekSDD123Detector::setEnabled(bool isEnabled){
	if(!allControls_->isConnected())
		return;

	if(isEnabled)
		isRequestedControl_->move(1);
	else
		isRequestedControl_->move(0);
}

void CLSAmptekSDD123Detector::setDescription(const QString &description){
	CLSAmptekSDD123DetectorInfo::setDescription(description);
}

void CLSAmptekSDD123Detector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void CLSAmptekSDD123Detector::onControlsTimedOut(){
	setTimedOut();
}

void CLSAmptekSDD123Detector::onStatusChanged(double status){
	Q_UNUSED(status)
	if(statusControl_->withinTolerance(1))
		emit statusChanged(true);
	else
		emit statusChanged(false);
}
