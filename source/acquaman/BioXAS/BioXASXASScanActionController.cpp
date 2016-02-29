#include "BioXASXASScanActionController.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMDetectorWaitForAcquisitionStateAction.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AM1DNormalizationAB.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASSIS3820Scaler.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "util/AMErrorMonitor.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent)
{
	bioXASConfiguration_ = configuration;

	useFeedback_ = true;

	scan_->setNotes(scanNotes());

	if (bioXASConfiguration_) {
		AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), true);

		if (bioXASDefaultXAS->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
	}
}

BioXASXASScanActionController::~BioXASXASScanActionController()
{

}

QString BioXASXASScanActionController::scanNotes()
{
	QString notes;

	// Note the storage ring current.

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(QString::number(CLSStorageRing::sr1()->ringCurrent(), 'f', 1)));

	// Note the mono settling time, if applicable.

	BioXASSSRLMonochromator *mono = BioXASBeamline::bioXAS()->mono();
	if (mono) {
		double settlingTime = mono->bragg()->settlingTime();
		if (settlingTime > 0)
			notes.append(QString("Settling time:\t%1 s\n").arg(settlingTime));
	}

	return notes;
}

bool BioXASXASScanActionController::usingScaler() const
{
	bool result = false;

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (scaler)
		result = ( usingI0Detector() || usingI1Detector() || usingI2Detector() );

	return result;
}

bool BioXASXASScanActionController::usingI0Detector() const
{
	bool result = false;

	AMDetector *i0 = BioXASBeamline::bioXAS()->i0Detector();

	if (i0)
		result = (bioXASConfiguration_->detectorConfigurations().indexOf(i0->name()) != -1);

	return result;
}

bool BioXASXASScanActionController::usingI1Detector() const
{
	bool result = false;

	AMDetector *i1 = BioXASBeamline::bioXAS()->i1Detector();

	if (i1)
		result = (bioXASConfiguration_->detectorConfigurations().indexOf(i1->name()) != -1);

	return result;
}

bool BioXASXASScanActionController::usingI2Detector() const
{
	bool result = false;

	AMDetector *i2 = BioXASBeamline::bioXAS()->i2Detector();

	if (i2)
		result = (bioXASConfiguration_->detectorConfigurations().indexOf(i2->name()) != -1);

	return result;
}

bool BioXASXASScanActionController::usingZebra() const
{
	bool result = false;

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra)
		result = true;

	return result;
}

bool BioXASXASScanActionController::usingGeDetector() const
{
	bool result = false;

	BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();

	if (geDetector)
		result = (bioXASConfiguration_->detectorConfigurations().indexOf(geDetector->name()) != -1);

	return result;
}

