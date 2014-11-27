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


#include "SGMFastScanConfiguration.h"

#include <QFile>
#include <QDir>

#include "util/SGM/SGMPeriodicTable.h"

SGMFastScanConfiguration::SGMFastScanConfiguration(QObject *parent) : AMFastScanConfiguration(parent), SGMScanConfiguration()
{
	currentSettings_ = 0; //NULL
	currentEnergyParameters_ = 0; //NULL

	settings_ = SGMPeriodicTable::sgmTable()->fastScanPresets(SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName());


	setParametersFromPreset(0);

	currentEnergyParameters_ = new SGMEnergyParameters(SGMBeamlineInfo::sgmInfo()->energyParametersForGrating(SGMBeamline::sgm()->currentGrating()));
}

SGMFastScanConfiguration::SGMFastScanConfiguration(const SGMFastScanConfiguration &original) :
		AMFastScanConfiguration(original), SGMScanConfiguration()
{
	currentSettings_ = 0; //NULL
	currentEnergyParameters_ = 0; //NULL

	settings_ = SGMPeriodicTable::sgmTable()->fastScanPresets(SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName());

	bool foundPreset = false;
	for(int x = 0; x < settings_.count(); x++){
		if(!foundPreset && settings_.at(x) == original.currentParameters() ){
			setParametersFromPreset(x);
			foundPreset = true;
		}
	}
	if(!foundPreset)
		setParameters(original.currentParameters());

	setEnergyParameters(original.currentEnergyParameters());

	setDetectorConfigurations(original.detectorChoiceConfigurations());

	dbLoadWarnings_ = original.dbLoadWarnings();
}

SGMFastScanConfiguration::~SGMFastScanConfiguration(){
}

const QMetaObject* SGMFastScanConfiguration::getMetaObject(){
	return metaObject();
}

AMScanConfiguration* SGMFastScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new SGMFastScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* SGMFastScanConfiguration::createController(){
	return 0; //NULL
}

#include "ui/SGM/SGMFastScanConfigurationView.h"

AMScanConfigurationView* SGMFastScanConfiguration::createView(){
	return new SGMFastScanConfigurationView(this);
}

QString SGMFastScanConfiguration::detailedDescription() const{
	return QString("Fast Scan from %1 to %2\nIntegration Time: %3").arg(startEnergy())
			.arg(endEnergy())
			.arg(runTime());
}

QString SGMFastScanConfiguration::element() const{
	return currentSettings_->element();
}

double SGMFastScanConfiguration::runTime() const{
	return currentSettings_->runSeconds();
}

double SGMFastScanConfiguration::energyStart() const{
	return currentSettings_->energyStart();
}

double SGMFastScanConfiguration::energyMidpoint() const{
	return currentSettings_->energyMidpoint();
}

double SGMFastScanConfiguration::energyEnd() const{
	return currentSettings_->energyEnd();
}

int SGMFastScanConfiguration::velocity() const{
	return currentSettings_->velocity();
}

int SGMFastScanConfiguration::velocityBase() const{
	return currentSettings_->velocityBase();
}

int SGMFastScanConfiguration::acceleration() const{
	return currentSettings_->acceleration();
}

double SGMFastScanConfiguration::scalerTime() const{
	return currentSettings_->scalerTime();
}

double SGMFastScanConfiguration::spacingParameter() const{
	return currentEnergyParameters_->spacingParameter();
}

double SGMFastScanConfiguration::c1Parameter() const{
	return currentEnergyParameters_->c1Parameter();
}

double SGMFastScanConfiguration::c2Parameter() const{
	return currentEnergyParameters_->c2Parameter();
}

double SGMFastScanConfiguration::sParameter() const{
	return currentEnergyParameters_->sParameter();
}

double SGMFastScanConfiguration::thetaParameter() const{
	return currentEnergyParameters_->thetaParameter();
}

int SGMFastScanConfiguration::baseLine() const{
	return currentSettings_->baseLine();
}

int SGMFastScanConfiguration::undulatorStartStep() const{
	return currentSettings_->undulatorStartStep();
}

int SGMFastScanConfiguration::undulatorVelocity() const{
	return currentSettings_->undulatorVelocity();
}

int SGMFastScanConfiguration::undulatorRelativeStep() const{
	return currentSettings_->undulatorRelativeStep();
}

double SGMFastScanConfiguration::exitSlitDistance() const{
	return currentSettings_->exitSlitDistance();
}

