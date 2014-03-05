#include "AMTimedScanConfigurationConverter.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMTimedRegionScanConfiguration.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMTimedScanConfigurationConverter::AMTimedScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler, AMTimedRegionScanConfiguration *scanConfiguration, QObject *parent)
	: AMScanActionControllerScanConfigurationConverter(scanAssembler, scanConfiguration, parent)
{
}

bool AMTimedScanConfigurationConverter::convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration)
{
	AMTimedRegionScanConfiguration *timedConfiguration = qobject_cast<AMTimedRegionScanConfiguration *>(scanConfiguration);

	if (!timedConfiguration){

		AMErrorMon::alert(this, AMTIMEDSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION, "The timed region scan configuration was invalid.");
		return false;
	}

	AMScanAxisRegion region(0, timedConfiguration->time(), timedConfiguration->totalTime(), timedConfiguration->time());
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);

	if (!scanAssembler->appendAxis(0, axis)){

		AMErrorMon::alert(this, AMTIMEDSCANCONFIGURATIONCONVERTER_INVALID_SCAN_AXIS, QString("Could not add the %1 scan axis to the scan assembler.").arg(axis->name()));
		return false;
	}

	for (int i = 0, size = timedConfiguration->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(timedConfiguration->detectorConfigurations().at(i));

		if (oneDetector && !scanAssembler->addDetector(oneDetector)){

			AMErrorMon::alert(this, AMTIMEDSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return false;
		}
	}

	return true;
}
