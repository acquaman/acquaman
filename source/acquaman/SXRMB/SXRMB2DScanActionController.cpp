#include "SXRMB2DScanActionController.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

#include "application/AMAppControllerSupport.h"
#include "beamline/SXRMB/SXRMBBeamline.h"
#include "dataman/AM2DScan.h"
#include "analysis/AM2DNormalizationAB.h"

SXRMB2DScanActionController::SXRMB2DScanActionController(SXRMB2DMapScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AM2DScan();
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	int yPoints = int(round((double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()))/double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()))) + 1;

	AMControlInfoList list;
	list.append(SXRMBBeamline::sxrmb()->microprobeSampleStageX()->toInfo());
	list.append(SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->toInfo());
	scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
	scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));

	QString scanName = configuration_->userScanName();
	if (scanName == "") {
		scanName = configuration_->autoScanName();
	}
	scan_->setName(scanName);


	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet detectors;

	SXRMBBeamline::sxrmb()->brukerDetector()->setIsVisible(false);
	SXRMBBeamline::sxrmb()->brukerDetector()->setHiddenFromUsers(true);

	detectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("Bruker")->toInfo());
	detectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("I0Detector")->toInfo());
	detectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("TEYDetector")->toInfo());

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

void SXRMB2DScanActionController::buildScanControllerImplementation()
{
	AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

	detector->removeAllRegionsOfInterest();

	QList<AMDataSource *> i0Sources = QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("I0Detector"));

	AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

	foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

		AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
		AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
		newRegion->setBinningRange(regionAB->binningRange());
		newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
		scan_->addAnalyzedDataSource(newRegion, false, true);
		detector->addRegionOfInterest(region);

		AM2DNormalizationAB *normalizedRegion = new AM2DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
		normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
		normalizedRegion->setDataName(newRegion->name());
		normalizedRegion->setNormalizationName(i0Sources.first()->name());
		scan_->addAnalyzedDataSource(normalizedRegion, true, false);
	}
}

void SXRMB2DScanActionController::createAxisOrderMap()
{
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
}

AMAction3* SXRMB2DScanActionController::createInitializationActions()
{
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SXRMB 2D Map Initialization Actions", "SXRMB 2D Map Initialization Actions"));

	tmpControl = SXRMBBeamline::sxrmb()->microprobeSampleStageY();
	AMControlInfo normalSetpoint = tmpControl->toInfo();
	normalSetpoint.setValue(configuration_->normalPosition());
	moveActionInfo = new AMControlMoveActionInfo3(normalSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationActions->addSubAction(moveAction);

	CLSSIS3820Scaler *scaler = SXRMBBeamline::sxrmb()->scaler();
	initializationActions->addSubAction(scaler->createStartAction3(false));
	initializationActions->addSubAction(scaler->createContinuousEnableAction3(false));

	initializationActions->addSubAction(scaler->createScansPerBufferAction3(1));
	initializationActions->addSubAction(scaler->createTotalScansAction3(1));

	// Bruker actions?

	return initializationActions;
}

AMAction3* SXRMB2DScanActionController::createCleanupActions()
{
	AMDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();
	detector->setHiddenFromUsers(false);
	detector->setIsVisible(true);

	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("SXRMB 2D Map Cleanup Actions", "SXRMB 2D Map Cleanup Actions"), AMListAction3::Sequential);

	CLSSIS3820Scaler *scaler = SXRMBBeamline::sxrmb()->scaler();
	cleanupActions->addSubAction(scaler->createDwellTimeAction3(scaler->dwellTime()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));

	return cleanupActions;
}

void SXRMB2DScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
