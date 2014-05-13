#include "AM2DScanConfigurationConverter.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AM2DScanConfiguration.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AM2DScanConfigurationConverter::AM2DScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler, AM2DScanConfiguration *scanConfiguration, QObject *parent)
	: AMScanActionControllerScanConfigurationConverter(scanAssembler, scanConfiguration, parent)
{
}

bool AM2DScanConfigurationConverter::convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration)
{
	AM2DScanConfiguration *configuration = qobject_cast<AM2DScanConfiguration *>(scanConfiguration);

	if (!configuration){

		AMErrorMon::alert(this, AM2DSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION, "The 2D scan configuration was invalid.");
		return false;
	}

	if (!configuration->validXAxis()){

		AMErrorMon::alert(this, AM2DSCANCONFIGURATIONCONVERTER_INVALID_XAXIS, "Invalid x-axis.");
		return false;
	}

	AMScanAxisRegion *xAxisRegion = new AMScanAxisRegion(configuration->xStart(), configuration->xStep(), configuration->xEnd(), configuration->timeStep());
	xAxisRegion->setName("x-axis region");
	AMScanAxis *xAxis = new AMScanAxis(AMScanAxis::StepAxis, xAxisRegion);
	xAxis->setName("x-axis");

	if (!configuration->validYAxis()){

		AMErrorMon::alert(this, AM2DSCANCONFIGURATIONCONVERTER_INVALID_YAXIS, "Invalid y-axis.");
		return false;
	}

	AMScanAxisRegion *yAxisRegion = new AMScanAxisRegion(configuration->yStart(), configuration->yStep(), configuration->yEnd(), configuration->timeStep());
	yAxisRegion->setName("y-axis region");
	AMScanAxis *yAxis = new AMScanAxis(AMScanAxis::StepAxis, yAxisRegion);
	yAxis->setName("y-axis");

	if (!scanAssembler->appendAxis(AMBeamline::bl()->exposedControlByInfo(configuration->axisControlInfos().at(1)), yAxis)){

		AMErrorMon::alert(this, AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_Y_AXIS, "Could not add y-axis.");
		return false;
	}

	if (!scanAssembler->appendAxis(AMBeamline::bl()->exposedControlByInfo(configuration->axisControlInfos().at(0)), xAxis)){

		AMErrorMon::alert(this, AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_X_AXIS, "Could not add x-axis.");
		return false;
	}

	for (int i = 0, size = configuration->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(configuration->detectorConfigurations().at(i));

		if (oneDetector && !scanAssembler->addDetector(oneDetector)){

			AMErrorMon::alert(this, AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return false;
		}
	}

	return true;
}
