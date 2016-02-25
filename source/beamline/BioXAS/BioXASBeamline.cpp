#include "BioXASBeamline.h"

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMDetectorWaitForAcquisitionStateAction.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AM1DNormalizationAB.h"

#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMScan.h"

#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool connected = (
				beamStatus_ && beamStatus_->isConnected() &&
				utilities_ && utilities_->isConnected()
				);

	return connected;
}

AMAction3* BioXASBeamline::createScanInitializationAction(AMGenericStepScanConfiguration *configuration)
{
	AMAction3 *result = 0;

	if (configuration) {

		AMListAction3 *initializationAction = new AMListAction3(new AMListActionInfo3("BioXAS scan initialization", "BioXAS scan intialization"), AMListAction3::Parallel);

		// Initialize the scaler.

		AMListAction3 *scalerInitialization = 0;
		CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

		if (scaler) {
			scalerInitialization = new AMListAction3(new AMListActionInfo3("BioXAS scaler initialization", "BioXAS scaler initialization"));
			scalerInitialization->addSubAction(scaler->createContinuousEnableAction3(false)); // Check that the scaler is in single shot mode and is not acquiring.
		}

		if (scalerInitialization)
			initializationAction->addSubAction(scalerInitialization);

		// Initialize Ge 32-el detector, if using.

		bool usingGeDetector = false;
		AMListAction3 *geDetectorInitialization = 0;
		BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();

		if (geDetector) {
			usingGeDetector = (configuration->detectorConfigurations().indexOf(geDetector->name()) != -1);

			if (usingGeDetector) {

				geDetectorInitialization = new AMListAction3(new AMListActionInfo3("BioXAS Xpress3 initialization", "BioXAS Xpress3 initialization"));
				geDetectorInitialization->addSubAction(geDetector->createDisarmAction());
				geDetectorInitialization->addSubAction(geDetector->createFramesPerAcquisitionAction(int(configuration->scanAxisAt(0)->numberOfPoints()*1.1)));	// Adding 10% just because.
				geDetectorInitialization->addSubAction(geDetector->createInitializationAction());

				AMDetectorWaitForAcquisitionStateAction *waitAction = new AMDetectorWaitForAcquisitionStateAction(new AMDetectorWaitForAcquisitionStateActionInfo(geDetector->toInfo(), AMDetector::ReadyForAcquisition), geDetector);
				geDetectorInitialization->addSubAction(waitAction);
			}
		}

		if (geDetectorInitialization)
			initializationAction->addSubAction(geDetectorInitialization);

		// Initialize the zebra.

		AMListAction3 *zebraInitialization = 0;
		BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

		if (zebra) {
			zebraInitialization = new AMListAction3(new AMListActionInfo3("BioXAS Zebra initialization", "BioXAS Zebra initialization"));

			BioXASZebraPulseControl *detectorPulse = zebra->pulseControlAt(2);

			if (detectorPulse) {
				if (usingGeDetector)
					zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(52));
				else
					zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(0));
			}
		}

		if (zebraInitialization)
			initializationAction->addSubAction(zebraInitialization);

		// Initialize the mono.

		AMListAction3 *monoInitialization = 0;
		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(BioXASBeamline::bioXAS()->mono());

		if (mono) {

			// If the mono is an SSRL mono, must set the bragg motor power to PowerOn to move/scan.

			CLSMAXvMotor *braggMotor = qobject_cast<CLSMAXvMotor*>(mono->bragg());

			if (braggMotor) {
				monoInitialization = new AMListAction3(new AMListActionInfo3("BioXAS SSRL monochromator initialization", "BioXAS SSRL monochromator initialization"));
				monoInitialization->addSubAction(braggMotor->createPowerAction(CLSMAXvMotor::PowerOn));
			}
		}

		if (monoInitialization)
			initializationAction->addSubAction(monoInitialization);

		// Initialize the standards wheel.

		AMAction3* standardsWheelInitialization = 0;
		CLSStandardsWheel *standardsWheel = BioXASBeamline::bioXAS()->standardsWheel();
		BioXASXASScanConfiguration *bioxasConfiguration = qobject_cast<BioXASXASScanConfiguration*>(configuration);

		if (standardsWheel && bioxasConfiguration) {
			if (standardsWheel->indexFromName(bioxasConfiguration->edge().split(" ").first()) != -1)
				standardsWheelInitialization = standardsWheel->createMoveToNameAction(bioxasConfiguration->edge().split(" ").first());
			else
				standardsWheelInitialization = standardsWheel->createMoveToNameAction("None");
		}

		if (standardsWheelInitialization)
			initializationAction->addSubAction(standardsWheelInitialization);

		// Complete action.

		result = initializationAction;
	}

	return result;
}

