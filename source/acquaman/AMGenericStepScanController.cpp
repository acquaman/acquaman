#include "AMGenericStepScanController.h"

#include "beamline/AMBeamline.h"
#include <QDebug>

AMGenericStepScanController::AMGenericStepScanController(AMGenericStepScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	qDebug() << "1. Beginning of AMGenericStepScanController constructor.";

	configuration_ = configuration;

	qDebug() << "2. Configuration set. About to create scan.";

	scan_ = new AMScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	qDebug() << "3. Scan created. About to set axis control infos from configuration.";

	AMControlInfo axisControlInfo1 = configuration_->axisControlInfos().at(0);
	scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo1.name(), 0, axisControlInfo1.description()));

	qDebug() << "4. First axis control info set.";

	qDebug() << "5. Setting any additional axis control infos.";

	if (configuration_->scanAxes().size() == 2){

		qDebug() << "\tPreparing to set additional axis control info.";
		int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();
		qDebug() << "\tAdditonal configuration scan axis has" << yPoints << "points.";
		AMControlInfo axisControlInfo2 = configuration_->axisControlInfos().at(1);
		scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo2.name(), yPoints, axisControlInfo2.description()));

		qDebug() << "\tAdditional axis control info set.";
	}

	qDebug() << "6. All axis control infos set.";

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

	qDebug() << "7. End of AMGenericStepScanController constructor.";
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
	return 0;
}

AMAction3 * AMGenericStepScanController::createCleanupActions()
{
	return 0;
}

void AMGenericStepScanController::buildScanControllerImplementation()
{
}
