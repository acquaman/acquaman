#include "VESPERSEnergyScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMXASScan.h"
#include "actions3/AMListAction3.h"

VESPERSEnergyScanActionController::~VESPERSEnergyScanActionController(){}

VESPERSEnergyScanActionController::VESPERSEnergyScanActionController(VESPERSEnergyScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	useFeedback_ = true;
	originalEnergy_ = VESPERSBeamline::vespers()->mono()->energy();

	AMControlInfoList list;
	list.append(VESPERSBeamline::vespers()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("EnergySetpoint")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MasterDwellTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RingCurrent")->toInfo());

	VESPERS::CCDDetectors ccdDetector = configuration_->ccdDetector();

	if (ccdDetector.testFlag(VESPERS::Roper))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RoperFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Mar))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MarFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Pilatus))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PilatusFileNumber")->toInfo());

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
}

AMAction3* VESPERSEnergyScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *initializationList = new AMSequentialListAction3(new AMSequentialListActionInfo3("Initialization List"));
	initializationList->addSubAction(buildBaseInitializationAction(configuration_->detectorConfigurations()));
	initializationList->addSubAction(buildCCDInitializationAction(configuration_->ccdDetector(), configuration_->ccdFileName()));

	return initializationList;
}

AMAction3* VESPERSEnergyScanActionController::createCleanupActions()
{
	AMListAction3 *cleanupAction = qobject_cast<AMListAction3 *>(buildCleanupAction());

	cleanupAction->addSubAction(VESPERSBeamline::vespers()->mono()->createEaAction(originalEnergy_));

	return cleanupAction;}

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