int SGMFastScanConfiguration::sgmGrating() const{
	return currentSettings_->sgmGrating();
}

QStringList SGMFastScanConfiguration::presets() const{
	return SGMPeriodicTable::sgmTable()->fastScanPresetsStrings(SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName());
}

SGMFastScanParameters* SGMFastScanConfiguration::currentParameters() const{
	return currentSettings_;
}

SGMEnergyParameters* SGMFastScanConfiguration::currentEnergyParameters() const{
	return currentEnergyParameters_;
}

QString SGMFastScanConfiguration::dbLoadWarnings() const{
	return dbLoadWarnings_;
}

bool SGMFastScanConfiguration::setParametersFromPreset(int index){
	if(index < 0 && index >= settings_.count())
		return false;
	return setParameters(settings_.at(index));
}

bool SGMFastScanConfiguration::setParameters(SGMFastScanParameters *settings){
	if(!settings)
		return false;
	disconnect(currentSettings_, 0);
	currentSettings_ = settings;
	setStartEnergy(currentSettings_->energyStart());
	setEndEnergy(currentSettings_->energyEnd());
	emit elementChanged(currentSettings_->element());
	emit runSecondsChanged(currentSettings_->runSeconds());
	emit energyStartChanged(currentSettings_->energyStart());
	emit energyMidpointChanged(currentSettings_->energyMidpoint());
	emit energyEndChanged(currentSettings_->energyEnd());
	emit velocityChanged(currentSettings_->velocity());
	emit velocityBaseChanged(currentSettings_->velocityBase());
	emit accelerationChanged(currentSettings_->acceleration());
	emit scalerTimeChanged(currentSettings_->scalerTime());
	emit baseLineChanged(currentSettings_->baseLine());

	emit undulatorStartStepChanged(currentSettings_->undulatorStartStep());
	emit undulatorVelocityChanged(currentSettings_->undulatorVelocity());
	emit undulatorRelativeStepChanged(currentSettings_->undulatorRelativeStep());
	emit exitSlitDistanceChanged(currentSettings_->exitSlitDistance());
	emit sgmGratingChanged(currentSettings_->sgmGrating());

	emit startPositionChanged();
	emit middlePositionChanged();
	emit endPositionChanged();
	emit fastScanSettingsChanged();
	emit parametersChanged();

	connect(currentSettings_, SIGNAL(elementChanged(QString)), this, SIGNAL(elementChanged(QString)));
	connect(currentSettings_, SIGNAL(runSecondsChanged(double)), this, SIGNAL(runSecondsChanged(double)));
	connect(currentSettings_, SIGNAL(energyStartChanged(double)), this, SIGNAL(energyStartChanged(double)));
	connect(currentSettings_, SIGNAL(energyMidpointChanged(double)), this, SIGNAL(energyMidpointChanged(double)));
	connect(currentSettings_, SIGNAL(energyEndChanged(double)), this, SIGNAL(energyEndChanged(double)));
	connect(currentSettings_, SIGNAL(velocityChanged(int)), this, SIGNAL(velocityChanged(int)));
	connect(currentSettings_, SIGNAL(velocityBaseChanged(int)), this, SIGNAL(velocityBaseChanged(int)));
	connect(currentSettings_, SIGNAL(accelerationChanged(int)), this, SIGNAL(accelerationChanged(int)));
	connect(currentSettings_, SIGNAL(scalerTimeChanged(double)), this, SIGNAL(scalerTimeChanged(double)));
	connect(currentSettings_, SIGNAL(baseLineChanged(int)), this, SIGNAL(baseLineChanged(int)));
	connect(currentSettings_, SIGNAL(undulatorStartStepChanged(int)), this, SIGNAL(undulatorStartStepChanged(int)));
	connect(currentSettings_, SIGNAL(undulatorVelocityChanged(int)), this, SIGNAL(undulatorVelocityChanged(int)));
	connect(currentSettings_, SIGNAL(undulatorRelativeStepChanged(int)), this, SIGNAL(undulatorRelativeStepChanged(int)));
	connect(currentSettings_, SIGNAL(exitSlitDistanceChanged(double)), this, SIGNAL(exitSlitDistanceChanged(double)));
	connect(currentSettings_, SIGNAL(sgmGratingChanged(int)), this, SIGNAL(sgmGratingChanged(int)));
	connect(currentSettings_, SIGNAL(startPositionChanged()), this, SIGNAL(startPositionChanged()));
	connect(currentSettings_, SIGNAL(middlePositionChanged()), this, SIGNAL(middlePositionChanged()));
	connect(currentSettings_, SIGNAL(endPositionChanged()), this, SIGNAL(endPositionChanged()));
	connect(currentSettings_, SIGNAL(fastScanSettingsChanged()), this, SIGNAL(fastScanSettingsChanged()));

	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setElement(const QString& element){
	currentSettings_->setElement(element);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setRunSeconds(double runSeconds){
	currentSettings_->setRunSeconds(runSeconds);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyStart(double energyStart){
	currentSettings_->setEnergyStart(energyStart);
	setStartEnergy(energyStart);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyMidpoint(double energyMidpoint){
	currentSettings_->setEnergyMidpoint(energyMidpoint);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyEnd(double energyEnd){
	currentSettings_->setEnergyEnd(energyEnd);
	setEndEnergy(energyEnd);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setVelocity(int velocity){
	currentSettings_->setVelocity(velocity);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setVelocityBase(int velocityBase){
	currentSettings_->setVelocityBase(velocityBase);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setAcceleration(int acceleration){
	currentSettings_->setAcceleration(acceleration);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setScalerTime(double scalerTime){
	currentSettings_->setScalerTime(scalerTime);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyParameters(SGMEnergyParameters *parameters){
	if(!parameters)
		return false;
	if( !currentEnergyParameters_ || (*currentEnergyParameters_ != *parameters)){
		currentEnergyParameters_ = parameters;
		emit spacingParameterChanged(currentEnergyParameters_->spacingParameter());
		emit c1ParameterChanged(currentEnergyParameters_->c1Parameter());
		emit c2ParameterChanged(currentEnergyParameters_->c2Parameter());
		emit sParameterChanged(currentEnergyParameters_->sParameter());
		emit thetaParameterChanged(currentEnergyParameters_->thetaParameter());
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setSpacingParameter(double spacingParameter){
	if(currentEnergyParameters_->spacingParameter() != spacingParameter){
		currentEnergyParameters_->setSpacingParameter(spacingParameter);
		emit spacingParameterChanged(spacingParameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setC1Parameter(double c1Parameter){
	if(currentEnergyParameters_->c1Parameter() != c1Parameter){
		currentEnergyParameters_->setC1Parameter(c1Parameter);
		emit c1ParameterChanged(c1Parameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setC2Parameter(double c2Parameter){
	if(currentEnergyParameters_->c2Parameter() != c2Parameter){
		currentEnergyParameters_->setC2Parameter(c2Parameter);
		emit c2ParameterChanged(c2Parameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setSParameter(double sParameter){
	if(currentEnergyParameters_->sParameter() != sParameter){
		currentEnergyParameters_->setSParameter(sParameter);
		emit sParameterChanged(sParameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setThetaParameter(double thetaParameter){
	if(currentEnergyParameters_->thetaParameter() != thetaParameter){
		currentEnergyParameters_->setThetaParameter(thetaParameter);
		emit thetaParameterChanged(thetaParameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setBaseLine(int baseLine){
	currentSettings_->setBaseLine(baseLine);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setUndulatorStartStep(int undulatorStartStep){
	currentSettings_->setUndulatorStartStep(undulatorStartStep);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setUndulatorVelocity(int undulatorVelocity){
	currentSettings_->setUndulatorVelocity(undulatorVelocity);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setUndulatorRelativeStep(int undulatorRelativeStep){
	currentSettings_->setUndulatorRelativeStep(undulatorRelativeStep);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setExitSlitDistance(double exitSlitDistance){
	currentSettings_->setExitSlitDistance(exitSlitDistance);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setSGMGrating(int sgmGrating){
	currentSettings_->setSGMGrating(sgmGrating);
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setDetectorConfigurations(AMOldDetectorInfoSet detectorConfigurations) {
	fastDetectorsConfigurations_ = detectorConfigurations;
	setModified(true);
	return true;
}

AMDbObject* SGMFastScanConfiguration::dbReadFastScanParameters(){
	return currentSettings_;
}

void SGMFastScanConfiguration::dbLoadFastScanParameters(AMDbObject *fastScanParameters){
	SGMFastScanParameters *scanParameters = qobject_cast<SGMFastScanParameters*>(fastScanParameters);
	if(scanParameters)
		currentSettings_ = scanParameters;
	else
		dbLoadWarnings_ = "This scan was run before fast scans were saving necessary information. YOU SHOULD NOT PROCEED WITH THIS SCAN.";
}
