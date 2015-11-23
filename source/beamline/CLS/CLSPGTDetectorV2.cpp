/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "CLSPGTDetectorV2.h"

#include <QStringBuilder>

#include "ui/CLS/CLSPGTDetectorV2View.h"

 CLSPGTDetectorV2::~CLSPGTDetectorV2(){}
CLSPGTDetectorV2::CLSPGTDetectorV2(const QString &name, const QString &description, const QString &baseName, QObject *parent) :
	AMDetector(name, description, parent)
{
	baseName_ = baseName;
	units_ = "Counts";

	allControls_ = new AMControlSet(this);
	statusControl_ = new AMReadOnlyPVControl(name%"Status", baseName%":GetAcquire", this);
	integrationTimeControl_ = new AMPVControl(name%"IntegrationTime", "BL1611-ID-1:AddOns:PGTDwellTime", "BL1611-ID-1:AddOns:PGTDwellTime", "", this, 0.001);
	integrationTimeControl_->setDescription("SDD Integration Time");
	integrationTimeControl_->setContextKnownDescription("Integration Time");
	integrationModeControl_ = new AMPVControl(name%"IntegrationMode", "BL1611-ID-1:AddOns:PGTDwellMode", "BL1611-ID-1:AddOns:PGTDwellMode", "", this, 0.1);
	integrationModeControl_->setDescription("SDD Integration Mode");
	integrationModeControl_->setContextKnownDescription("Integration Mode");
	startAcquisitionControl_ = new AMPVControl(name%"StartAcquisition", "BL1611-ID-1:AddOns:PGTDwellTrigger", "BL1611-ID-1:AddOns:PGTDwellTrigger", "", this, 0.1);
	startAcquisitionControl_->setDescription("SDD Start Acquisition");
	startAcquisitionControl_->setContextKnownDescription("Start Acquisition");
	spectrumControl_ = new AMReadOnlyPVControl(name%"Spectrum", baseName%":GetChannels", this);
	spectrumControl_->setDescription("SDD Spectrum");
	spectrumControl_->setContextKnownDescription("Spectrum");
	binnedSpectrumControl_ = new AMReadOnlyWaveformBinningPVControl(name%"BinnedSpectrum", baseName%":GetChannels", 0, 1024, this);
	clearControl_ = new AMPVControl(name%"Clear", baseName%":ClearSpectrum.PROC", baseName%":ClearSpectrum.PROC", QString(), this, 0.1);
	powerControl_ = new AMPVControl(name%"PowerOn", baseName%":Power:fbk", baseName%":Power", QString(), this, 0.1);
	voltageControl_ = new AMPVControl(name%"HV", baseName+":Bias:VoltActual:fbk", baseName+":Bias:Volt", QString(), this, 0.5);
	voltageControl_->setDescription("SDD High Voltage");
	voltageControl_->setContextKnownDescription("Voltage");

	allControls_->addControl(statusControl_);
	allControls_->addControl(integrationTimeControl_);
	allControls_->addControl(integrationModeControl_);
	allControls_->addControl(startAcquisitionControl_);
	allControls_->addControl(spectrumControl_);
	allControls_->addControl(binnedSpectrumControl_);
	allControls_->addControl(clearControl_);
	allControls_->addControl(powerControl_);
	allControls_->addControl(voltageControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));

	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionTimeChanged(double)));
	connect(integrationModeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionModeChanged()));
	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged(double)));

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	spectrumDataSource_ = new AM1DProcessVariableDataSource(tmpControl->readPV(), "Spectrum", this);
}

int CLSPGTDetectorV2::size(int axisNumber) const{
	if(axisNumber == 0)
		return 1024;
	return -1;
}

QList<AMAxisInfo> CLSPGTDetectorV2::axes() const{
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Energy", 1024, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;
	return axisInfo;
}

double CLSPGTDetectorV2::acquisitionTime() const{
	if(isConnected())
		return integrationTimeControl_->value();
	return -1;
}

double CLSPGTDetectorV2::acquisitionTimeTolerance() const
{
	if (isConnected())
		return integrationTimeControl_->tolerance();

	return -1;
}

QString CLSPGTDetectorV2::synchronizedDwellKey() const{
	return QString("%1:StartAcquisition.PROC NPP NM").arg(baseName_);
}

bool CLSPGTDetectorV2::sharesDetectorTriggerSource() const{
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* CLSPGTDetectorV2::detectorTriggerSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();
	return 0;
}

AMDetectorDwellTimeSource* CLSPGTDetectorV2::detectorDwellTimeSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();
	return 0;
}

AMNumber CLSPGTDetectorV2::reading(const AMnDIndex &indexes) const{
	if( (!isConnected()) || (indexes.rank() != 1) || (indexes.i() > 1024) )
		return AMNumber(AMNumber::DimensionError);

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	return tmpControl->readPV()->lastIntegerValues().at(indexes.i());
}

AMNumber CLSPGTDetectorV2::singleReading() const{
	if(!isConnected())
		return AMNumber(AMNumber::Null);

	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(binnedSpectrumControl_);
	return tmpControl->value();

}

bool CLSPGTDetectorV2::data(double *outputValues) const
{
	return spectrumDataSource_->values(AMnDIndex(0), AMnDIndex(spectrumDataSource_->size(0)-1), outputValues);
}

AMControl* CLSPGTDetectorV2::privilegedIntegrationModeControl(const QObject *caller){
	const CLSPGTDetectorV2View *view = qobject_cast<const CLSPGTDetectorV2View*>(caller);
	if(view)
		return integrationModeControl_;
	return 0; //NULL
}

bool CLSPGTDetectorV2::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	if(!integrationTimeControl_->withinTolerance(seconds))
		integrationTimeControl_->move(seconds);
	return true;
}

bool CLSPGTDetectorV2::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

bool CLSPGTDetectorV2::clear(){
	if(!isConnected())
		return false;
	clearControl_->move(1);
	return true;
}

bool CLSPGTDetectorV2::initializeImplementation(){
	setInitialized();
	return true;
}

bool CLSPGTDetectorV2::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	AMControl::FailureExplanation failureExplanation = startAcquisitionControl_->move(1);
	if(failureExplanation != AMControl::NoFailure)
		return true;
	return false;
}

bool CLSPGTDetectorV2::cleanupImplementation(){
	setCleanedUp();
	return true;
}

void CLSPGTDetectorV2::onControlsConnected(bool connected){
	if(connected)
		setConnected(connected);
	else
		setConnected(false);

	if(connected)
		setReadyForAcquisition();
	else if(!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void CLSPGTDetectorV2::onControlsTimedOut(){
	setConnected(false);
}

void CLSPGTDetectorV2::onStatusControlChanged(double value){
	Q_UNUSED(value)

	if(statusControl_->withinTolerance(1))
		setAcquiring();
	else if(statusControl_->withinTolerance(0) || statusControl_->withinTolerance(2)){
		if(isAcquiring())
			QTimer::singleShot(100, this, SLOT(setAcquisitionSucceeded())	);

		if(!isConnected() && !isNotReadyForAcquisition())
			setNotReadyForAcquisition();
		else if(isConnected() && !isReadyForAcquisition())
			setReadyForAcquisition();
	}
}
