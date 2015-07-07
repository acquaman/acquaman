#include "BioXASXASScanActionController.h"

#include "actions3/AMListAction3.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMXASScan.h"

#include "util/AMErrorMonitor.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setName(configuration_->name());
	scan_->setFileFormat("amCDFv1");
	scan_->setScanConfiguration(configuration_);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
	scan_->setNotes(beamlineSettings());

	useFeedback_ = true;

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

BioXASXASScanActionController::~BioXASXASScanActionController()
{

}

QString BioXASXASScanActionController::beamlineSettings()
{
	QString notes;

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(CLSStorageRing::sr1()->ringCurrent()));

	return notes;
}

AMAction3* BioXASXASScanActionController::createInitializationActions()
{
	AMAction3 *result = 0;

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		double regionTime = double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

		AMSequentialListAction3 *initializationAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS XAS Scan Initialization Actions", "BioXAS Main Scan Initialization Actions"));

		AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("Initialization Stage 1", "Initialization Stage 1"), AMListAction3::Parallel);
		stage1->addSubAction(scaler->createContinuousEnableAction3(false));

		AMListAction3 *stage2 = new AMListAction3(new AMListActionInfo3("Initialization Stage 2", "Initialization Stage 2"), AMListAction3::Parallel);

		stage2->addSubAction(scaler->createStartAction3(false));
		stage2->addSubAction(scaler->createScansPerBufferAction3(1));
		stage2->addSubAction(scaler->createTotalScansAction3(1));

		AMListAction3 *stage3 = new AMListAction3(new AMListActionInfo3("Initialization Stage 3", "Initialization Stage 3"), AMListAction3::Parallel);
		stage3->addSubAction(scaler->createStartAction3(true));
		stage3->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));

		AMListAction3 *stage4 = new AMListAction3(new AMListActionInfo3("Initialization Stage 4", "Initialization Stage 4"), AMListAction3::Parallel);
		stage4->addSubAction(scaler->createStartAction3(true));
		stage4->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));

		initializationAction->addSubAction(stage1);
		initializationAction->addSubAction(stage2);
		initializationAction->addSubAction(scaler->createDwellTimeAction3(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));
		initializationAction->addSubAction(stage3);
		initializationAction->addSubAction(stage4);

		result = initializationAction;

	} else {
		AMErrorMon::alert(this, BIOXASXASSCANACTIONCONTROLLER_SCALER_NOT_FOUND, "Failed to complete scan initialization--valid scaler not found.");
	}

	return result;
}

AMAction3* BioXASXASScanActionController::createCleanupActions()
{
	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("BioXAS XAS Scan Cleanup", "BioXAS XAS Scan Cleanup"), AMListAction3::Sequential);
	cleanup->addSubAction(scaler->createDwellTimeAction3(1.0));
	cleanup->addSubAction(scaler->createContinuousEnableAction3(true));

	return cleanup;
}

void BioXASXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void BioXASXASScanActionController::buildScanControllerImplementation()
{

}

void BioXASXASScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
