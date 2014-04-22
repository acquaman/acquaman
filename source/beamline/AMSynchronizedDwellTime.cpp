#include "AMSynchronizedDwellTime.h"

#include "beamline/AMDetector.h"

 AMSynchronizedDwellTime::~AMSynchronizedDwellTime(){}
AMSynchronizedDwellTime::AMSynchronizedDwellTime(QObject *parent) :
	QObject(parent)
{
}

int AMSynchronizedDwellTime::indexOfDetector(const AMDetector *detector) const{
	QStringList currentKeys = keys();
	for(int x = 0; x < currentKeys.count(); x++)
		if(currentKeys.at(x) == detector->synchronizedDwellKey())
			return x;

	return -1;
}
