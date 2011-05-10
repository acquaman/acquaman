#include "AMFastScanConfiguration.h"

AMFastScanConfiguration::AMFastScanConfiguration(QObject *parent) :
    AMScanConfiguration(parent)
{
	startEnergy_ = -1;
	endEnergy_ = -1;
}

bool AMFastScanConfiguration::setStartEnergy(double startEnergy) {
	if(startEnergy != startEnergy_){
		startEnergy_ = startEnergy;
		setModified(true);
		emit energyRangeChanged();
	}
	return true;
}

bool AMFastScanConfiguration::setEndEnergy(double endEnergy) {
	if(endEnergy != endEnergy_){
		endEnergy_ = endEnergy;
		setModified(true);
		emit energyRangeChanged();
	}
	return true;
}
