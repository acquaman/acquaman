#include "AMGenericStepScanController.h"
#include "beamline/AMBeamline.h"

AMGenericStepScanController::AMGenericStepScanController(AMGenericStepScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	AMControlInfo axisControlInfo1 = configuration_->axisControlInfos().at(0);
	scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo1.name(), 0, axisControlInfo1.description()));

	if (configuration_->scanAxes().size() == 2){

		int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();
		AMControlInfo axisControlInfo2 = configuration_->axisControlInfos().at(1);
		scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo2.name(), yPoints, axisControlInfo2.description()));
	}

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

AMGenericStepScanController::~AMGenericStepScanController()
{

}

void AMGenericStepScanController::createAxisOrderMap()
{
	if (scan_->scanRank() == 1){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 0);
	}

	else if (scan_->scanRank() == 2){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
	}
}

void AMGenericStepScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

AMAction3 * AMGenericStepScanController::createInitializationActions()
{
	return AMBeamline::bl()->createScanInitializationAction(configuration_);
}

AMAction3 * AMGenericStepScanController::createCleanupActions()
{
	return AMBeamline::bl()->createScanCleanupAction(configuration_);
}

void AMGenericStepScanController::buildScanControllerImplementation()
{
	AMBeamline::bl()->buildScan(scan_);
}
