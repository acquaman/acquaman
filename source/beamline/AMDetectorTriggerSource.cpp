#include "AMDetectorTriggerSource.h"

AMDetectorTriggerSource::AMDetectorTriggerSource(const QString &name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

void AMDetectorTriggerSource::trigger(){
	emit triggered();
}
