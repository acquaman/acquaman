#include "BioXASJJSlitsScanController.h"

BioXASJJSlitsScanController::BioXASJJSlitsScanController(QObject *parent) :
    AMStepScanActionController(parent)
{
	configuration_ = configuration;

	scan_ = new AMScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	AMControlInfo gapControlInfo = configuration_->jjSlitsGap();
	scan_->rawData()->addScanAxis(AMAxisInfo(gapControlInfo.name()), 0, gapControlInfo.description());

	int centerPoints = int(round((double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionStart()))));

	AMControlInfo centerControlInfo = configuration_->jjSlitsCenter();
	scan_->rawData()->addScanAxis(AMAxisInfo(centerControlInfo.name(), centerPoints, centerControlInfo.description()));

	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime();
	elapsedTime_.setInterval(1000);

	connect( this, SIGNAL(started()), &elapsedTime_, SLOT(start()) );
	connect( this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()) );
	connect( this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()) );
	connect( this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()) );
	connect( this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()) );
	connect( this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()) );

	connect( &elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()) );
}

BioXASJJSlitsScanConfiguration::~BioXASJJSlitsScanConfiguration()
{

}

AMAction3* BioXASJJSlitsScanController::createInitializationActions()
{
	return 0;
}

AMAction3* BioXASJJSlitsScanController::createCleanupActions()
{
	return 0;
}

void BioXASJJSlitsScanController::buildScanControllerImplementation()
{

}

void BioXASJJSlitsScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()) {

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
