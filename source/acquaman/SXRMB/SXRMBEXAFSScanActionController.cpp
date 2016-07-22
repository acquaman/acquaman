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
	scan_->setFileFormat("amCDFv1");
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

	if (configuration_->fluorescenceDetector().testFlag(SXRMB::BrukerDetector))
		sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("Bruker")->toInfo());

	if (configuration_->fluorescenceDetector().testFlag(SXRMB::FourElementDetector))
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

AMAction3* SXRMBEXAFSScanActionController::createInitializationActions()
{
	SXRMBBeamline *sxrmbBl = SXRMBBeamline::sxrmb();

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SXRMB EXAFS Initialization Actions", "SXRMB EXAFS Initialization Actions"), AMListAction3::Sequential);

	switch (configuration_->endstation()){

	case SXRMB::SolidState:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->solidStateSampleStageY(), configuration_->y()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->solidStateSampleStageX(), configuration_->x()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->solidStateSampleStageZ(), configuration_->z()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->solidStateSampleStageR(), configuration_->rotation()));

		if (configuration_->powerOnHVControl())
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->teyHVControl()->powerOnOffControl(), 1));
		break;

	case SXRMB::AmbiantWithGasChamber:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantSampleHolderZ(), configuration_->z()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantSampleHolderR(), configuration_->rotation()));
		if (configuration_->powerOnHVControl()) {
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantIC0HVControl()->powerOnOffControl(), 1));
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantIC1HVControl()->powerOnOffControl(), 1));
		}

		break;

	case SXRMB::AmbiantWithoutGasChamber:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantSampleStageX(), configuration_->x()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantSampleStageZ(), configuration_->z()));
		if (configuration_->powerOnHVControl()) {
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantIC0HVControl()->powerOnOffControl(), 1));
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->ambiantIC1HVControl()->powerOnOffControl(), 1));
		}

		break;

	case SXRMB::Microprobe:

		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->microprobeSampleStageY(), configuration_->y()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->microprobeSampleStageX(), configuration_->x()));
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->microprobeSampleStageZ(), configuration_->z()));

		if (configuration_->powerOnHVControl())
			initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->microprobeTEYHVControl()->powerOnOffControl(), 1));
		break;

	default:
		break;
	}

	CLSSIS3820Scaler *scaler = sxrmbBl->scaler();
	initializationActions->addSubAction(scaler->createStartAction3(false));
	initializationActions->addSubAction(scaler->createContinuousEnableAction3(false));

	initializationActions->addSubAction(scaler->createScansPerBufferAction3(1));
	initializationActions->addSubAction(scaler->createTotalScansAction3(1));

	// Bruker actions?

	// move energy
	initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(sxrmbBl->energy(), configuration_->energy()));

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

	// setup the analysis block for XRF detectors
	buildXRFAnalysisBlock(i0Sources);

	// setup the analysis block for the transmission detector when it ambiant endstation
	buildTransmissionAnalysisBlock(i0Sources);

	// setup the analysis block for the TEY detector when it solid state endstation
	buildTEYAnalysisBlock(i0Sources);
}

void SXRMBEXAFSScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void SXRMBEXAFSScanActionController::buildXRFAnalysisBlock(const QList<AMDataSource *> &i0Sources)
{
	SXRMB::FluorescenceDetectors xrfDetectorConfig = configuration_->fluorescenceDetector();

	AMXRFDetector *xrfDetector = SXRMBBeamline::sxrmb()->xrfDetector(xrfDetectorConfig);
	if (xrfDetector) {
		xrfDetector->removeAllRegionsOfInterest();

		AMDataSource *spectraSource = 0;

		if (xrfDetectorConfig.testFlag(SXRMB::BrukerDetector) && xrfDetectorConfig.testFlag(SXRMB::FourElementDetector)){

			AM2DAdditionAB *sumSpectra = new AM2DAdditionAB("OneAndFourSpectra");
			sumSpectra->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("Bruker")) << scan_->dataSourceAt(scan_->indexOfDataSource("FourElementVortex")));
			scan_->addAnalyzedDataSource(sumSpectra, false, true);

			spectraSource = sumSpectra;
		} else {

			spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(xrfDetector->name()));
		}

		if (spectraSource) {
			QString edgeSymbol = configuration_->edge().split(" ").first();

			foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){
				xrfDetector->addRegionOfInterest(region);

				AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
				AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
				newRegion->setBinningRange(regionAB->binningRange());
				newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
				scan_->addAnalyzedDataSource(newRegion, false, true);

				AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
				normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
				normalizedRegion->setDataName(newRegion->name());
				normalizedRegion->setNormalizationName(i0Sources.first()->name());
				scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
			}
		}
	}
}

void SXRMBEXAFSScanActionController::buildTransmissionAnalysisBlock(const QList<AMDataSource *> &i0Sources)
{
	if (configuration_->endstation() == SXRMB::AmbiantWithGasChamber || configuration_->endstation() == SXRMB::AmbiantWithoutGasChamber) {
		AMDataSource *transmissionSource = scan_->dataSourceAt(scan_->indexOfDataSource("TransmissionDetector"));

		AM1DExpressionAB* transmission = new AM1DExpressionAB("transmission");
		transmission->setDescription("Transmission");
		transmission->setInputDataSources(QList<AMDataSource *>() << transmissionSource << i0Sources);
		transmission->setExpression(QString("ln(I0Detector/TransmissionDetector)"));
		scan_->addAnalyzedDataSource(transmission, true, false);
	}
}

void SXRMBEXAFSScanActionController::buildTEYAnalysisBlock(const QList<AMDataSource *> &i0Sources)
{
	if (configuration_->endstation() == SXRMB::SolidState) {
		AMDetector* teyDetector = SXRMBBeamline::sxrmb()->teyDetector();
		AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource("TEYDetector"));

		AM1DNormalizationAB *normalizedTEY = new AM1DNormalizationAB(QString("norm_%1").arg(teyDetector->name()));
		normalizedTEY->setDataName(teyDetector->name());
		normalizedTEY->setNormalizationName(i0Sources.first()->name());
		normalizedTEY->setInputDataSources(QList<AMDataSource *>() << spectraSource << i0Sources);

		scan_->addAnalyzedDataSource(normalizedTEY, true, false);
	}

}

