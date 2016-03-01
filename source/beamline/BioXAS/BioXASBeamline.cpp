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
				utilities_ && utilities_->isConnected() &&
				beamStatus_ && beamStatus_->isConnected()
				);

	return connected;
}

AMAction3* BioXASBeamline::createScanInitializationAction(AMGenericStepScanConfiguration *configuration)
{
	AMSequentialListAction3 *result = 0;

	// Initialize the scaler.

	AMSequentialListAction3 *scalerInitialization = 0;
	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (scaler && usingScaler(configuration)) {

		scalerInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Initialization Actions", "BioXAS Scaler Initialization Actions"));

		// Check that the scaler is in single shot mode and is not acquiring.

		scalerInitialization->addSubAction(scaler->createContinuousEnableAction3(false));

		// Clear all detectors and managers. Add those used for this scan.

//		scalerInitialization->addSubAction(scaler->createClearDetectorsAction());

//		if (usingI0Detector(configuration))
//			scalerInitialization->addSubAction(scaler->createAddDetectorAction(BioXASBeamline::bioXAS()->i0Detector()));

//		if (usingI1Detector(configuration))
//			scalerInitialization->addSubAction(scaler->createAddDetectorAction(BioXASBeamline::bioXAS()->i1Detector()));

//		if (usingI2Detector(configuration))
//			scalerInitialization->addSubAction(scaler->createAddDetectorAction(BioXASBeamline::bioXAS()->i2Detector()));

//		scalerInitialization->addSubAction(scaler->createClearDetectorManagersAction());
	}

	// Initialize Ge 32-el detector, if using.

	AMSequentialListAction3 *geDetectorInitialization = 0;
	BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();

	if (geDetector && usingGeDetector(configuration)) {

		geDetectorInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Xpress3 Initialization", "BioXAS Xpress3 Initialization"));
		geDetectorInitialization->addSubAction(geDetector->createDisarmAction());
		geDetectorInitialization->addSubAction(geDetector->createFramesPerAcquisitionAction(int(configuration->scanAxisAt(0)->numberOfPoints()*1.1)));	// Adding 10% just because.
		geDetectorInitialization->addSubAction(geDetector->createInitializationAction());

		AMDetectorWaitForAcquisitionStateAction *waitAction = new AMDetectorWaitForAcquisitionStateAction(new AMDetectorWaitForAcquisitionStateActionInfo(geDetector->toInfo(), AMDetector::ReadyForAcquisition), geDetector);
		geDetectorInitialization->addSubAction(waitAction);
	}

	// Initialize the zebra.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();
	AMSequentialListAction3 *zebraInitialization = 0;

	if (zebra && usingZebra(configuration)) {

		zebraInitialization = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Zebra Initialization", "BioXAS Zebra Initialization"));

		// Set up the Ge detector pulse control, if we are using the detector in the scan.

		BioXASZebraPulseControl *detectorPulse = zebra->pulseControlAt(2);

		if (detectorPulse) {
			if (usingGeDetector(configuration))
				zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(52));
			else
				zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(0));
		}

		// Clear all detectors and managers. Add those used for this scan.

		zebraInitialization->addSubAction(zebra->createClearDetectorsAction());

		if (usingGeDetector(configuration))
			zebraInitialization->addSubAction(zebra->createAddDetectorAction(geDetector));

		zebraInitialization->addSubAction(zebra->createClearDetectorManagersAction());

