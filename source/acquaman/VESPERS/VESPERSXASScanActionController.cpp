#include "VESPERSXASScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMXASScan.h"

VESPERSXASScanActionController::VESPERSXASScanActionController(VESPERSEXAFSScanConfiguration *configuration, QObject *parent)
	: AMRegionScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setName(regionsConfiguration_->name());
	scan_->setScanConfiguration(regionsConfiguration_);
	scan_->setFileFormat("sgm2013XAS");
	scan_->setRunId(AMUser::user()->currentRunId());
//	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(2)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(3)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(4)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(1)->toInfo());

	configuration_->setDetectorConfigurations(detectors);
}

AMAction3* VESPERSXASScanActionController::createInitializationActions()
{
	return 0;
}

AMAction3* VESPERSXASScanActionController::createCleanupActions()
{
	return 0;
}

