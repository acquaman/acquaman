#include "VESPERSTimeScanActionController.h"

#include "actions3/AMListAction3.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMLineScan.h"

VESPERSTimeScanActionController::VESPERSTimeScanActionController(VESPERSTimeScanConfiguration *configuration, QObject *parent)
	: AMTimedScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMLineScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	scan_->rawData()->addScanAxis(AMAxisInfo("Time", 0, "Time", "s"));

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex")->toInfo());
	configuration_->setDetectorConfigurations(detectors);
}
