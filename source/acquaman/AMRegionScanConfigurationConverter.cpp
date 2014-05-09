#include "AMRegionScanConfigurationConverter.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMRegionScanConfiguration.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMRegionScanConfigurationConverter::~AMRegionScanConfigurationConverter(){}

AMRegionScanConfigurationConverter::AMRegionScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler, AMRegionScanConfiguration *scanConfiguration, QObject *parent)
	: AMScanActionControllerScanConfigurationConverter(scanAssembler, scanConfiguration, parent)
{
}

bool AMRegionScanConfigurationConverter::convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration)
{
	AMRegionScanConfiguration *regionScanConfiguration = qobject_cast<AMRegionScanConfiguration*>(scanConfiguration);

	if (!regionScanConfiguration || regionScanConfiguration->regionCount() == 0){

		AMErrorMon::alert(this, AMREGIONSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION, "The region scan configuration was invalid or contained no regions.");
		return false;
	}

	AMScanAxisRegion *firstRegion = new AMScanAxisRegion(regionScanConfiguration->regionStart(0), regionScanConfiguration->regionDelta(0), regionScanConfiguration->regionEnd(0), regionScanConfiguration->regionTime(0));
	AMScanAxis *fullRegionAxis = new AMScanAxis(AMScanAxis::StepAxis, firstRegion);

	for (int x = 1; x < regionScanConfiguration->regionCount(); x++){

		AMScanAxisRegion *anotherRegion = new AMScanAxisRegion(regionScanConfiguration->regionStart(x), regionScanConfiguration->regionDelta(x), regionScanConfiguration->regionEnd(x), regionScanConfiguration->regionTime(x));

		if (!fullRegionAxis->appendRegion(anotherRegion)){

			AMErrorMon::alert(this, AMREGIONSCANCONFIGURATIONCONVERTER_INVALID_REGION, QString("Could not add %1 to the scan axis").arg(anotherRegion->name()));
			return false;
		}
	}

	if (!scanAssembler->appendAxis(regionScanConfiguration->regions()->defaultControl(), fullRegionAxis)){

		AMErrorMon::alert(this, AMREGIONSCANCONFIGURATIONCONVERTER_INVALID_SCAN_AXIS, QString("Could not add the %1 scan axis to the scan assembler.").arg(fullRegionAxis->name()));
		return false;
	}

	for (int x = 0; x < regionScanConfiguration->detectorConfigurations().count(); x++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(regionScanConfiguration->detectorConfigurations().at(x));

		if (oneDetector && !scanAssembler->addDetector(oneDetector)){

			AMErrorMon::alert(this, AMREGIONSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return false;
		}
	}

	return true;
}
