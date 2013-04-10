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
