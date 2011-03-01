#include "SGMFastScanConfiguration.h"

#include <QFile>
#include <QDir>

SGMFastScanConfiguration::SGMFastScanConfiguration(QObject *parent) : AMFastScanConfiguration(parent), SGMScanConfiguration()
{
	currentSettings_ = 0; //NULL
	sensibleFileSavePath_ = "";
	finalizedSavePath_ = "";
	sensibleFileSaveWarning_ = "";

	settings_.append( new SGMFastScanParameters("Nitrogen", 5.0, 400.0, 415.0, 430.0, 10000, 10000, 10000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Nitrogen", 20.0, 400.0, 415.0, 430.0, 1000, 1000, 1000, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 5.0, 530.0, 545.0, 560.0, 10000, 10000, 10000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 20.0, 530.0, 545.0, 560.0, 1000, 1000, 1000, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Copper", 5.0, 925.0, 935.0, 945.0, 3000, 3000, 3000, 5.0, 200, this));
	settings_.append( new SGMFastScanParameters("Copper", 20.0, 925.0, 935.0, 945.0, 450, 450, 450, 20.0, 800, this));
	settings_.append( new SGMFastScanParameters("Carbon", 5.0, 280.0, 295.0, 320.0, 19000, 19000, 19000, 5.0, 200, this));

	setParametersFromPreset(0);

//	connect(&savePathWatcher_, SIGNAL(directoryChanged(QString)), this, SLOT(onSaveDirectoryChanged(QString)));
}

SGMFastScanConfiguration::~SGMFastScanConfiguration(){
	while(settings_.count() > 0)
		delete settings_.takeLast();
}

AMScanConfiguration* SGMFastScanConfiguration::createCopy() const{
	return new SGMFastScanConfiguration(*this);
}

#include "SGMFastDacqScanController.h"

AMScanController* SGMFastScanConfiguration::createController(){
	return new SGMFastDacqScanController(this);
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

QString SGMFastScanConfiguration::sensibleFileSavePath() const{
	return sensibleFileSavePath_;
}

QString SGMFastScanConfiguration::finalizedSavePath() const{
	return finalizedSavePath_;
}

QString SGMFastScanConfiguration::sensibleFileSaveWarning() const {
	return sensibleFileSaveWarning_;
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

QList<AMDetectorInfo*> SGMFastScanConfiguration::usingDetectors() const{
	QList<AMDetectorInfo*> usingDetectors;
	usingDetectors << SGMBeamline::sgm()->XASDetectors()->detectorByName("tey");
	usingDetectors << SGMBeamline::sgm()->feedbackDetectors()->detectorByName("I0");
	usingDetectors << SGMBeamline::sgm()->XASDetectors()->detectorByName("tfy");
	usingDetectors << SGMBeamline::sgm()->allDetectors()->detectorByName("photodiode");
	//usingDetectors << SGMBeamline::sgm()->allDetectors()->detectorByName("encoderUp");
	//usingDetectors << SGMBeamline::sgm()->allDetectors()->detectorByName("encoderDown");
	return usingDetectors;
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
	setStart(currentSettings_->energyStart());
	setEnd(currentSettings_->energyEnd());
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
	return true;
}

bool SGMFastScanConfiguration::setElement(const QString& element){
	currentSettings_->setElement(element);
	emit onElementChanged(currentSettings_->element());
	return true;
}

bool SGMFastScanConfiguration::setRunSeconds(double runSeconds){
	currentSettings_->setRunSeconds(runSeconds);
	emit onRunSecondsChanged(currentSettings_->runSeconds());
	return true;
}

bool SGMFastScanConfiguration::setEnergyStart(double energyStart){
	currentSettings_->setEnergyStart(energyStart);
	setStart(energyStart);
	emit onEnergyStartChanged(currentSettings_->energyStart());
	return true;
}

bool SGMFastScanConfiguration::setEnergyMidpoint(double energyMidpoint){
	currentSettings_->setEnergyMidpoint(energyMidpoint);
	emit onEnergyMidpointChanged(currentSettings_->energyMidpoint());
	return true;
}

bool SGMFastScanConfiguration::setEnergyEnd(double energyEnd){
	currentSettings_->setEnergyEnd(energyEnd);
	setEnd(energyEnd);
	emit onEnergyEndChanged(currentSettings_->energyEnd());
	return true;
}

bool SGMFastScanConfiguration::setVelocity(int velocity){
	currentSettings_->setVelocity(velocity);
	emit onVelocityChanged(currentSettings_->velocity());
	return true;
}

bool SGMFastScanConfiguration::setVelocityBase(int velocityBase){
	currentSettings_->setVelocityBase(velocityBase);
	emit onVelocityBaseChanged(currentSettings_->velocityBase());
	return true;
}

bool SGMFastScanConfiguration::setAcceleration(int acceleration){
	currentSettings_->setAcceleration(acceleration);
	emit onAccelerationChanged(currentSettings_->acceleration());
	return true;
}

bool SGMFastScanConfiguration::setScalerTime(double scalerTime){
	currentSettings_->setScalerTime(scalerTime);
	emit onScalerTimeChanged(currentSettings_->scalerTime());
	return true;
}

bool SGMFastScanConfiguration::setBaseLine(int baseLine){
	currentSettings_->setBaseLine(baseLine);
	emit onBaseLineChanged(currentSettings_->baseLine());
	return true;
}

bool SGMFastScanConfiguration::setSensibleFileSavePath(const QString &sensibleFileSavePath){
	/*
	QString path = sensibleFileSavePath.section('/', 0, -2);
	QString file = sensibleFileSavePath.section('/', -1);
	//file.append(".dat");
	QDir dir(path);
	if(file.contains(".") || file.contains("_") || file.contains(" ")){
		sensibleFileSaveWarning_ = "Files cannot '.', '_' or ' '";
		return false;
	}
	if(!dir.exists()){
		sensibleFileSaveWarning_ = "Directory does not exist";
		return false;
	}
	if(!sensibleFileSavePath_.isEmpty())
		savePathWatcher_.removePath(sensibleFileSavePath_.section('/', 0, -2));
	sensibleFileSavePath_ = sensibleFileSavePath;
	savePathWatcher_.addPath(path);
	QStringList filefilters, likeFiles;
	filefilters << QString("%1_*.dat").arg(file);
	likeFiles = dir.entryList(filefilters, QDir::Files);
	int pIndex, aIndex, maxIndex;
	maxIndex = 0;
	foreach(QString f, likeFiles){
		pIndex = f.indexOf('_');
		aIndex = f.indexOf(".dat");
		maxIndex = std::max(f.mid(pIndex+1, aIndex-(pIndex+1)).toInt(), maxIndex);
	}
	finalizedSavePath_ = QString("%1/%2_%3.dat").arg(path).arg(file).arg(maxIndex+1);
	emit onNewFinalizedSavePath(finalizedSavePath_);
	emit onSensibleFileSavePathChanged(sensibleFileSavePath_);
	return true;
	*/
}

void SGMFastScanConfiguration::onSaveDirectoryChanged(const QString &directory){
	setSensibleFileSavePath(sensibleFileSavePath_);
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
