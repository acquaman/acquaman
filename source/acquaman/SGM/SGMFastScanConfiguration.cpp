#include "SGMFastScanConfiguration.h"

#include <QFile>
#include <QDir>

#include "util/SGM/SGMPeriodicTable.h"

SGMFastScanConfiguration::SGMFastScanConfiguration(QObject *parent) : AMFastScanConfiguration(parent), SGMScanConfiguration()
{
	currentSettings_ = 0; //NULL
	currentEnergyParameters_ = 0; //NULL

	settings_ = SGMPeriodicTable::sgmTable()->fastScanPresets();


	setParametersFromPreset(0);

	currentEnergyParameters_ = new SGMEnergyParameters(SGMBeamline::sgm()->energyParametersForGrating(SGMBeamline::sgm()->currentGrating()));

	fastDetectors_ = SGMBeamline::sgm()->FastDetectors();

	allDetectors_ = new AMDetectorSet(this);
	allDetectors_->addDetector(SGMBeamline::sgm()->i0Detector(), true);
	allDetectors_->addDetector(SGMBeamline::sgm()->photodiodeDetector(), true);
	for(int x = 0; x < fastDetectors_->count(); x++)
		allDetectors_->addDetector(fastDetectors_->detectorAt(x), fastDetectors_->detectorAt(x));
	fastDetectorsConfigurations_ = fastDetectors_->toInfoSet();
}

SGMFastScanConfiguration::SGMFastScanConfiguration(const SGMFastScanConfiguration &original){
	currentSettings_ = 0; //NULL
	currentEnergyParameters_ = 0; //NULL

	settings_ = SGMPeriodicTable::sgmTable()->fastScanPresets();

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

	fastDetectors_ = SGMBeamline::sgm()->FastDetectors();
	allDetectors_ = new AMDetectorSet(this);
	allDetectors_->addDetector(SGMBeamline::sgm()->i0Detector(), true);
	allDetectors_->addDetector(SGMBeamline::sgm()->photodiodeDetector(), true);
	for(int x = 0; x < fastDetectors_->count(); x++)
		allDetectors_->addDetector(fastDetectors_->detectorAt(x), fastDetectors_->detectorAt(x));

	setDetectorConfigurations(original.detectorChoiceConfigurations());

}

SGMFastScanConfiguration::~SGMFastScanConfiguration(){
}

AMDetectorInfoSet SGMFastScanConfiguration::allDetectorConfigurations() const{
	AMDetectorInfoSet allConfigurations;
	for(int x = 0; x < SGMBeamline::sgm()->feedbackDetectors()->count(); x++)
		allConfigurations.addDetectorInfo(SGMBeamline::sgm()->feedbackDetectors()->detectorAt(x)->toInfo(), true);
	for(int x = 0; x < fastDetectorsConfigurations_.count(); x++)
		allConfigurations.addDetectorInfo(fastDetectorsConfigurations_.detectorInfoAt(x), fastDetectorsConfigurations_.isActiveAt(x));
	return allConfigurations;
}

AMScanConfiguration* SGMFastScanConfiguration::createCopy() const{
	return new SGMFastScanConfiguration(*this);
}

#include "SGMFastDacqScanController.h"