AMAction3* BioXASBeamline::createScanCleanupAction(AMGenericStepScanConfiguration *configuration)
{
	Q_UNUSED(configuration)

	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("BioXAS scan cleanup actions", "BioXAS scan cleanup actions"), AMListAction3::Parallel);

	// Create scaler cleanup actions.

	AMListAction3 *scalerCleanup = 0;
	CLSSIS3820Scaler *scaler = BioXASBeamline::clsBeamline()->scaler();

	if (scaler) {
		scalerCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler cleanup", "BioXAS Scaler cleanup"));
		scalerCleanup->addSubAction(scaler->createContinuousEnableAction3(true)); // Put the scaler in Continuous mode.
	}

	if (scalerCleanup)
		result->addSubAction(scalerCleanup);

	// Create zebra cleanup actions.

	AMListAction3 *zebraCleanup = 0;
	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra) {
		zebraCleanup = new AMListAction3(new AMListActionInfo3("BioXAS Zebra cleanup", "BioXAS Zebra cleanup"));

		BioXASZebraPulseControl *detectorPulse = zebra->pulseControlAt(2);

		if (detectorPulse)
			zebraCleanup->addSubAction(detectorPulse->createSetInputValueAction(52));
	}

	if (zebraCleanup)
		result->addSubAction(zebraCleanup);

	// Create mono cleanup actions.

	AMListAction3 *monoCleanup = 0;
	BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(BioXASBeamline::bioXAS()->mono());

	if (mono) {

		// Set the bragg motor power to PowerAutoSoftware. The motor can get too warm when left on for too long, that's why we turn it off when not in use.
		CLSMAXvMotor *braggMotor = qobject_cast<CLSMAXvMotor*>(mono->bragg());

		if (braggMotor) {
			monoCleanup = new AMListAction3(new AMListActionInfo3("BioXAS SSRL monochromator cleanup", "BioXAS SSRL monochromator cleanup"));
			monoCleanup->addSubAction(braggMotor->createPowerAction(CLSMAXvMotor::PowerAutoSoftware));
		}
	}

	if (monoCleanup)
		result->addSubAction(monoCleanup);

	return result;
}

