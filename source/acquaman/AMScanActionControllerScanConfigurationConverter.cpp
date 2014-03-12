#include "AMScanActionControllerScanConfigurationConverter.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMScanConfiguration.h"

 AMScanActionControllerScanConfigurationConverter::~AMScanActionControllerScanConfigurationConverter(){}
AMScanActionControllerScanConfigurationConverter::AMScanActionControllerScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration, QObject *parent) :
	QObject(parent)
{
	scanAssembler_ = scanAssembler;
	scanConfiguration_ = scanConfiguration;
}

bool AMScanActionControllerScanConfigurationConverter::convert(){
	if(!scanAssembler_ || !scanConfiguration_)
		return false;
	return convertImplementation(scanAssembler_, scanConfiguration_);
}

void AMScanActionControllerScanConfigurationConverter::setScanAssembler(AMScanActionControllerScanAssembler *scanAssembler){
	scanAssembler_ = scanAssembler;
}

void AMScanActionControllerScanConfigurationConverter::setScanConfiguration(AMScanConfiguration *scanConfiguration){
	scanConfiguration_ = scanConfiguration;
}
