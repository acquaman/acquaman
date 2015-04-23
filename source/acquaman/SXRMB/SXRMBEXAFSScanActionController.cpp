#include "SXRMBEXAFSScanActionController.h"

#include "actions3/AMActionSupport.h"

#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "beamline/SXRMB/SXRMBBeamline.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM2DAdditionAB.h"
#include "analysis/AM1DExpressionAB.h"

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
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->energyFeedbackDetector()->toInfo());

	switch (configuration_->endstation()){

	case SXRMB::SolidState:

		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("BeamlineI0Detector")->toInfo());
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("TEYDetector")->toInfo());

		break;

	case SXRMB::AmbiantWithGasChamber:
	case SXRMB::AmbiantWithoutGasChamber:

		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("BeamlineI0Detector")->toInfo());
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("I0Detector")->toInfo());
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("TransmissionDetector")->toInfo());

		break;

	case SXRMB::Microprobe:

		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("I0Detector")->toInfo());
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("TEYDetector")->toInfo());

		break;

	default:
		break;
	}

	if (configuration_->fluorescenceDetectors().testFlag(SXRMB::Bruker))
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("Bruker")->toInfo());

	if (configuration_->fluorescenceDetectors().testFlag(SXRMB::FourElement))
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("FourElementVortex")->toInfo());

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

	switch (configuration_->endstation()){

	case SXRMB::SolidState:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->solidStateSampleStageY(), configuration_->y()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->solidStateSampleStageX(), configuration_->x()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->solidStateSampleStageZ(), configuration_->z()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->solidStateSampleStageR(), configuration_->rotation()));

		if (configuration_->powerOnTEYHVControl())
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->teyHVControl()->powerOnOffControl(), 1));
		break;

	case SXRMB::AmbiantWithGasChamber:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->ambiantSampleHolderZ(), configuration_->z()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->ambiantSampleHolderR(), configuration_->rotation()));

		break;

	case SXRMB::AmbiantWithoutGasChamber:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->ambiantSampleStageX(), configuration_->x()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->ambiantSampleStageZ(), configuration_->z()));

		break;

	case SXRMB::Microprobe:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->microprobeSampleStageY(), configuration_->y()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->microprobeSampleStageX(), configuration_->x()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->microprobeSampleStageZ(), configuration_->z()));

		if (configuration_->powerOnTEYHVControl())
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(SXRMBBeamline::sxrmb()->microprobeTEYHVControl()->powerOnOffControl(), 1));
		break;

	default:
		break;
	}

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
	QList<AMDataSource *> i0Sources;
	if (configuration_->endstation() == SXRMB::SolidState)
		i0Sources = QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("BeamlineI0Detector"));

	else
		i0Sources = QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("I0Detector"));

	if (configuration_->fluorescenceDetectors().testFlag(SXRMB::Bruker)){

		AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

		detector->removeAllRegionsOfInterest();

		AMDataSource *spectraSource = 0;

		SXRMB::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetectors();

		if (xrfDetector.testFlag(SXRMB::Bruker) && xrfDetector.testFlag(SXRMB::FourElement)){

			AM2DAdditionAB *sumSpectra = new AM2DAdditionAB("BrukerAndFourSpectra");
			sumSpectra->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("Bruker")) << scan_->dataSourceAt(scan_->indexOfDataSource("FourElementVortex")));
			scan_->addAnalyzedDataSource(sumSpectra, false, true);
			spectraSource = sumSpectra;
		}

		else
			spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));


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

	if (configuration_->endstation() == SXRMB::AmbiantWithGasChamber || configuration_->endstation() == SXRMB::AmbiantWithoutGasChamber){

		AM1DExpressionAB* transmission = new AM1DExpressionAB("transmission");
		transmission->setDescription("Transmission");
		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("TransmissionDetector")) << i0Sources);
		transmission->setExpression(QString("ln(I0Detector/TransmissionDetector)"));
		scan_->addAnalyzedDataSource(transmission, true, false);
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