void BioXASBeamline::buildScan(AMGenericStepScanConfiguration *configuration, AMScan *scan)
{
	if (configuration && scan) {

		// Identify the zebra trigger source.

		AMZebraDetectorTriggerSource *zebraTriggerSource = BioXASBeamline::bioXAS()->zebraTriggerSource();

		if (zebraTriggerSource) {
			zebraTriggerSource->removeAllDetectors();
			zebraTriggerSource->removeAllDetectorManagers();
		}

		// Identify the scaler.

		CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

		// Identify data sources for the scaler channels.

		AMDataSource *i0DetectorSource = 0;
		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();

		if (i0Detector) {

			int i0DetectorIndex = scan->indexOfDataSource(i0Detector->name());

			if (i0DetectorIndex != -1) {

				if (zebraTriggerSource)
					zebraTriggerSource->addDetector(i0Detector);

				i0DetectorSource = scan->dataSourceAt(i0DetectorIndex);
			}
		}

		AMDataSource *i1DetectorSource = 0;
		AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();

		if (i1Detector) {

			int i1DetectorIndex = scan->indexOfDataSource(i1Detector->name());

			if (i1DetectorIndex != -1) {

				if (zebraTriggerSource)
					zebraTriggerSource->addDetector(i1Detector);

				i1DetectorSource = scan->dataSourceAt(i1DetectorIndex);
			}
		}

		AMDataSource *i2DetectorSource = 0;
		AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();

		if (i2Detector) {

			int i2DetectorIndex = scan->indexOfDataSource(i2Detector->name());

			if (i2DetectorIndex != -1) {

				if (zebraTriggerSource)
					zebraTriggerSource->addDetector(i2Detector);

				i2DetectorSource = scan->dataSourceAt(i2DetectorIndex);
			}
		}

		if (scan->indexOfDataSource(i0Detector->name()) != -1
				|| scan->indexOfDataSource(i1Detector->name()) != -1
				|| scan->indexOfDataSource(i2Detector->name()) != -1){

			if (scaler && zebraTriggerSource)
				zebraTriggerSource->addDetectorManager(scaler);
		}

		// Create analyzed data source for the absorbance.

		AM1DExpressionAB *absorbanceSource = 0;

		if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
			absorbanceSource = new AM1DExpressionAB("Absorbance");
			absorbanceSource->setDescription("Absorbance");
			absorbanceSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << i1DetectorSource << i2DetectorSource);
			absorbanceSource->setExpression(QString("ln(%1/%2)").arg(i1DetectorSource->name(), i2DetectorSource->name()));

			scan->addAnalyzedDataSource(absorbanceSource, true, false);
		}

		// Create analyzed data source for the derivative of the absorbance.

		AM1DDerivativeAB *derivAbsorbanceSource = 0;

		if (absorbanceSource) {
			derivAbsorbanceSource = new AM1DDerivativeAB("DerivAbsorbance");
			derivAbsorbanceSource->setInputDataSources(QList<AMDataSource*>() << absorbanceSource);

			scan->addAnalyzedDataSource(derivAbsorbanceSource, true, false);
		}

		// Create analyzed data sources for the dark current corrected scaler channel detectors.

		AMDataSource *dwellTimeSource = 0;
		AM1DDarkCurrentCorrectionAB *i0CorrectedDetectorSource = 0;
		AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;
		AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

		AMDetector *scalerDwellTimeDetector = BioXASBeamline::bioXAS()->scalerDwellTimeDetector();

		if (scalerDwellTimeDetector) {
			int dwellTimeIndex = scan->indexOfDataSource(scalerDwellTimeDetector->name());
			if (dwellTimeIndex != -1) {
				dwellTimeSource = scan->dataSourceAt(dwellTimeIndex);
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

				scan->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);
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

				scan->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);
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

				scan->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);
			}
		}

		// Create analyzed data source for the absorbance, dark current corrected values.

		AM1DExpressionAB *absorbanceCorrectedSource = 0;

		if (i0CorrectedDetectorSource && i1CorrectedDetectorSource && i2CorrectedDetectorSource) {
			absorbanceCorrectedSource = new AM1DExpressionAB("Absorbance_DarkCorrect");
			absorbanceCorrectedSource->setDescription("Absorbance Dark Current Corrected");
			absorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << i0CorrectedDetectorSource << i1CorrectedDetectorSource << i2CorrectedDetectorSource);
			absorbanceCorrectedSource->setExpression(QString("ln(%1/%2)").arg(i1CorrectedDetectorSource->name(), i2CorrectedDetectorSource->name()));

			scan->addAnalyzedDataSource(absorbanceCorrectedSource, true, false);
		}

		// Create analyzed data source for the derivative of the absorbance, dark current corrected values.

		AM1DDerivativeAB *derivAbsorbanceCorrectedSource = 0;

		if (absorbanceCorrectedSource) {
			derivAbsorbanceCorrectedSource = new AM1DDerivativeAB("DerivAbsorbance_DarkCorrect");
			derivAbsorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << absorbanceCorrectedSource);

			scan->addAnalyzedDataSource(derivAbsorbanceCorrectedSource, true, false);
		}

		// Create analyzed data source for the Ge 32-el detector.

		AMXRFDetector *ge32Detector = BioXASBeamline::bioXAS()->ge32ElementDetector();
		BioXASXASScanConfiguration *bioxasConfiguration = qobject_cast<BioXASXASScanConfiguration*>(configuration);

		if (ge32Detector && bioxasConfiguration) {

			int ge32DetectorIndex = scan->indexOfDataSource(ge32Detector->name());

			if (ge32DetectorIndex != -1) {

				if (zebraTriggerSource) {
					zebraTriggerSource->addDetector(ge32Detector);
					zebraTriggerSource->addDetectorManager(ge32Detector);
				}

				// Clear any previous regions.

				ge32Detector->removeAllRegionsOfInterest();

				// Iterate through each region in the configuration.
				// Create analysis block for each region, add ge32Detector spectra source as input source for each.
				// Add analysis block to the scan and to the ge32Detector.
				// Create normalized analysis block for each region, add to scan.

				AMDataSource *spectraSource = scan->dataSourceAt(ge32DetectorIndex);
				QString edgeSymbol = bioxasConfiguration->edge().split(" ").first();
				bool canNormalize = (i0DetectorSource || i0CorrectedDetectorSource);

				foreach (AMRegionOfInterest *region, bioxasConfiguration->regionsOfInterest()){

					AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();

					AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
					newRegion->setBinningRange(regionAB->binningRange());
					newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);

					scan->addAnalyzedDataSource(newRegion, false, true);
					ge32Detector->addRegionOfInterest(region);

					if (canNormalize) {
						AMDataSource *normalizationSource = (i0CorrectedDetectorSource != 0) ? i0CorrectedDetectorSource : i0DetectorSource;

						AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
						normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << normalizationSource);
						normalizedRegion->setDataName(newRegion->name());
						normalizedRegion->setNormalizationName(normalizationSource->name());

						scan->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
					}
				}
			}
		}
	}
}


