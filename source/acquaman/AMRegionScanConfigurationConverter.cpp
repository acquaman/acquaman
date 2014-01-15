#include "AMRegionScanConfigurationConverter.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMRegionScanConfiguration.h"
#include "beamline/AMBeamline.h"

AMRegionScanConfigurationConverter::~AMRegionScanConfigurationConverter(){}

AMRegionScanConfigurationConverter::AMRegionScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler, AMRegionScanConfiguration *scanConfiguration, QObject *parent)
	: AMScanActionControllerScanConfigurationConverter(scanAssembler, scanConfiguration, parent)
{
}

bool AMRegionScanConfigurationConverter::convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration)
{
	AMRegionScanConfiguration *regionScanConfiguration = qobject_cast<AMRegionScanConfiguration*>(scanConfiguration);

	if (!regionScanConfiguration || regionScanConfiguration->regionCount() == 0)
		return false;

	AMScanAxisRegion firstRegion(regionScanConfiguration->regionStart(0), regionScanConfiguration->regionDelta(0), regionScanConfiguration->regionEnd(0), regionScanConfiguration->regionTime(0));
	AMScanAxis *fullRegionAxis = new AMScanAxis(AMScanAxis::StepAxis, firstRegion);

	for (int x = 1; x < regionScanConfiguration->regionCount(); x++){

		AMScanAxisRegion anotherRegion(regionScanConfiguration->regionStart(x), regionScanConfiguration->regionDelta(x), regionScanConfiguration->regionEnd(x), regionScanConfiguration->regionTime(x));

		if (!fullRegionAxis->appendRegion(anotherRegion))
			return false;
	}

	if (!scanAssembler->appendAxis(regionScanConfiguration->regions()->defaultControl(), fullRegionAxis))
		return false;

	AMDetector *oneDetector;

	for (int x = 0; x < regionScanConfiguration->detectorConfigurations().count(); x++){

		oneDetector = AMBeamline::bl()->exposedDetectorByInfo(regionScanConfiguration->detectorConfigurations().at(x));

		if (oneDetector && !scanAssembler->addDetector(oneDetector))
			return false;
	}

	return true;
}
