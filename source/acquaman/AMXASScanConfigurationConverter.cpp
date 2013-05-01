#include "AMXASScanConfigurationConverter.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMXASScanConfiguration.h"
#include "beamline/AMBeamline.h"

AMXASScanConfigurationConverter::AMXASScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler, AMXASScanConfiguration *scanConfiguration, QObject *parent) :
	AMScanActionControllerScanConfigurationConverter(scanAssembler, scanConfiguration, parent)
{
}

bool AMXASScanConfigurationConverter::convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration){
	AMXASScanConfiguration *xasScanConfiguration = qobject_cast<AMXASScanConfiguration*>(scanConfiguration);
	if(!xasScanConfiguration || xasScanConfiguration->regionCount() == 0)
		return false;

	AMScanAxisRegion firstRegion(xasScanConfiguration->regionStart(0), xasScanConfiguration->regionDelta(0), xasScanConfiguration->regionEnd(0), xasScanConfiguration->regionTime(0));
	AMScanAxis *energyAxis = new AMScanAxis(AMScanAxis::StepAxis, firstRegion);
	for(int x = 1; x < xasScanConfiguration->regionCount(); x++){
		AMScanAxisRegion anotherRegion(xasScanConfiguration->regionStart(x), xasScanConfiguration->regionDelta(x), xasScanConfiguration->regionEnd(x), xasScanConfiguration->regionTime(x));
		if(!energyAxis->appendRegion(anotherRegion))
			return false;
	}

	if(!scanAssembler->appendAxis(xasScanConfiguration->regions()->defaultControl(), energyAxis))
		return false;

	AMDetector *oneDetector;
	for(int x = 0; x < xasScanConfiguration->detectorConfigurations().count(); x++){
		oneDetector = AMBeamline::bl()->exposedDetectorByInfo(xasScanConfiguration->detectorConfigurations().at(x));
		if(oneDetector)
			if(!scanAssembler->addDetector(oneDetector))
				return false;
	}

	return true;
}
