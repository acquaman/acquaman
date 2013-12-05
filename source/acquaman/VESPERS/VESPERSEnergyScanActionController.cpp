#include "VESPERSEnergyScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMXASScan.h"
#include "actions3/AMListAction3.h"

VESPERSEnergyScanActionController::VESPERSEnergyScanActionController(VESPERSEnergyScanConfiguration *configuration, QObject *parent)
	: AMRegionScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("sgm2013XAS");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(2)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(3)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(4)->toInfo());

	configuration_->setDetectorConfigurations(detectors);

	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));
}

void VESPERSEnergyScanActionController::buildScanControllerImplementation()
{
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
}

AMAction3* VESPERSEnergyScanActionController::createInitializationActions()
{
	return buildBaseInitializationAction(configuration_->detectorConfigurations());
}

AMAction3* VESPERSEnergyScanActionController::createCleanupActions()
{
	return buildCleanupAction(true);
}

void VESPERSEnergyScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