BioXASShutters* BioXASBeamline::shutters() const
{
	BioXASShutters *result = 0;

	if (utilities_)
		result = utilities_->shutters();

	return result;
}

BioXASValves* BioXASBeamline::beampathValves() const
{
	BioXASValves *result = 0;

	if (utilities_)
		result = utilities_->beampathValves();

	return result;
}

BioXASValves* BioXASBeamline::valves() const
{
	BioXASValves *result = 0;

	if (utilities_)
		result = utilities_->valves();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::ionPumps() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->ionPumps();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::flowSwitches() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->flowSwitches();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::pressureMonitors() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->pressureMonitors();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::temperatureMonitors() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->temperatureMonitors();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::flowTransducers() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->flowTransducers();

	return result;
}

AMBasicControlDetectorEmulator* BioXASBeamline::detectorForControl(AMControl *control) const
{
	return controlDetectorMap_.value(control, 0);
}

void BioXASBeamline::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASBeamline::updateConnected()
{
	setConnected( isConnected() );
}

void BioXASBeamline::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addShutter(newControl, openValue, closedValue);

	if (beamStatus_)
		beamStatus_->addShutter(newControl, openValue, closedValue);
}

void BioXASBeamline::removeShutter(AMControl *control)
{
	if (utilities_)
		utilities_->removeShutter(control);

	if (beamStatus_)
		beamStatus_->removeShutter(control);
}

void BioXASBeamline::clearShutters()
{
	if (utilities_)
		utilities_->clearShutters();

	if (beamStatus_)
		beamStatus_->clearShutters();
}

void BioXASBeamline::addBeampathValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addBeampathValve(newControl, openValue, closedValue);

	if (beamStatus_)
		beamStatus_->addValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeBeampathValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeBeampathValve(control);

	if (beamStatus_)
		beamStatus_->removeValve(control);
}

void BioXASBeamline::clearBeampathValves()
{
	if (utilities_)
		utilities_->clearBeampathValves();

	if (beamStatus_)
		beamStatus_->clearValves();
}

void BioXASBeamline::addValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeValve(control);
}

void BioXASBeamline::clearValves()
{
	if (utilities_)
		utilities_->clearValves();
}

void BioXASBeamline::addIonPump(AMControl *newControl)
{
	if (utilities_)
		utilities_->addIonPump(newControl);
}

void BioXASBeamline::removeIonPump(AMControl *control)
{
	if (utilities_)
		utilities_->removeIonPump(control);
}

void BioXASBeamline::clearIonPumps()
{
	if (utilities_)
		utilities_->clearIonPumps();
}

void BioXASBeamline::addFlowSwitch(AMControl *newControl)
{
	if (utilities_)
		utilities_->addFlowSwitch(newControl);
}

void BioXASBeamline::removeFlowSwitch(AMControl *control)
{
	if (utilities_)
		utilities_->removeFlowSwitch(control);
}

void BioXASBeamline::clearFlowSwitches()
{
	if (utilities_)
		utilities_->clearFlowSwitches();
}

void BioXASBeamline::addPressureMonitor(AMControl *newControl)
{
	if (utilities_)
		utilities_->addPressureMonitor(newControl);
}

void BioXASBeamline::removePressureMonitor(AMControl *control)
{
	if (utilities_)
		utilities_->removePressureMonitor(control);
}