AMAction3* BioXASXASScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *result = 0;

	// Initialize the scaler.

	AMSequentialListAction3 *scalerInitialization = 0;
	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (scaler && usingScaler()) {

		scalerInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Initialization Actions", "BioXAS Scaler Initialization Actions"));

		// Check that the scaler is in single shot mode and is not acquiring.

		scalerInitialization->addSubAction(scaler->createContinuousEnableAction3(false));

		// Clear all detectors and managers. Add those used for this scan.

		scalerInitialization->addSubAction(scaler->createClearDetectorsAction());

		if (usingI0Detector())
			scalerInitialization->addSubAction(scaler->createAddDetectorAction(BioXASBeamline::bioXAS()->i0Detector()));

		if (usingI1Detector())
			scalerInitialization->addSubAction(scaler->createAddDetectorAction(BioXASBeamline::bioXAS()->i1Detector()));

		if (usingI2Detector())
			scalerInitialization->addSubAction(scaler->createAddDetectorAction(BioXASBeamline::bioXAS()->i2Detector()));

		scalerInitialization->addSubAction(scaler->createClearDetectorManagersAction());
	}

	// Initialize Ge 32-el detector, if using.

	AMSequentialListAction3 *geDetectorInitialization = 0;
	BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();

	if (geDetector && usingGeDetector()) {

		geDetectorInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Xpress3 Initialization", "BioXAS Xpress3 Initialization"));
		geDetectorInitialization->addSubAction(geDetector->createDisarmAction());
		geDetectorInitialization->addSubAction(geDetector->createFramesPerAcquisitionAction(int(bioXASConfiguration_->scanAxisAt(0)->numberOfPoints()*1.1)));	// Adding 10% just because.
		geDetectorInitialization->addSubAction(geDetector->createInitializationAction());

		AMDetectorWaitForAcquisitionStateAction *waitAction = new AMDetectorWaitForAcquisitionStateAction(new AMDetectorWaitForAcquisitionStateActionInfo(geDetector->toInfo(), AMDetector::ReadyForAcquisition), geDetector);
		geDetectorInitialization->addSubAction(waitAction);
	}

	// Initialize the zebra.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();
	AMSequentialListAction3 *zebraInitialization = 0;

	if (zebra && usingZebra()) {

		zebraInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Zebra Initialization", "BioXAS Zebra Initialization"));

		// Set up the Ge detector pulse control, if we are using the detector in the scan.

		BioXASZebraPulseControl *detectorPulse = zebra->pulseControlAt(2);

		if (detectorPulse) {
			if (usingGeDetector())
				zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(52));
			else
				zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(0));
		}

		// Clear all detectors and managers. Add those used for this scan.

		zebraInitialization->addSubAction(zebra->createClearDetectorsAction());

		if (usingGeDetector())
			zebraInitialization->addSubAction(zebra->createAddDetectorAction(geDetector));

		zebraInitialization->addSubAction(zebra->createClearDetectorManagersAction());

		if (usingScaler())
			zebraInitialization->addSubAction(zebra->createAddDetectorManagerAction(scaler));
	}

	// Initialize the mono.

	AMSequentialListAction3 *monoInitialization = 0;
	BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(BioXASBeamline::bioXAS()->mono());

	if (mono) {

		// If the mono is an SSRL mono, must set the bragg motor power to PowerOn to move/scan.

		CLSMAXvMotor *braggMotor = qobject_cast<CLSMAXvMotor*>(mono->bragg());

		if (braggMotor) {
			monoInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Monochromator Initialization", "BioXAS Monochromator Initialization"));
			monoInitialization->addSubAction(braggMotor->createPowerAction(CLSMAXvMotor::PowerOn));
		}
	}

	// Initialize the standards wheel.

	AMAction3* standardsWheelInitialization = 0;
	CLSStandardsWheel *standardsWheel = BioXASBeamline::bioXAS()->standardsWheel();

	if (standardsWheel) {
		if (standardsWheel->indexFromName(bioXASConfiguration_->edge().split(" ").first()) != -1) {
			standardsWheelInitialization = standardsWheel->createMoveToNameAction(bioXASConfiguration_->edge().split(" ").first());
		} else {
			standardsWheelInitialization = standardsWheel->createMoveToNameAction("None");
		}
	}

	// Create complete initialization action.

	result = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS XAS Scan Initialization Actions", "BioXAS Main Scan Initialization Actions"));

	// Add scaler initialization.
	if (scalerInitialization)
		result->addSubAction(scalerInitialization);

	// Add Ge 32-el detector initialization.
	if (geDetectorInitialization)
		result->addSubAction(geDetectorInitialization);

	// Add zebra initialization.
	if (zebraInitialization)
		result->addSubAction(zebraInitialization);

	// Add mono initialization.
	if (monoInitialization)
		result->addSubAction(monoInitialization);

	// Add standards wheel initialization.
	if (standardsWheelInitialization)
		result->addSubAction(standardsWheelInitialization);

	return result;
}

