#include "VESPERSXASScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMXASScan.h"
#include "actions3/AMListAction3.h"
#include "dataman/export/AMExporterAthena.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

 VESPERSXASScanActionController::~VESPERSXASScanActionController(){}
VESPERSXASScanActionController::VESPERSXASScanActionController(VESPERSEXAFSScanConfiguration *configuration, QObject *parent)
	: AMRegionScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSDefault", configuration_->exportSpectraSources(), true, true, configuration_->exportSpectraInRows());
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSEXAFSScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(vespersDefault->id());

	useFeedback_ = true;

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());

	switch ((int)configuration_->fluorescenceDetector()){

	case VESPERS::SingleElement:

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex")->toInfo());
		break;

	case VESPERS::FourElement:

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex")->toInfo());
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex")->toInfo());
		break;
	}

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

void VESPERSXASScanActionController::buildScanControllerImplementation()
{
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
}

AMAction3* VESPERSXASScanActionController::createInitializationActions()
{
	return buildBaseInitializationAction(configuration_->detectorConfigurations());
}

AMAction3* VESPERSXASScanActionController::createCleanupActions()
{
	return buildCleanupAction(true);
}

void VESPERSXASScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
