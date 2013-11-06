#include "VESPERSSpatialLineScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMLineScan.h"
#include "actions3/AMListAction3.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/export/VESPERS/VESPERSExporterLineScanAscii.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

VESPERSSpatialLineScanActionController::VESPERSSpatialLineScanActionController(VESPERSSpatialLineScanConfiguration *configuration, QObject *parent)
	: AMRegionScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMLineScan();
	scan_->setName(regionsConfiguration_->name());
	scan_->setScanConfiguration(regionsConfiguration_);
	scan_->setFileFormat("sgm2013XAS");
	scan_->setRunId(AMUser::user()->currentRunId());
//	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");
//	scan_->setNotes(buildNotes());

	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSLineScanDefault", configuration_->exportSpectraSources(), false, false, configuration_->exportSpectraInRows());
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSSpatialLineScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(2)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(3)->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(4)->toInfo());

	switch ((int)configuration_->fluorescenceDetector()){

	case VESPERS::SingleElement:
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(0)->toInfo());
		break;

	case VESPERS::FourElement:
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(1)->toInfo());
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(0)->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectors()->at(1)->toInfo());
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

AMAction3* VESPERSSpatialLineScanActionController::createInitializationActions()
{
	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Initialization Stage 1", "VESPERS Initialization Stage 1"), AMListAction3::Parallel);

	CLSSynchronizedDwellTime *synchronizedDwell = qobject_cast<CLSSynchronizedDwellTime*>(VESPERSBeamline::vespers()->synchronizedDwellTime());
	QStringList dwellKeys;

	for (int i = 0, size = configuration_->detectorConfigurations().count(); i < size; i++)
		dwellKeys << VESPERSBeamline::vespers()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(i))->synchronizedDwellKey();

	dwellKeys.removeDuplicates();

	for (int i = 0, size = synchronizedDwell->elementCount(); i < size; i++){

		if (synchronizedDwell->enabledAt(i) && !dwellKeys.contains(synchronizedDwell->keyAt(i)))
			stage1->addSubAction(synchronizedDwell->elementAt(i)->createEnableAction3(false));

		else if (!synchronizedDwell->enabledAt(i) && dwellKeys.contains(synchronizedDwell->keyAt(i)))
			stage1->addSubAction(synchronizedDwell->elementAt(i)->createEnableAction3(true));
	}

	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	stage1->addSubAction(scaler->createStartAction3(false));
	stage1->addSubAction(scaler->createScansPerBufferAction3(1));
	stage1->addSubAction(scaler->createTotalScansAction3(1));

	return stage1;
}

AMAction3* VESPERSSpatialLineScanActionController::createCleanupActions()
{
	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup", "VESPERS Cleanup"), AMListAction3::Sequential);
	AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup Stage 1", "VESPERS Cleanup Stage 1"), AMListAction3::Parallel);

	CLSSynchronizedDwellTime *synchronizedDwell = qobject_cast<CLSSynchronizedDwellTime*>(VESPERSBeamline::vespers()->synchronizedDwellTime());

	// The scaler is always first.  Therefore, we know this will always only ensure the scaler is still enabled.
	for (int i = 0, size = synchronizedDwell->elementCount(); i < size; i++)
		stage1->addSubAction(synchronizedDwell->elementAt(i)->createEnableAction3(i == 0));

	AMListAction3 *stage2 = new AMListAction3(new AMListActionInfo3("VESPERS Cleanup Stage 2", "VESPERS Cleanup Stage 2"), AMListAction3::Parallel);
	stage2->addSubAction(synchronizedDwell->createMasterTimeAction3(1));
	stage2->addSubAction(synchronizedDwell->createModeAction3(CLSSynchronizedDwellTime::Continuous));

	cleanup->addSubAction(stage1);
	cleanup->addSubAction(stage2);

	return cleanup;
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
