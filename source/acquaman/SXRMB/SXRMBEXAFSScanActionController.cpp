#include "SXRMBEXAFSScanActionController.h"

#include "actions3/actions/AMControlMoveAction3.h"

#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "beamline/SXRMB/SXRMBBeamline.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "analysis/AM1DNormalizationAB.h"

SXRMBEXAFSScanActionController::SXRMBEXAFSScanActionController(SXRMBEXAFSScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(configuration);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString scanName = configuration_->userScanName();
	if (scanName == "") {
		scanName = configuration_->autoScanName();
	}
	scan_->setName(scanName);

	AMControlInfoList list;
	list.append(SXRMBBeamline::sxrmb()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet sxrmbDetectors;
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->i0Detector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->teyDetector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->energyFeedbackDetector()->toInfo());
	if (configuration_->enableBrukerDetector())
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->brukerDetector()->toInfo());
	configuration_->setDetectorConfigurations(sxrmbDetectors);

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

SXRMBEXAFSScanActionController::~SXRMBEXAFSScanActionController()
{
}

AMAction3* SXRMBEXAFSScanActionController::createInitializationActions()
{
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SXRMB EXAFS Initialization Actions", "SXRMB EXAFS Initialization Actions"), AMListAction3::Sequential);

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	tmpControl = SXRMBBeamline::sxrmb()->microprobeSampleStageY();
	AMControlInfo normalSetpoint = tmpControl->toInfo();
	normalSetpoint.setValue(configuration_->normalPosition());
	moveActionInfo = new AMControlMoveActionInfo3(normalSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationActions->addSubAction(moveAction);

	tmpControl = SXRMBBeamline::sxrmb()->microprobeSampleStageX();
	AMControlInfo stageXSetpoint = tmpControl->toInfo();
	stageXSetpoint.setValue(configuration_->microprobeSampleStageX());
	moveActionInfo = new AMControlMoveActionInfo3(stageXSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationActions->addSubAction(moveAction);

	tmpControl = SXRMBBeamline::sxrmb()->microprobeSampleStageZ();
	AMControlInfo stageZSetpoint = tmpControl->toInfo();
	stageZSetpoint.setValue(configuration_->microprobeSampleStageZ());
	moveActionInfo = new AMControlMoveActionInfo3(stageZSetpoint);
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

AMAction3* SXRMBEXAFSScanActionController::createCleanupActions()
{
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("SXRMB EXAFS Cleanup Actions", "SXRMB EXAFS Cleanup Actions"), AMListAction3::Sequential);

	CLSSIS3820Scaler *scaler = SXRMBBeamline::sxrmb()->scaler();
	cleanupActions->addSubAction(scaler->createDwellTimeAction3(scaler->dwellTime()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));

	return cleanupActions;
}

void SXRMBEXAFSScanActionController::buildScanControllerImplementation()
{
	if (configuration_->enableBrukerDetector()){
		AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

		detector->removeAllRegionsOfInterest();

		QList<AMDataSource *> i0Sources = QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("I0Detector"));

		AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

		QString edgeSymbol = configuration_->edge().split(" ").first();

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, false, true);
			detector->addRegionOfInterest(region);

			AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0Sources.first()->name());
			scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
		}
	}
}

void SXRMBEXAFSScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void SXRMBEXAFSScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
