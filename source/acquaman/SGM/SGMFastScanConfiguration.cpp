#include "SGMFastScanConfiguration.h"

#include <QFile>
#include <QDir>

SGMFastScanConfiguration::SGMFastScanConfiguration(QObject *parent) : AMFastScanConfiguration(parent), SGMScanConfiguration()
{
	currentSettings_ = 0; //NULL

	settings_.append( new SGMFastScanParameters("Nitrogen", 5.0, 400.0, 415.0, 430.0, 10000, 10000, 10000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Nitrogen", 20.0, 400.0, 415.0, 430.0, 1000, 1000, 1000, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 5.0, 530.0, 545.0, 560.0, 10000, 10000, 10000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 20.0, 530.0, 545.0, 560.0, 1000, 1000, 1000, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Copper", 5.0, 925.0, 935.0, 945.0, 3000, 3000, 3000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Copper", 20.0, 925.0, 935.0, 945.0, 450, 450, 450, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Carbon", 5.0, 280.0, 295.0, 320.0, 19000, 19000, 19000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Zinc 3/2", 5.0, 1010.0, 1025.0, 1040.0, 3200, 3200, 3200, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Zinc 1/2", 5.0, 1025.0, 1040.0, 1055.0, 3350, 3350, 3350, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Zinc L", 5.0, 1010.0, 1035.0, 1060.0, 6000, 6000, 6000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Zinc L", 20.0, 1010.0, 1025.0, 1040.0, 860, 860, 860, 20.0, 200, this));

	setParametersFromPreset(0);

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

	settings_.append( new SGMFastScanParameters("Nitrogen", 5.0, 400.0, 415.0, 430.0, 10000, 10000, 10000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Nitrogen", 20.0, 400.0, 415.0, 430.0, 1000, 1000, 1000, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 5.0, 530.0, 545.0, 560.0, 10000, 10000, 10000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 20.0, 530.0, 545.0, 560.0, 1000, 1000, 1000, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Copper", 5.0, 925.0, 935.0, 945.0, 3000, 3000, 3000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Copper", 20.0, 925.0, 935.0, 945.0, 450, 450, 450, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Carbon", 5.0, 280.0, 295.0, 320.0, 19000, 19000, 19000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("David", 5.0, 700.0, 715.0, 730.0, 10000, 10000, 10000, 5.0, 200, this));

	bool foundPreset = false;
	for(int x = 0; x < settings_.count(); x++){
		if(!foundPreset && settings_.at(x) == original.currentParameters() ){
			setParametersFromPreset(x);
			foundPreset = true;
		}
	}
	if(!foundPreset)
		setParameters(original.currentParameters());

	fastDetectors_ = SGMBeamline::sgm()->FastDetectors();
	allDetectors_ = new AMDetectorSet(this);
	allDetectors_->addDetector(SGMBeamline::sgm()->i0Detector(), true);
	allDetectors_->addDetector(SGMBeamline::sgm()->photodiodeDetector(), true);
	for(int x = 0; x < fastDetectors_->count(); x++)
		allDetectors_->addDetector(fastDetectors_->detectorAt(x), fastDetectors_->detectorAt(x));

	setDetectorConfigurations(original.detectorChoiceConfigurations());

}

SGMFastScanConfiguration::~SGMFastScanConfiguration(){
	while(settings_.count() > 0)
		delete settings_.takeLast();
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

int SGMFastScanConfiguration::baseLine() const{
	return currentSettings_->baseLine();
}

QStringList SGMFastScanConfiguration::presets() const{
	QStringList retVal;
	QString tmpStr;
	for(int x = 0; x < settings_.count(); x++)
		retVal << settings_.at(x)->element() + " " + tmpStr.setNum(settings_.at(x)->runSeconds());
	return retVal;
}

SGMFastScanParameters* SGMFastScanConfiguration::currentParameters() const{
	return currentSettings_;
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

bool SGMFastScanConfiguration::setBaseLine(int baseLine){
	currentSettings_->setBaseLine(baseLine);
	emit onBaseLineChanged(currentSettings_->baseLine());
	setModified(true);
	return true;
}

bool SGMFastScanConfiguration::setDetectorConfigurations(AMDetectorInfoSet detectorConfigurations) {
	fastDetectorsConfigurations_ = detectorConfigurations;
	setModified(true);
	return true;
}

SGMFastScanParameters::SGMFastScanParameters(QObject *parent) : QObject(parent)
{
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, int baseLine, QObject *parent) :
		QObject(parent)
{
	setElement(element);
	setRunSeconds(runSeconds);
	setEnergyStart(energyStart);
	setEnergyMidpoint(energyMidpoint);
	setEnergyEnd(energyEnd);
	setVelocity(velocity);
	setVelocityBase(velocityBase);
	setAcceleration(acceleration);
	setScalerTime(scalerTime);
	setBaseLine(baseLine);
}

bool SGMFastScanParameters::operator ==(const SGMFastScanParameters &other){
	if( element() == other.element() &&
	    runSeconds() == other.runSeconds() &&
	    energyStart() == other.energyStart() &&
	    energyMidpoint() == other.energyMidpoint() &&
	    energyEnd() == other.energyEnd() &&
	    velocity() == other.velocity() &&
	    velocityBase() == other.velocityBase() &&
	    acceleration() == other.acceleration() &&
	    scalerTime() == other.scalerTime() &&
	    baseLine() == other.baseLine() ){
		return true;
	}
	return false;
}
