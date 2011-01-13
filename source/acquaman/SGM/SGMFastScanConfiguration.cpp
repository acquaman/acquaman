#include "SGMFastScanConfiguration.h"

SGMFastScanConfiguration::SGMFastScanConfiguration(QObject *parent) : AMFastScanConfiguration(parent), SGMScanConfiguration()
{
	element_ = "";
	runTime_ = -1;

	settings_.append( new SGMFastScanParameters("Nitrogen", 5.0, 400.0, 415.0, 430.0, 10000, 10000, 10000, 5.0, this));
	settings_.append( new SGMFastScanParameters("Nitrogen", 20.0, 400.0, 415.0, 430.0, 1000, 1000, 1000, 25.0, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 5.0, 530.0, 545.0, 560.0, 10000, 10000, 10000, 5.0, this));
	settings_.append( new SGMFastScanParameters("Oxygen", 20.0, 530.0, 545.0, 560.0, 1000, 1000, 1000, 25.0, this));
	settings_.append( new SGMFastScanParameters("Copper", 5.0, 940.0, 955.0, 970.0, 10000, 10000, 10000, 5.0, this));
	settings_.append( new SGMFastScanParameters("Copper", 20.0, 940.0, 955.0, 970.0, 1000, 1000, 1000, 25.0, this));
}

SGMFastScanConfiguration::~SGMFastScanConfiguration(){
	while(settings_.count() > 0)
		delete settings_.takeLast();
}

QStringList SGMFastScanConfiguration::options() const{
	QStringList retVal;
	QString tmpStr;
	for(int x = 0; x < settings_.count(); x++)
		retVal << settings_.at(x)->element() + " " + tmpStr.setNum(settings_.at(x)->runSeconds());
	return retVal;
}

bool SGMFastScanConfiguration::setParameters(const QString &element, double runTime){
	for(int x = 0; x < settings_.count(); x++)
		if(settings_.at(x)->element() == element && settings_.at(x)->runSeconds() == runTime){
			element_ = element;
			runTime_ = runTime;
			start_ = settings_.at(x)->energyStart();
			end_ = settings_.at(x)->energyEnd();
			return true;
		}
	return false;
}

/*
bool SGMFastScanConfiguration::setElement(const QString &element){
	for(int x = 0; x < settings_.count(); x++)
		if(settings_.at(x)->element() == element){
			element_ = element;
			return true;
		}
	return false;
}

bool SGMFastScanConfiguration::setRunTime(double runTime){
	for(int x = 0; x < settings_.count(); x++)
		if(settings_.at(x)->runSeconds() == runTime){
			runTime_ = runTime;
			return true;
		}
	return false;
}
*/

SGMFastScanParameters::SGMFastScanParameters(QObject *parent) : QObject(parent)
{
}

SGMFastScanParameters::SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, QObject *parent) :
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
}
