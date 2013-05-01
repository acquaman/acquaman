#include "AMDetectorTriggerSource.h"

AMDetectorTriggerSource::AMDetectorTriggerSource(const QString &name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

void AMDetectorTriggerSource::trigger(AMDetectorDefinitions::ReadMode readMode){
	emit triggered(readMode);
}

void AMDetectorTriggerSource::setSucceeded(){
	emit succeeded();
}

void AMDetectorTriggerSource::setFailed(){
	emit failed();
}

AMDetectorDwellTimeSource::AMDetectorDwellTimeSource(const QString &name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

void AMDetectorDwellTimeSource::requestSetDwellTime(double dwellSeconds){
	emit setDwellTime(dwellSeconds);
}

void AMDetectorDwellTimeSource::setSucceeded(){
	emit succeeded();
}

void AMDetectorDwellTimeSource::setFailed(){
	emit failed();
}