void BioXASBeamline::clearPressureMonitors()
{
	if (utilities_)
		utilities_->clearPressureMonitors();
}

void BioXASBeamline::addTemperatureMonitor(AMControl *newControl)
{
	if (utilities_)
		utilities_->addTemperatureMonitor(newControl);
}

void BioXASBeamline::removeTemperatureMonitor(AMControl *control)
{
	if (utilities_)
		utilities_->removeTemperatureMonitor(control);
}

void BioXASBeamline::clearTemperatureMonitors()
{
	if (utilities_)
		utilities_->clearTemperatureMonitors();
}

void BioXASBeamline::addFlowTransducer(AMControl *newControl)
{
	if (utilities_)
		utilities_->addFlowTransducer(newControl);
}

void BioXASBeamline::removeFlowTransducer(AMControl *control)
{
	if (utilities_)
		utilities_->removeFlowTransducer(control);
}

void BioXASBeamline::clearFlowTransducers()
{
	if (utilities_)
		utilities_->clearFlowTransducers();
}

void BioXASBeamline::setupComponents()
{
	// Beam status.

	beamStatus_ = new BioXASBeamStatus("BioXASBeamStatus", this);
	connect( beamStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Utilities.

	utilities_ = new BioXASUtilities("BioXASUtilities", this);
	connect( utilities_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Utilities - front-end shutters.

	addShutter(new BioXASFrontEndShutters("Front-end shutters", this), BioXASFrontEndShutters::Open, BioXASFrontEndShutters::Closed);

	// Utilities - front-end beampath valves.

	addBeampathValve(new AMReadOnlyPVControl("VVR1407-I00-01", "VVR1407-I00-01:state", this), BIOXASBEAMLINE_VALVE_OPEN, BIOXASBEAMLINE_VALVE_CLOSED);
	addBeampathValve(new AMReadOnlyPVControl("VVF1407-I00-01", "VVF1407-I00-01:state", this), BIOXASBEAMLINE_FASTVALVE_OPEN, BIOXASBEAMLINE_FASTVALVE_CLOSED);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I00-01", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-01", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-01", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-01", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Side beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-02", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-03", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-04", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-06", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-07", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Main beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-02", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-03", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-04", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-06", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-07", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Imaging beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-02", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-03", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-04", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-05", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	//addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-06", "VVR1607-5-I10-06:state", "VVR1607-5-I10-06:opr:open", "VVR1607-5-I10-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	//addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-07", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-08", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-10", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - front-end ion pumps.

	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-01", "IOP1407-I00-01", this));
	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-02", "IOP1407-I00-02", this));
	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-03", "IOP1407-I00-03", this));
	addIonPump(new AMReadOnlyPVControl("IOP1607-5-I00-01", "IOP1607-5-I00-01", this));
	addIonPump(new AMReadOnlyPVControl("IOP1607-5-I00-02", "IOP1607-5-I00-02", this));

	// Utilities - front-end flow switches.

	addFlowSwitch(new AMReadOnlyPVControl("SWF1407-I00-01", "SWF1407-I00-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1407-I00-02", "SWF1407-I00-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1407-I00-03", "SWF1407-I00-03", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I00-01", "SWF1607-5-I00-01", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I20-01", "SWF1607-5-I20-01", this));

	// Utilities - beamline flow switches.

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-01", "SWF1607-5-I10-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-02", "SWF1607-5-I10-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-03", "SWF1607-5-I10-03", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-04", "SWF1607-5-I10-04", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-05", "SWF1607-5-I10-05", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-06", "SWF1607-5-I10-06", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-07", "SWF1607-5-I10-07", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I20-01", "SWF1607-5-I20-01", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-01", "SWF1607-5-I21-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-02", "SWF1607-5-I21-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-03", "SWF1607-5-I21-03", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-04", "SWF1607-5-I21-04", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-01", "SWF1607-5-I22-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-02", "SWF1607-5-I22-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-03", "SWF1607-5-I22-03", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-04", "SWF1607-5-I22-04", this));

	// Utilities - front-end pressure monitors.

	addPressureMonitor(new AMReadOnlyPVControl("CCG1407-I00-01", "CCG1407-I00-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1407-I00-02", "CCG1407-I00-02:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-02", "CCG1607-5-I00-02:vac", this));

	// Utilities - beamline pressure monitors.

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-03", "CCG1607-5-I00-03:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-04", "CCG1607-5-I00-04:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-05", "CCG1607-5-I00-05:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I10-01", "CCG1607-5-I10-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I10-02", "CCG1607-5-I10-02:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I10-03", "CCG1607-5-I10-03:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-8-I10-01", "CCG1607-8-I10-01:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I21-01", "CCG1607-5-I21-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I21-02", "CCG1607-5-I21-02:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I22-01", "CCG1607-5-I22-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I22-02", "CCG1607-5-I22-02:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I22-03", "CCG1607-5-I22-03:vac", this));

	// Utilities - front-end temperature monitors.

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-01", "TM1407-I00-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-02", "TM1407-I00-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-03", "TM1407-I00-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-04", "TM1407-I00-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-05", "TM1407-I00-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I00-01", "TM1607-5-I00-01:trip", this));

	// Utilities - beamline temperature monitors.

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-01", "TM1607-5-I10-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-02", "TM1607-5-I10-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-03", "TM1607-5-I10-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-04", "TM1607-5-I10-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-05", "TM1607-5-I10-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-06", "TM1607-5-I10-06:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-07", "TM1607-5-I10-07:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-08", "TM1607-5-I10-08:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-09", "TM1607-5-I10-09:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-10", "TM1607-5-I10-10:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-11", "TM1607-5-I10-11:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-12", "TM1607-5-I10-12:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-13", "TM1607-5-I10-13:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-14", "TM1607-5-I10-14:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-15", "TM1607-5-I10-15:trip", this));

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I20-01", "TM1607-5-I20-01:trip", this));

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-01", "TM1607-5-I21-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-02", "TM1607-5-I21-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-03", "TM1607-5-I21-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-04", "TM1607-5-I21-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-05", "TM1607-5-I21-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-06", "TM1607-5-I21-06:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-07", "TM1607-5-I21-07:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-08", "TM1607-5-I21-08:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-09", "TM1607-5-I21-09:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-10", "TM1607-5-I21-10:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-11", "TM1607-5-I21-11:trip", this));

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-01", "TM1607-5-I21-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-02", "TM1607-5-I21-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-03", "TM1607-5-I21-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-04", "TM1607-5-I21-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-05", "TM1607-5-I21-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-06", "TM1607-5-I21-06:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-07", "TM1607-5-I21-07:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-08", "TM1607-5-I21-08:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-09", "TM1607-5-I21-09:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-10", "TM1607-5-I21-10:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-11", "TM1607-5-I21-11:trip", this));

	// Utilities - front-end flow transducers.

	addFlowTransducer(new AMReadOnlyPVControl("FLT1407-I00-01", "FLT1407-I00-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1407-I00-02", "FLT1407-I00-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1407-I00-03", "FLT1407-I00-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I00-01", "FLT1607-5-I00-01:lowflow", this));

	// Utilities - beamline flow transducers.

	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-01", "FLT1607-5-I10-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-02", "FLT1607-5-I10-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-03", "FLT1607-5-I10-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-04", "FLT1607-5-I10-04:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-05", "FLT1607-5-I10-05:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-06", "FLT1607-5-I10-06:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-07", "FLT1607-5-I10-07:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I20-01", "FLT1607-5-I20-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-01", "FLT1607-5-I21-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-02", "FLT1607-5-I21-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-03", "FLT1607-5-I21-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-04", "FLT1607-5-I21-04:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-01", "FLT1607-5-I22-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-02", "FLT1607-5-I22-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-03", "FLT1607-5-I22-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-04", "FLT1607-5-I22-04:lowflow", this));
}

AMBasicControlDetectorEmulator* BioXASBeamline::createDetectorEmulator(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	AMBasicControlDetectorEmulator *detector = 0;

	if (control) {
		detector = new AMBasicControlDetectorEmulator(name, description, control, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
		detector->setHiddenFromUsers(hiddenFromUsers);
		detector->setIsVisible(isVisible);
	}

	return detector;
}

void BioXASBeamline::addControlAsDetector(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	if (control && !controlDetectorMap_.contains(control)) {
		AMBasicControlDetectorEmulator *detector = createDetectorEmulator(name, description, control, hiddenFromUsers, isVisible);
		controlDetectorMap_.insert(control, detector);
	}
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	beamStatus_ = 0;
	utilities_ = 0;

	// Setup procedures.

	setupComponents();
}
