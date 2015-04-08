#include "BioXASSollerSlitScanController.h"

#include <math.h>

BioXASSollerSlitScanController::BioXASSollerSlitScanController(BioXASSollerSlitScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	AMControlInfo xControlInfo = configuration_->sollerSlitX();
	scan_->rawData()->addScanAxis(AMAxisInfo(xControlInfo.name(), 0, xControlInfo.description()));

	int yPoints = int(round((double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()))/double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()))) + 1;
	AMControlInfo zControlInfo = configuration_->sollerSlitZ();
	scan_->rawData()->addScanAxis(AMAxisInfo(zControlInfo.name(), yPoints, zControlInfo.description()));

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

BioXASSollerSlitScanController::~BioXASSollerSlitScanController()
{

}

AMAction3 * BioXASSollerSlitScanController::createInitializationActions()
{
	return 0;
}

AMAction3 * BioXASSollerSlitScanController::createCleanupActions()
{
	return 0;
}

void BioXASSollerSlitScanController::buildScanControllerImplementation()
{
}

void BioXASSollerSlitScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