AMAction3* BioXASXASScanActionController::createCleanupActions()
{
	AMSequentialListAction3 *result = 0;

	// Create scaler cleanup actions.

	AMSequentialListAction3 *scalerCleanup = 0;
	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler && usingScaler()) {

		scalerCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Cleanup", "BioXAS Scaler Cleanup"));

		// Clear all detectors and managers. Add the defaults.

		scalerCleanup->addSubAction(scaler->createClearDetectorsAction());

		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
		if (i0Detector)
			scalerCleanup->addSubAction(scaler->createAddDetectorAction(i0Detector));

		AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();
		if (i1Detector)
			scalerCleanup->addSubAction(scaler->createAddDetectorAction(i1Detector));

		AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();
		if (i2Detector)
			scalerCleanup->addSubAction(scaler->createAddDetectorAction(i2Detector));

		scalerCleanup->addSubAction(scaler->createClearDetectorManagersAction());

		// Put the scaler in Continuous mode.

		scalerCleanup->addSubAction(scaler->createContinuousEnableAction3(true));
	}

	// Create zebra cleanup actions.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();
	AMSequentialListAction3 *zebraCleanup = 0;

	if (zebra && usingZebra()) {

		zebraCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Zebra Initialization", "BioXAS Zebra Initialization"));

		// Clear detectors and managers. Add defaults.

		zebraCleanup->addSubAction(zebra->createClearDetectorsAction());

		BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();
		if (geDetector)
			zebraCleanup->addSubAction(zebra->createAddDetectorAction(geDetector));

		zebraCleanup->addSubAction(zebra->createClearDetectorManagersAction());

		CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();
		if (scaler)
			zebraCleanup->addSubAction(zebra->createAddDetectorManagerAction(scaler));

		// Enable default settings for Ge detector.

		BioXASZebraPulseControl *detectorPulse = zebra->pulseControlAt(2);

		if (detectorPulse)
			zebraCleanup->addSubAction(detectorPulse->createSetInputValueAction(52));
	}

	// Create mono cleanup actions.

	AMSequentialListAction3 *monoCleanup = 0;
	BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(BioXASBeamline::bioXAS()->mono());

	if (mono) {

		// Set the bragg motor power to PowerAutoSoftware. The motor can get too warm when left on for too long, that's why we turn it off when not in use.
		CLSMAXvMotor *braggMotor = qobject_cast<CLSMAXvMotor*>(mono->bragg());

		if (braggMotor) {
			monoCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Monochromator Cleanup", "BioXAS Monochromator Cleanup"));
			monoCleanup->addSubAction(braggMotor->createPowerAction(CLSMAXvMotor::PowerAutoSoftware));
		}
	}

	// Create complete cleanup action.

	result = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS XAS Scan Cleanup Actions", "BioXAS XAS Scan Cleanup Actions"));

	if (scalerCleanup)
		result->addSubAction(scalerCleanup);

	if (monoCleanup)
		result->addSubAction(monoCleanup);

	if (zebraCleanup)
		result->addSubAction(zebraCleanup);

	return result;
}

void BioXASXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void BioXASXASScanActionController::buildScanControllerImplementation()
{
	// Identify the zebra trigger source.

	//BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();
//	AMZebraDetectorTriggerSource *zebraTriggerSource = 0;

//	if (zebra) {

//		zebraTriggerSource = zebra->triggerSource();

//		if (zebraTriggerSource) {
//			zebraTriggerSource->removeAllDetectors();
//			zebraTriggerSource->removeAllDetectorManagers();
//		}
//	}

//	if (zebra) {
//		zebra->clearDetectors();
//		zebra->clearDetectorManagers();
//	}

	// Identify the scaler.

	//CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	// Identify data sources for the scaler channels.

	AMDataSource *i0DetectorSource = 0;
	AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();

	if (i0Detector) {

		int i0DetectorIndex = scan_->indexOfDataSource(i0Detector->name());

		if (i0DetectorIndex != -1) {

//			if (zebraTriggerSource)
//				zebraTriggerSource->addDetector(i0Detector);

//			if (zebra)
//				zebra->addDetector(i0Detector);

			i0DetectorSource = scan_->dataSourceAt(i0DetectorIndex);
		}
	}

	AMDataSource *i1DetectorSource = 0;
	AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();

	if (i1Detector) {

		int i1DetectorIndex = scan_->indexOfDataSource(i1Detector->name());

		if (i1DetectorIndex != -1) {

//			if (zebraTriggerSource)
//				zebraTriggerSource->addDetector(i1Detector);

//			if (zebra)
//				zebra->addDetector(i1Detector);

			i1DetectorSource = scan_->dataSourceAt(i1DetectorIndex);
		}
	}

	AMDataSource *i2DetectorSource = 0;
	AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();

	if (i2Detector) {

		int i2DetectorIndex = scan_->indexOfDataSource(i2Detector->name());

		if (i2DetectorIndex != -1) {

//			if (zebraTriggerSource)
//				zebraTriggerSource->addDetector(i2Detector);

//			if (zebra)
//				zebra->addDetector(i2Detector);

			i2DetectorSource = scan_->dataSourceAt(i2DetectorIndex);
		}
	}

	if (scan_->indexOfDataSource(i0Detector->name()) != -1
			|| scan_->indexOfDataSource(i1Detector->name()) != -1
			|| scan_->indexOfDataSource(i2Detector->name()) != -1){

//		if (scaler && zebraTriggerSource)
//			zebraTriggerSource->addDetectorManager(scaler);

//		if (scaler && zebra)
//			zebra->addDetectorManager(scaler);
	}

	// Create analyzed data source for the absorbance.

	AM1DExpressionAB *absorbanceSource = 0;

	if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
		absorbanceSource = new AM1DExpressionAB("Absorbance");
		absorbanceSource->setDescription("Absorbance");
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
		absorbanceCorrectedSource->setDescription("Absorbance Dark Current Corrected");
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

	// Create analyzed data source for the Ge 32-el detector.

	AMXRFDetector *ge32Detector = BioXASBeamline::bioXAS()->ge32ElementDetector();

	if (ge32Detector) {

		int ge32DetectorIndex = scan_->indexOfDataSource(ge32Detector->name());

		if (ge32DetectorIndex != -1) {

//			if (zebraTriggerSource) {
//				zebraTriggerSource->addDetector(ge32Detector);
//				zebraTriggerSource->addDetectorManager(ge32Detector);
//			}

//			if (zebra) {
//				zebra->addDetector(ge32Detector);
//				zebra->addDetectorManager(ge32Detector);
//			}

			// Clear any previous regions.

			ge32Detector->removeAllRegionsOfInterest();

			// Iterate through each region in the configuration.
			// Create analysis block for each region, add ge32Detector spectra source as input source for each.
			// Add analysis block to the scan and to the ge32Detector.
			// Create normalized analysis block for each region, add to scan.

			AMDataSource *spectraSource = scan_->dataSourceAt(ge32DetectorIndex);
			QString edgeSymbol = bioXASConfiguration_->edge().split(" ").first();
			bool canNormalize = (i0DetectorSource || i0CorrectedDetectorSource);

			foreach (AMRegionOfInterest *region, bioXASConfiguration_->regionsOfInterest()){

				AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();

				AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
				newRegion->setBinningRange(regionAB->binningRange());
				newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);

				scan_->addAnalyzedDataSource(newRegion, false, true);
				ge32Detector->addRegionOfInterest(region);

				if (canNormalize) {
					AMDataSource *normalizationSource = (i0CorrectedDetectorSource != 0) ? i0CorrectedDetectorSource : i0DetectorSource;

					AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
					normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << normalizationSource);
					normalizedRegion->setDataName(newRegion->name());
					normalizedRegion->setNormalizationName(normalizationSource->name());

					scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
				}
			}
		}
	}
}
