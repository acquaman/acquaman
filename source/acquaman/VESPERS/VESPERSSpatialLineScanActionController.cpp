#include "VESPERSSpatialLineScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMLineScan.h"
#include "actions3/AMListAction3.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/export/VESPERS/VESPERSExporterLineScanAscii.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

 VESPERSSpatialLineScanActionController::~VESPERSSpatialLineScanActionController(){}
VESPERSSpatialLineScanActionController::VESPERSSpatialLineScanActionController(VESPERSSpatialLineScanConfiguration *configuration, QObject *parent)
	: AMRegionScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AMLineScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSLineScanDefault", configuration_->exportSpectraSources(), false, false, configuration_->exportSpectraInRows());
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSSpatialLineScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

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

	switch (int(configuration_->ccdDetector())){

	case VESPERS::NoCCD:
		break;

	case VESPERS::Roper:
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RoperCCD")->toInfo());
		break;

	case VESPERS::Mar:
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MarCCD")->toInfo());
		break;

	case VESPERS::Pilatus:
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PilatusPixelArrayDetector")->toInfo());
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

void VESPERSSpatialLineScanActionController::buildScanControllerImplementation()
{
	// Need scan axes!
}

AMAction3* VESPERSSpatialLineScanActionController::createInitializationActions()
{
	return buildBaseInitializationAction(configuration_->detectorConfigurations());
}

AMAction3* VESPERSSpatialLineScanActionController::createCleanupActions()
{
	return buildCleanupAction(false);
}

void VESPERSSpatialLineScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
