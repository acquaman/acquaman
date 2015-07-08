#include "BioXASXASScanActionController.h"

#include "actions3/AMListAction3.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AM1DNormalizationAB.h"

#include "application/AMAppControllerSupport.h"
#include "application/BioXAS/BioXAS.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMXASScan.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#include "util/AMErrorMonitor.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", configuration_->edge().split(" ").first(), configuration_->edge().split(" ").last(), true);
	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());

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

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(QString::number(CLSStorageRing::sr1()->ringCurrent(), 'f', 1)));

	// Note which energy control is being used.

	QString controlType;
	if (configuration_->usingEncoderEnergy())
		controlType = QString("encoder-based");
	else
		controlType = QString("step-based");

	notes.append(QString("Energy option:\t%1\n").arg(controlType));

	return notes;
}

AMAction3* BioXASXASScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *result = 0;

	bool scalerOK = false;
	AMSequentialListAction3 *scalerInitialization = 0;

	bool vortexOK = false;
	AMSequentialListAction3 *vortexDetectorInitialization = 0;

	// Initialize the scaler.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		scalerOK = true;

		double regionTime = double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

		scalerInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Initialization Actions", "BioXAS Scaler Initialization Actions"));
		AMListAction3 *scalerInitializationStep;

		scalerInitializationStep = new AMListAction3(new AMListActionInfo3("Scaler Initialization Stage 1", "Scaler Initialization Stage 1"), AMListAction3::Parallel);
		scalerInitializationStep->addSubAction(scaler->createContinuousEnableAction3(false));
		scalerInitialization->addSubAction(scalerInitializationStep);

		scalerInitializationStep = new AMListAction3(new AMListActionInfo3("Scaler Initialization Stage 2", "Scaler Initialization Stage 2"), AMListAction3::Parallel);
		scalerInitializationStep->addSubAction(scaler->createStartAction3(false));
		scalerInitializationStep->addSubAction(scaler->createScansPerBufferAction3(1));
		scalerInitializationStep->addSubAction(scaler->createTotalScansAction3(1));
		scalerInitialization->addSubAction(scalerInitializationStep);

		scalerInitializationStep = new AMListAction3(new AMListActionInfo3("Scaler Initialization Stage 3", "Scaler Initialization Stage 3"), AMListAction3::Parallel);
		scalerInitializationStep->addSubAction(scaler->createStartAction3(true));
		scalerInitializationStep->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));
		scalerInitialization->addSubAction(scalerInitializationStep);

		scalerInitializationStep = new AMListAction3(new AMListActionInfo3("Scaler Initialization Stage 4", "Scaler Initialization Stage 4"));
		scalerInitializationStep->addSubAction(scaler->createDwellTimeAction3(double(regionTime)));
		scalerInitialization->addSubAction(scalerInitializationStep);

		scalerInitializationStep = new AMListAction3(new AMListActionInfo3("Scaler Initialization Stage 5", "Scaler Initialization Stage 5"), AMListAction3::Parallel);
		scalerInitializationStep->addSubAction(scaler->createStartAction3(true));
		scalerInitializationStep->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));
		scalerInitialization->addSubAction(scalerInitializationStep);

	} else {
		AMErrorMon::alert(this, BIOXASXASSCANACTIONCONTROLLER_SCALER_NOT_FOUND, "Failed to complete scan initialization--valid scaler not found.");
	}

	// Initialize four element XRF detector, if using.

	BioXASFourElementVortexDetector *detector = BioXASBeamline::bioXAS()->fourElementVortexDetector();

	if (configuration_->usingXRFDetector() && detector){
		vortexOK = true;

		vortexDetectorInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Xpress3 Initialization", "BioXAS Xpress3 Initialization"));
		vortexDetectorInitialization->addSubAction(detector->createDisarmAction());
		vortexDetectorInitialization->addSubAction(detector->createFramesPerAcquisitionAction(int(configuration_->scanAxisAt(0)->numberOfPoints()*1.1)));	// Adding 10% just because.
		vortexDetectorInitialization->addSubAction(detector->createInitializationAction());

	} else if (!configuration_->usingXRFDetector()) {
		vortexOK = true;

	} else {
		AMErrorMon::alert(this, BIOXASXASSCANACTIONCONTROLLER_VORTEX_DETECTOR_NOT_FOUND, "Failed to complete scan initialization--scan to use Vortex detector, but detector not found.");
	}

	// Create complete initialization action.

	if (scalerOK && vortexOK) {
		result = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS XAS Scan Initialization Actions", "BioXAS Main Scan Initialization Actions"));

		// Add scaler initialization.
		result->addSubAction(scalerInitialization);

		// Add vortex initialization.
		if (vortexDetectorInitialization)
			result->addSubAction(vortexDetectorInitialization);
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
	// Identify data sources for the scaler channels.

	AMDataSource *i0DetectorSource = 0;
	AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();

	if (i0Detector) {
		int i0DetectorIndex = scan_->indexOfDataSource(i0Detector->name());
		if (i0DetectorIndex != -1) {
			i0DetectorSource = scan_->dataSourceAt(i0DetectorIndex);
		}
	}

	AMDataSource *i1DetectorSource = 0;
	AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();

	if (i1Detector) {
		int i1DetectorIndex = scan_->indexOfDataSource(i1Detector->name());
		if (i1DetectorIndex != -1) {
			i1DetectorSource = scan_->dataSourceAt(i1DetectorIndex);
		}
	}

	AMDataSource *i2DetectorSource = 0;
	AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();

	if (i2Detector) {
		int i2DetectorIndex = scan_->indexOfDataSource(i2Detector->name());
		if (i2DetectorIndex != -1) {
			i2DetectorSource = scan_->dataSourceAt(i2DetectorIndex);
		}
	}

	// Create analyzed data source for the absorbance.

	AM1DExpressionAB *absorbanceSource = 0;

	if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
		absorbanceSource = new AM1DExpressionAB("Absorbance");
		absorbanceSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << i1DetectorSource << i2DetectorSource);
		absorbanceSource->setExpression(QString("ln(%1/%2)").arg(i1DetectorSource->name(), i2DetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance.

	AM1DDerivativeAB *derivAbsorbanceSource = 0;

	if (absorbanceSource) {
		derivAbsorbanceSource = new AM1DDerivativeAB("DerivAbsorbance");
		derivAbsorbanceSource->setInputDataSources(QList<AMDataSource*>() << absorbanceSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceSource, true, false);
	}

	// Create analyzed data sources for the dark current corrected scaler channel detectors.

	AMDataSource *dwellTimeSource = 0;
	AM1DDarkCurrentCorrectionAB *i0CorrectedDetectorSource = 0;
	AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;
	AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

	AMDetector *scalerDwellTimeDetector = BioXASBeamline::bioXAS()->scalerDwellTimeDetector();

	if (scalerDwellTimeDetector) {
		int dwellTimeIndex = scan_->indexOfDataSource(scalerDwellTimeDetector->name());
		if (dwellTimeIndex != -1) {
			dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);
		}

		if (dwellTimeSource && i0DetectorSource) {
			i0CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i0DetectorSource->name()));
			i0CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i0DetectorSource->name()));
			i0CorrectedDetectorSource->setDataName(i0DetectorSource->name());
			i0CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
			i0CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i0DetectorSource->name())->darkCurrentValue());
			i0CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << dwellTimeSource);
			i0CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

			connect( i0Detector, SIGNAL(darkCurrentValueChanged(double)), i0CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

			scan_->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);
		}

		if (dwellTimeSource && i1DetectorSource) {
			i1CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i1DetectorSource->name()));
			i1CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i1DetectorSource->name()));
			i1CorrectedDetectorSource->setDataName(i1DetectorSource->name());
			i1CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
			i1CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i1DetectorSource->name())->darkCurrentValue());
			i1CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i1DetectorSource << dwellTimeSource);
			i1CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

			connect( i1Detector, SIGNAL(darkCurrentValueChanged(double)), i1CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

			scan_->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);
		}

		if (dwellTimeSource && i2DetectorSource) {
			i2CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i2DetectorSource->name()));
			i2CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i2DetectorSource->name()));
			i2CorrectedDetectorSource->setDataName(i2DetectorSource->name());
			i2CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
			i2CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i2DetectorSource->name())->darkCurrentValue());
			i2CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i2DetectorSource << dwellTimeSource);
			i2CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

			connect( i2Detector, SIGNAL(darkCurrentValueChanged(double)), i2CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

			scan_->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);
		}
	}


	// Create analyzed data source for the absorbance, dark current corrected values.

	AM1DExpressionAB *absorbanceCorrectedSource = 0;

	if (i0CorrectedDetectorSource && i1CorrectedDetectorSource && i2CorrectedDetectorSource) {
		absorbanceCorrectedSource = new AM1DExpressionAB("Absorbance_DarkCorrect");
		absorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << i0CorrectedDetectorSource << i1CorrectedDetectorSource << i2CorrectedDetectorSource);
		absorbanceCorrectedSource->setExpression(QString("ln(%1/%2)").arg(i1CorrectedDetectorSource->name(), i2CorrectedDetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceCorrectedSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance, dark current corrected values.

	AM1DDerivativeAB *derivAbsorbanceCorrectedSource = 0;

	if (absorbanceCorrectedSource) {
		derivAbsorbanceCorrectedSource = new AM1DDerivativeAB("DerivAbsorbance_DarkCorrect");
		derivAbsorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << absorbanceCorrectedSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceCorrectedSource, true, false);
	}

	// Create analyzed data source for the four element Vortex detector.

	AMXRFDetector *vortexDetector = BioXASBeamline::bioXAS()->fourElementVortexDetector();
	if (configuration_->usingXRFDetector() && vortexDetector){

		vortexDetector->removeAllRegionsOfInterest();

		AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(vortexDetector->name()));
		QString edgeSymbol = configuration_->edge().split(" ").first();

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, false, true);
			vortexDetector->addRegionOfInterest(region);

			AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0CorrectedDetectorSource);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0CorrectedDetectorSource->name());
			scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
		}
	}
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
