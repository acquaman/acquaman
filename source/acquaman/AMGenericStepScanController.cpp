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

	AMControlInfoList controls;

	AMControlInfo axisControlInfo1 = configuration_->controls().at(0);
	controls.append(axisControlInfo1);
	scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo1.name(), 0, axisControlInfo1.description()));

	if (configuration_->scanAxes().size() == 2){

		int yPoints = int(round((double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()))/double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()))) + 1;
		AMControlInfo axisControlInfo2 = configuration_->controls().at(1);
		controls.append(axisControlInfo2);
		scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo2.name(), yPoints, axisControlInfo2.description()));
	}

	configuration_->setAxisControlInfos(controls);

	AMDetectorInfoSet detectors;

	for (int i = 0, size = configuration_->detectors().size(); i < size; i++)
		detectors.addDetectorInfo(configuration_->detectors().at(i));

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

AMGenericStepScanController::~AMGenericStepScanController()
{

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
	return 0;
}

AMAction3 * AMGenericStepScanController::createCleanupActions()
{
	return 0;
}

void AMGenericStepScanController::buildScanControllerImplementation()
{
}
