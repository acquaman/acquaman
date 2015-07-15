#include "BioXASXASScanActionController.h"

#include "actions3/AMListAction3.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AM1DNormalizationAB.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMXASScan.h"

#include "util/AMErrorMonitor.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	// Create scan object.

	scan_ = new AMXASScan();
	scan_->setName(configuration_->name());
	scan_->setFileFormat("amCDFv1");
	scan_->setScanConfiguration(configuration_);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
	scan_->setNotes(beamlineSettings());

	// Set axis control infos.

	AMControlInfoList list;

	BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();
	if (mono) {
		AMControl *energyControl = BioXASBeamline::bioXAS()->mono()->energyControl();

		if (energyControl)
			list.append(energyControl->toInfo());
	}

	setConfigurationAxisControlInfos(list);

	// Set detectors.

	AMDetectorInfoSet bioXASDetectors;

	AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
	if (i0Detector)
		bioXASDetectors.addDetectorInfo(i0Detector->toInfo());

	AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();
	if (i1Detector)
		bioXASDetectors.addDetectorInfo(i1Detector->toInfo());

	AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();
	if (i2Detector)
		bioXASDetectors.addDetectorInfo(i2Detector->toInfo());

//	AMDetector *vortexDetector = BioXASBeamline::bioXAS()->fourElementVortexDetector();
//	if (vortexDetector)
//		bioXASDetectors.addDetectorInfo(vortexDetector->toInfo());

//	AMDetector *ge32Detector = BioXASBeamline::bioXAS()->ge32ElementDetector();
//	if (ge32Detector)
//		bioXASDetectors.addDetectorInfo(ge32Detector->toInfo());

	AMDetector *scalerDwellTimeDetector = BioXASBeamline::bioXAS()->scalerDwellTimeDetector();
	if (scalerDwellTimeDetector)
		bioXASDetectors.addDetectorInfo(scalerDwellTimeDetector->toInfo());

	setConfigurationDetectorConfigurations(bioXASDetectors);

	// Set up the scan timer.

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

void BioXASXASScanActionController::setConfigurationAxisControlInfos(const AMControlInfoList &infos)
{
	if (configuration_) {
		configuration_->setAxisControlInfos(infos);
	}
}

void BioXASXASScanActionController::setConfigurationDetectorConfigurations(const AMDetectorInfoSet &infos)
{
	if (configuration_) {
		configuration_->setDetectorConfigurations(infos);
	}
}

QString BioXASXASScanActionController::beamlineSettings()
{
	QString notes;

	// Note the storage ring current.

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

	AMSequentialListAction3 *scalerInitialization = 0;

	// The vortex is okay if the scan is using this detector and one is found. Also okay if we aren't using it. We have a problem otherwise.
	bool vortexOK = false;
	AMSequentialListAction3 *vortexDetectorInitialization = 0;

	AMSequentialListAction3 *monoInitialization = 0;

	// Initialize the scaler.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		double regionTime = double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

		scalerInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Initialization Actions", "BioXAS Scaler Initialization Actions"));
		scalerInitialization->addSubAction(scaler->createContinuousEnableAction3(false));
		scalerInitialization->addSubAction(scaler->createStartAction3(false));
		scalerInitialization->addSubAction(scaler->createDwellTimeAction3(double(regionTime)));
		scalerInitialization->addSubAction(scaler->createScansPerBufferAction3(1));
		scalerInitialization->addSubAction(scaler->createTotalScansAction3(1));
		scalerInitialization->addSubAction(scaler->createStartAction3(true));
		scalerInitialization->addSubAction(scaler->createWaitForDwellFinishedAction(double(regionTime) + 5.0));
	}

	// Initialize four element XRF detector, if using.

	BioXASFourElementVortexDetector *detector = BioXASBeamline::bioXAS()->fourElementVortexDetector();

	if (configuration_->usingXRFDetector() && detector) {
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

	// Initialize the mono.

	BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();

	if (mono) {

		// Set the bragg motor power to PowerOn, must be on to move/scan.
		CLSMAXvMotor *braggMotor = qobject_cast<CLSMAXvMotor*>(mono->braggMotor());

		if (braggMotor) {
			monoInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Monochromator Initialization", "BioXAS Monochromator Initialization"));
			monoInitialization->addSubAction(braggMotor->createPowerAction(CLSMAXvMotor::PowerOn));
		}
	}

	// Create complete initialization action.

	if (vortexOK) {
		result = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS XAS Scan Initialization Actions", "BioXAS Main Scan Initialization Actions"));

		// Add scaler initialization.
		if (scalerInitialization)
			result->addSubAction(scalerInitialization);

		// Add vortex initialization.
		if (vortexDetectorInitialization)
			result->addSubAction(vortexDetectorInitialization);

		// Add mono initialization.
		if (monoInitialization)
			result->addSubAction(monoInitialization);
	}

	return result;
}

AMAction3* BioXASXASScanActionController::createCleanupActions()
{
	AMSequentialListAction3 *result = 0;
	AMSequentialListAction3 *scalerCleanup = 0;
	AMSequentialListAction3 *monoCleanup = 0;

	// Create scaler cleanup actions.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		scalerCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Cleanup", "BioXAS Scaler Cleanup"));
		scalerCleanup->addSubAction(scaler->createDwellTimeAction3(1.0));
		scalerCleanup->addSubAction(scaler->createContinuousEnableAction3(true));
	}

	// Create mono cleanup actions.

	BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();

	if (mono) {

		// Set the bragg motor power to PowerAutoHardware. The motor can get too warm when left on for too long, that's why we turn it off when not in use.
		CLSMAXvMotor *braggMotor = qobject_cast<CLSMAXvMotor*>(mono->braggMotor());

		if (braggMotor) {
			monoCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Monochromator Cleanup", "BioXAS Monochromator Cleanup"));
			monoCleanup->addSubAction(braggMotor->createPowerAction(CLSMAXvMotor::PowerAutoHardware));
		}
	}

	// Create complete cleanup action.

	result = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS XAS Scan Cleanup Actions", "BioXAS XAS Scan Cleanup Actions"));

	if (scalerCleanup)
		result->addSubAction(scalerCleanup);

	if (monoCleanup)
		result->addSubAction(monoCleanup);

	return result;
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
