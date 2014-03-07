#include "VESPERS2DScanActionController.h"

#include "actions3/AMListAction3.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AM2DScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/VESPERS/VESPERSExporter2DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporterSMAK.h"

VESPERS2DScanActionController::VESPERS2DScanActionController(VESPERS2DScanConfiguration *configuration, QObject *parent)
	: AM2DScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AM2DScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
//	scan_->setFileFormat("amRegionAscii2013");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	if (configuration_->exportAsAscii()){

		AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERS2DDefault", configuration_->exportSpectraSources(), false, false, configuration_->exportSpectraInRows());
		if(vespersDefault->id() > 0)
			AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporter2DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());
	}

	else{

		// SMAK format requires a specific spectra file format.
		AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERS2DDefault", configuration_->exportSpectraSources(), false, false, true);
		if(vespersDefault->id() > 0)
			AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporterSMAK, AMExporterOptionGeneralAscii>(vespersDefault->id());
	}

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MasterDwellTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RingCurrent")->toInfo());

	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();

	if (xrfDetector.testFlag(VESPERS::SingleElement)){

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexDeadTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRealTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexLiveTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexFastPeaks")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexSlowPeaks")->toInfo());
	}

	if (xrfDetector.testFlag(VESPERS::FourElement)){

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks4")->toInfo());
	}

	VESPERS::CCDDetectors ccdDetector = configuration_->ccdDetector();

	if (ccdDetector == VESPERS::Roper)
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RoperCCD")->toInfo());

	if (ccdDetector == VESPERS::Mar)
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MarCCD")->toInfo());

	if (ccdDetector == VESPERS::Pilatus)
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PilatusPixelArrayDetector")->toInfo());

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

void VESPERS2DScanActionController::buildScanControllerImplementation()
{
	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();
	AMXRFDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex"));

	else if (xrfDetector.testFlag(VESPERS::FourElement))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex"));

	if (detector){

		foreach (AMRegionOfInterest *region, detector->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name(), this);
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource(detector->name())));
			scan_->addAnalyzedDataSource(newRegion);
		}
	}
}


AMAction3* VESPERS2DScanActionController::createInitializationActions()
{
	return buildBaseInitializationAction(configuration_->detectorConfigurations());
}

AMAction3* VESPERS2DScanActionController::createCleanupActions()
{
	return buildCleanupAction();
}

void VESPERS2DScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
