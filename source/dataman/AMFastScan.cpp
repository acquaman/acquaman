#include "AMFastScan.h"

AMFastScan::AMFastScan(QObject *parent) :
		AMScan(parent)
{
	AMAxisInfo energyAxis("ev", 0, "Incident Energy", "eV");
	//data_->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );
	data_->addScanAxis(energyAxis);
}

bool AMFastScan::loadDataImplementation(){
	return false;
}