//		if (usingScaler(configuration))
//			zebraInitialization->addSubAction(zebra->createAddDetectorManagerAction(scaler));
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
	BioXASXASScanConfiguration *xasConfiguration = qobject_cast<BioXASXASScanConfiguration*>(configuration);

	if (standardsWheel) {
		if (xasConfiguration && standardsWheel->indexFromName(xasConfiguration->edge().split(" ").first()) != -1) {
			standardsWheelInitialization = standardsWheel->createMoveToNameAction(xasConfiguration->edge().split(" ").first());
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

AMAction3* BioXASBeamline::createScanCleanupAction(AMGenericStepScanConfiguration *configuration)
{
	AMSequentialListAction3 *result = 0;

	// Create scaler cleanup actions.

	AMSequentialListAction3 *scalerCleanup = 0;
	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler && usingScaler(configuration)) {

		scalerCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Scaler Cleanup", "BioXAS Scaler Cleanup"));

		// Clear all detectors and managers. Add the defaults.

//		scalerCleanup->addSubAction(scaler->createClearDetectorsAction());

//		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
//		if (i0Detector)
//			scalerCleanup->addSubAction(scaler->createAddDetectorAction(i0Detector));

//		AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();
//		if (i1Detector)
//			scalerCleanup->addSubAction(scaler->createAddDetectorAction(i1Detector));

//		AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();
//		if (i2Detector)
//			scalerCleanup->addSubAction(scaler->createAddDetectorAction(i2Detector));

//		scalerCleanup->addSubAction(scaler->createClearDetectorManagersAction());

		// Put the scaler in Continuous mode.

		scalerCleanup->addSubAction(scaler->createContinuousEnableAction3(true));
	}

	// Create zebra cleanup actions.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();
	AMSequentialListAction3 *zebraCleanup = 0;

	if (zebra && usingZebra(configuration)) {

		zebraCleanup = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Zebra Initialization", "BioXAS Zebra Initialization"));

		// Clear detectors and managers. Add defaults.

		zebraCleanup->addSubAction(zebra->createClearDetectorsAction());

		BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();
		if (geDetector)
			zebraCleanup->addSubAction(zebra->createAddDetectorAction(geDetector));

		zebraCleanup->addSubAction(zebra->createClearDetectorManagersAction());

//		CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();
//		if (scaler)
//			zebraCleanup->addSubAction(zebra->createAddDetectorManagerAction(scaler));

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

QString BioXASBeamline::scanNotes() const
{
	QString notes;

	// Note the storage ring current.

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(QString::number(CLSStorageRing::sr1()->ringCurrent(), 'f', 1)));

	// Note the mono settling time, if applicable.

	BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(BioXASBeamline::bioXAS()->mono());
	if (mono) {
		double settlingTime = mono->bragg()->settlingTime();
		if (settlingTime > 0)
			notes.append(QString("Settling time:\t%1 s\n").arg(settlingTime));
	}

	return notes;
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
}

void BioXASBeamline::removeShutter(AMControl *control)
{
	if (utilities_)
		utilities_->removeShutter(control);
}

void BioXASBeamline::clearShutters()
{
	if (utilities_)
		utilities_->clearShutters();
}

void BioXASBeamline::addBeampathValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addBeampathValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeBeampathValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeBeampathValve(control);
}

void BioXASBeamline::clearBeampathValves()
{
	if (utilities_)
		utilities_->clearBeampathValves();
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

bool BioXASBeamline::usingScaler(AMGenericStepScanConfiguration *configuration) const
{
	bool result = false;

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (scaler)
		result = ( usingI0Detector(configuration) || usingI1Detector(configuration) || usingI2Detector(configuration) );

	return result;
}

bool BioXASBeamline::usingI0Detector(AMGenericStepScanConfiguration *configuration) const
{
	bool result = false;

	AMDetector *i0 = BioXASBeamline::bioXAS()->i0Detector();

	if (i0)
		result = (configuration->detectorConfigurations().indexOf(i0->name()) != -1);

	return result;
}

bool BioXASBeamline::usingI1Detector(AMGenericStepScanConfiguration *configuration) const
{
	bool result = false;

	AMDetector *i1 = BioXASBeamline::bioXAS()->i1Detector();

	if (i1)
		result = (configuration->detectorConfigurations().indexOf(i1->name()) != -1);

	return result;
}

bool BioXASBeamline::usingI2Detector(AMGenericStepScanConfiguration *configuration) const
{
	bool result = false;

	AMDetector *i2 = BioXASBeamline::bioXAS()->i2Detector();

	if (i2)
		result = (configuration->detectorConfigurations().indexOf(i2->name()) != -1);

	return result;
}

bool BioXASBeamline::usingZebra(AMGenericStepScanConfiguration *configuration) const
{
	Q_UNUSED(configuration)

	bool result = false;

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra)
		result = true;

	return result;
}

bool BioXASBeamline::usingGeDetector(AMGenericStepScanConfiguration *configuration) const
{
	bool result = false;

	BioXAS32ElementGeDetector *geDetector = BioXASBeamline::bioXAS()->ge32ElementDetector();

	if (geDetector)
		result = (configuration->detectorConfigurations().indexOf(geDetector->name()) != -1);

	return result;
}

void BioXASBeamline::setupComponents()
{
	// Utilities.

	utilities_ = new BioXASUtilities("BioXASUtilities", this);
	connect( utilities_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Beam status.

	beamStatus_ = new BioXASBeamStatus("BioXASBeamStatus", this);
	connect( beamStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	beamStatus_->addComponent(utilities_->shutters(), BioXASShutters::Open);
	beamStatus_->addComponent(utilities_->beampathValves(), BioXASValves::Open);

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