AMScanController* SGMFastScanConfiguration::createController(){
	return new SGMFastDacqScanController(this);
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

QStringList SGMFastScanConfiguration::presets() const{
	return SGMPeriodicTable::sgmTable()->fastScanPresetsStrings();
}

SGMFastScanParameters* SGMFastScanConfiguration::currentParameters() const{
	return currentSettings_;
}

SGMEnergyParameters* SGMFastScanConfiguration::currentEnergyParameters() const{
	return currentEnergyParameters_;
}

bool SGMFastScanConfiguration::setParametersFromPreset(int index){
	if(index < 0 && index >= settings_.count())
		return false;
	return setParameters(settings_.at(index));
}

bool SGMFastScanConfiguration::setParameters(SGMFastScanParameters *settings){
	if(!settings)
		return false;
	currentSettings_ = settings;
	setStartEnergy(currentSettings_->energyStart());
	setEndEnergy(currentSettings_->energyEnd());
	emit onElementChanged(currentSettings_->element());
	emit onRunSecondsChanged(currentSettings_->runSeconds());
	emit onEnergyStartChanged(currentSettings_->energyStart());
	emit onEnergyMidpointChanged(currentSettings_->energyMidpoint());
	emit onEnergyEndChanged(currentSettings_->energyEnd());
	emit onVelocityChanged(currentSettings_->velocity());
	emit onVelocityBaseChanged(currentSettings_->velocityBase());
	emit onAccelerationChanged(currentSettings_->acceleration());
	emit onScalerTimeChanged(currentSettings_->scalerTime());
	emit onBaseLineChanged(currentSettings_->baseLine());

	emit undulatorStartStepChanged(currentSettings_->undulatorStartStep());
	emit undulatorVelocityChanged(currentSettings_->undulatorVelocity());
	emit undulatorRelativeStepChanged(currentSettings_->undulatorRelativeStep());
	emit exitSlitDistanceChanged(currentSettings_->exitSlitDistance());

	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setElement(const QString& element){
	currentSettings_->setElement(element);
	emit onElementChanged(currentSettings_->element());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setRunSeconds(double runSeconds){
	currentSettings_->setRunSeconds(runSeconds);
	emit onRunSecondsChanged(currentSettings_->runSeconds());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyStart(double energyStart){
	currentSettings_->setEnergyStart(energyStart);
	setStartEnergy(energyStart);
	emit onEnergyStartChanged(currentSettings_->energyStart());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyMidpoint(double energyMidpoint){
	currentSettings_->setEnergyMidpoint(energyMidpoint);
	emit onEnergyMidpointChanged(currentSettings_->energyMidpoint());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyEnd(double energyEnd){
	currentSettings_->setEnergyEnd(energyEnd);
	setEndEnergy(energyEnd);
	emit onEnergyEndChanged(currentSettings_->energyEnd());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setVelocity(int velocity){
	currentSettings_->setVelocity(velocity);
	emit onVelocityChanged(currentSettings_->velocity());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setVelocityBase(int velocityBase){
	currentSettings_->setVelocityBase(velocityBase);
	emit onVelocityBaseChanged(currentSettings_->velocityBase());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setAcceleration(int acceleration){
	currentSettings_->setAcceleration(acceleration);
	emit onAccelerationChanged(currentSettings_->acceleration());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setScalerTime(double scalerTime){
	currentSettings_->setScalerTime(scalerTime);
	emit onScalerTimeChanged(currentSettings_->scalerTime());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setEnergyParameters(SGMEnergyParameters *parameters){
	if(!parameters)
		return false;
	if( !currentEnergyParameters_ || (*currentEnergyParameters_ != *parameters)){
		currentEnergyParameters_ = parameters;
		emit onSpacingParameterChanged(currentEnergyParameters_->spacingParameter());
		emit onC1ParameterChanged(currentEnergyParameters_->c1Parameter());
		emit onC2ParameterChanged(currentEnergyParameters_->c2Parameter());
		emit onSParameterChanged(currentEnergyParameters_->sParameter());
		emit onThetaParameterChanged(currentEnergyParameters_->thetaParameter());
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setSpacingParameter(double spacingParameter){
	if(currentEnergyParameters_->spacingParameter() != spacingParameter){
		currentEnergyParameters_->setSpacingParameter(spacingParameter);
		emit onSpacingParameterChanged(spacingParameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setC1Parameter(double c1Parameter){
	if(currentEnergyParameters_->c1Parameter() != c1Parameter){
		currentEnergyParameters_->setC1Parameter(c1Parameter);
		emit onC1ParameterChanged(c1Parameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setC2Parameter(double c2Parameter){
	if(currentEnergyParameters_->c2Parameter() != c2Parameter){
		currentEnergyParameters_->setC2Parameter(c2Parameter);
		emit onC2ParameterChanged(c2Parameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setSParameter(double sParameter){
	if(currentEnergyParameters_->sParameter() != sParameter){
		currentEnergyParameters_->setSParameter(sParameter);
		emit onSParameterChanged(sParameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setThetaParameter(double thetaParameter){
	if(currentEnergyParameters_->thetaParameter() != thetaParameter){
		currentEnergyParameters_->setThetaParameter(thetaParameter);
		emit onThetaParameterChanged(thetaParameter);
		setModified(true);
	}
	return true;
}

bool SGMFastScanConfiguration::setBaseLine(int baseLine){
	currentSettings_->setBaseLine(baseLine);
	emit onBaseLineChanged(currentSettings_->baseLine());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setUndulatorStartStep(int undulatorStartStep){
	currentSettings_->setUndulatorStartStep(undulatorStartStep);
	emit undulatorStartStepChanged(currentSettings_->undulatorStartStep());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setUndulatorVelocity(int undulatorVelocity){
	currentSettings_->setUndulatorVelocity(undulatorVelocity);
	emit undulatorVelocityChanged(currentSettings_->undulatorVelocity());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setUndulatorRelativeStep(int undulatorRelativeStep){
	currentSettings_->setUndulatorRelativeStep(undulatorRelativeStep);
	emit undulatorRelativeStepChanged(currentSettings_->undulatorRelativeStep());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setExitSlitDistance(double exitSlitDistance){
	currentSettings_->setExitSlitDistance(exitSlitDistance);
	emit exitSlitDistanceChanged(currentSettings_->exitSlitDistance());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setDetectorConfigurations(AMDetectorInfoSet detectorConfigurations) {
	fastDetectorsConfigurations_ = detectorConfigurations;
	setModified(true);
	return true;
}
