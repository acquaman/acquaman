#include "AMDetectorTriggerSource.h"

 AMDetectorTriggerSource::~AMDetectorTriggerSource(){}
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

 AMDetectorDwellTimeSource::~AMDetectorDwellTimeSource(){}
AMDetectorDwellTimeSource::AMDetectorDwellTimeSource(const QString &name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

void AMDetectorDwellTimeSource::requestSetDwellTime(double dwellSeconds){
    emit setDwellTime(dwellSeconds);
}

#include <QDebug>
void AMDetectorDwellTimeSource:: requestSetDarkCurrentCorrectionTime(double timeSeconds) {
    qDebug() << "Want to emit darkCurrentCorrectionTime as " << timeSeconds << " in " << name();
    emit setDarkCurrentCorrectionTime(timeSeconds);

    emit darkCurrentTimeChanged(timeSeconds);
}

void AMDetectorDwellTimeSource::setSucceeded(){
	emit succeeded();
}

void AMDetectorDwellTimeSource::setFailed(){
	emit failed();
}
