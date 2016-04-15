#include "BioXASBeamline.h"

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMDetectorWaitForAcquisitionStateAction.h"
#include "actions3/actions/AMDetectorTriggerAction.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AM1DNormalizationAB.h"

#include "beamline/AMControlSet.h"
#include "beamline/AM1DControlDetectorEmulator.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMScan.h"
#include "dataman/AMScanAxisEXAFSRegion.h"

#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool connected = (
				beamStatus_ && beamStatus_->isConnected() &&
				utilities_ && utilities_->isConnected() &&

				diodeDetector_ && diodeDetector_->isConnected() &&
				pipsDetector_ && pipsDetector_->isConnected() &&
				lytleDetector_ && lytleDetector_->isConnected() &&

				scalerDwellTimeDetector_ && scalerDwellTimeDetector_->isConnected() &&

				detectorStageLateralMotors_ && detectorStageLateralMotors_->isConnected() &&
				ge32Detectors_ && ge32Detectors_->isConnnected()
				);

	return connected;
}

AMAction3* BioXASBeamline::createDarkCurrentMeasurementAction(double dwellSeconds)
{
	AMListAction3 *result = 0;

	CLSExclusiveStatesControl *soeShutter = BioXASBeamline::bioXAS()->soeShutter();
	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (soeShutter && scaler) {

		// Identify the shutter's initial settings.

		bool shutterOpen = soeShutter->isOpen();

		// Create dark current measurement action.

		result = new AMListAction3(new AMListActionInfo3("BioXAS dark current measurement", "BioXAS dark current measurement"), AMListAction3::Sequential);
		result->addSubAction(AMActionSupport::buildControlMoveAction(soeShutter, CLSExclusiveStatesControl::Closed));
		result->addSubAction(scaler->createMeasureDarkCurrentAction(dwellSeconds));

		// Return shutter to initial settings.

		if (shutterOpen)
			result->addSubAction(AMActionSupport::buildControlMoveAction(soeShutter, CLSExclusiveStatesControl::Open));
	}

	return result;
}

AMAction3* BioXASBeamline::createScanInitializationAction(AMGenericStepScanConfiguration *configuration)
{
	AMListAction3 *initializationAction = new AMListAction3(new AMListActionInfo3("BioXAS scan initialization", "BioXAS scan initialization"), AMListAction3::Parallel);

	if (configuration) {

		// Initialize the zebra.

		AMListAction3 *zebraInitialization = 0;
		BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

		if (zebra) {
			zebraInitialization = new AMListAction3(new AMListActionInfo3("BioXAS Zebra initialization", "BioXAS Zebra initialization"));

			// Set up 'scan' pulse.

			BioXASZebraPulseControl *scanPulse = zebra->pulseControlAt(0);

			if (scanPulse)
				zebraInitialization->addSubAction(AMActionSupport::buildControlMoveAction(scanPulse->pulseWidthSecondsControl(), configuration->scanAxisAt(0)->regionAt(0)->regionTime()));

			// Set up Ge detector pulse.

			BioXASZebraPulseControl *detectorPulse = zebra->pulseControlAt(2);

			if (detectorPulse) {
				if (BioXASBeamlineSupport::usingAnyGeDetector(configuration))
					zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(52));
				else
					zebraInitialization->addSubAction(detectorPulse->createSetInputValueAction(0)); // Disconnects the pulse if the Ge detectors aren't being used.
			}
		}

		if (zebraInitialization)
			initializationAction->addSubAction(zebraInitialization);

		// Initialize the scaler.

		AMListAction3 *scalerInitialization = 0;
		CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

		if (BioXASBeamlineSupport::usingScaler(configuration)) {
			scalerInitialization = new AMListAction3(new AMListActionInfo3("BioXAS scaler initialization", "BioXAS scaler initialization"));
			scalerInitialization->addSubAction(scaler->createContinuousEnableAction3(false)); // Check that the scaler is in single shot mode and is not acquiring.

			// Determine the longest region time of all axis in the scan.

			double maxRegionTime = configuration->scanAxisAt(0)->regionAt(0)->regionTime();

			for (int i = 0, count = configuration->scanAxes().count(); i < count; i++) {
				double regionTime = configuration->scanAxisAt(i)->maximumRegionTime();

				if (maxRegionTime < regionTime)
					maxRegionTime = regionTime;
			}

			// Determine whether a dark current measurement is needed, by asking each scaler channel detector
			// if its dark current value would be valid for the longest region time.

			bool requiresDarkCurrentMeasurement = false;

			for (int i = 0, count = scaler->channels().count(); i < count && !requiresDarkCurrentMeasurement; i++)
				requiresDarkCurrentMeasurement = (scaler->channelAt(i)->detector() && !scaler->channelAt(i)->detector()->darkCurrentValidState(maxRegionTime));

			// If a dark current measurement is needed, add measurement to scaler initialization.

			if (requiresDarkCurrentMeasurement)
				scalerInitialization->addSubAction(BioXASBeamline::bioXAS()->createDarkCurrentMeasurementAction(maxRegionTime));
		}

		if (scalerInitialization)
			initializationAction->addSubAction(scalerInitialization);

		// Initialize Ge 32-el detector, if using.

		AMListAction3 *geDetectorsInitialization = 0;

		if (BioXASBeamlineSupport::usingAnyGeDetector(configuration)) {

			AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();
			geDetectorsInitialization = new AMListAction3(new AMListActionInfo3("BioXAS Xspress3s initialization", "BioXAS Xspress3s initialization"));

			for (int i = 0, count = geDetectors->count(); i < count; i++) {
				BioXAS32ElementGeDetector *geDetector = qobject_cast<BioXAS32ElementGeDetector*>(geDetectors->at(i));

				if (BioXASBeamlineSupport::usingGeDetector(configuration, geDetector)) {

					AMListAction3 *geDetectorInitialization = new AMListAction3(new AMListActionInfo3("BioXAS Xpress3 initialization", "BioXAS Xpress3 initialization"));
					geDetectorInitialization->addSubAction(geDetector->createDisarmAction());
					geDetectorInitialization->addSubAction(geDetector->createFramesPerAcquisitionAction(int(configuration->scanAxisAt(0)->numberOfPoints()*1.1)));	// Adding 10% just because.
					geDetectorInitialization->addSubAction(geDetector->createInitializationAction());

					AMDetectorWaitForAcquisitionStateAction *waitAction = new AMDetectorWaitForAcquisitionStateAction(new AMDetectorWaitForAcquisitionStateActionInfo(geDetector->toInfo(), AMDetector::ReadyForAcquisition), geDetector);
					geDetectorInitialization->addSubAction(waitAction);

					geDetectorsInitialization->addSubAction(geDetectorInitialization);
				}
			}
		}

		if (geDetectorsInitialization)
			initializationAction->addSubAction(geDetectorsInitialization);

		// Initialize the mono.

		AMListAction3 *monoInitialization = 0;
		BioXASSSRLMonochromator *mono = qobject_cast<BioXASSSRLMonochromator*>(BioXASBeamline::bioXAS()->mono());

		if (BioXASBeamlineSupport::usingMono(configuration)) {

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

		if (BioXASBeamlineSupport::usingStandardsWheel(bioxasConfiguration))
			standardsWheelInitialization = standardsWheel->createMoveToNameAction(bioxasConfiguration->edge().split(" ").first());
		else
			standardsWheelInitialization = standardsWheel->createMoveToNameAction("None");

		if (standardsWheelInitialization)
			initializationAction->addSubAction(standardsWheelInitialization);

		// Initialize the fast shutter.

		AMListAction3 *fastShutterInitialization = 0;
		BioXASFastShutter *fastShutter = BioXASBeamline::bioXAS()->fastShutter();

		if (fastShutter) {
			fastShutterInitialization = new AMListAction3(new AMListActionInfo3("BioXAS fast shutter initialization", "BioXAS fast shutter initialization"));
			fastShutterInitialization->addSubAction(AMActionSupport::buildControlMoveAction(fastShutter, BioXASFastShutter::Open));
		}

		if (fastShutterInitialization)
			initializationAction->addSubAction(fastShutterInitialization);
	}

	return initializationAction;
}

AMAction3* BioXASBeamline::createScanCleanupAction(AMGenericStepScanConfiguration *configuration)
{
	Q_UNUSED(configuration)

	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("BioXAS scan cleanup actions", "BioXAS scan cleanup actions"), AMListAction3::Parallel);

	// Create fast shutter cleanup actions.

	AMListAction3 *fastShutterCleanup = 0;
	BioXASFastShutter *fastShutter = BioXASBeamline::bioXAS()->fastShutter();

	if (fastShutter) {
		fastShutterCleanup = new AMListAction3(new AMListActionInfo3("BioXAS fast shutter initialization", "BioXAS fast shutter initialization"));
		fastShutterCleanup->addSubAction(AMActionSupport::buildControlMoveAction(fastShutter, BioXASFastShutter::Closed));
	}

	if (fastShutterCleanup)
		result->addSubAction(fastShutterCleanup);

	// Create scaler cleanup actions.

	AMListAction3 *scalerCleanup = 0;
	CLSSIS3820Scaler *scaler = BioXASBeamline::clsBeamline()->scaler();

	if (BioXASBeamlineSupport::usingScaler(configuration)) {
		scalerCleanup = new AMListAction3(new AMSequentialListActionInfo3("BioXAS Scaler cleanup", "BioXAS Scaler cleanup"));
		scalerCleanup->addSubAction(scaler->createContinuousEnableAction3(true)); // Put the scaler in Continuous mode.
	}

	if (scalerCleanup)
		result->addSubAction(scalerCleanup);

	// Create zebra cleanup actions.

	AMListAction3 *zebraCleanup = 0;
	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (BioXASBeamlineSupport::usingZebra(configuration)) {
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

	if (BioXASBeamlineSupport::usingMono(configuration)) {

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

AMTemperatureMonitorGroup* BioXASBeamline::temperatureMonitors() const
{
	AMTemperatureMonitorGroup *result = 0;

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

void BioXASBeamline::useCryostat(bool useCryostat)
{
	if (useCryostat && canUseCryostat() && cryostat())
		setUsingCryostat(true);
	else if (!useCryostat)
		setUsingCryostat(false);
}

bool BioXASBeamline::addDetectorStageLateralMotor(CLSMAXvMotor *newMotor)
{
	bool result = false;

	if (detectorStageLateralMotors_->addControl(newMotor)) {
		result = true;
		emit detectorStageLateralMotorsChanged();
	}

	return result;
}

bool BioXASBeamline::removeDetectorStageLateralMotor(CLSMAXvMotor *motor)
{
	bool result = false;

	if (detectorStageLateralMotors_->removeControl(motor)) {
		result = true;
		emit detectorStageLateralMotorsChanged();
	}

	return result;
}

bool BioXASBeamline::clearDetectorStageLateralMotors()
{
	detectorStageLateralMotors_->clear();
	emit detectorStageLateralMotorsChanged();
	return true;
}
#include <QDebug>
bool BioXASBeamline::addGe32Detector(BioXAS32ElementGeDetector *newDetector)
{
	bool result = false;

	if (newDetector && ge32Detectors_->addDetector(newDetector)) {

		qDebug() << "\n\nAdding new Ge detector.";

		// Add the detector to the appropriate detector sets.

		addExposedScientificDetector(newDetector);
		addExposedDetector(newDetector);
		addDefaultXASScanDetector(newDetector);
		addDefaultXASScanDetectorOption(newDetector);

		addSynchronizedXRFDetector(newDetector);

		// Add each detector spectrum control.

		qDebug() << "\tAdding" << newDetector->spectraControls().count() << "spectra controls.";

		foreach (AMControl *spectra, newDetector->spectraControls()) {
			AM1DControlDetectorEmulator *element = new AM1DControlDetectorEmulator(spectra->name(), spectra->description(), 4096, spectra, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
			element->setAccessAsDouble(true);
			element->setAxisInfo(newDetector->axes().first());
			addDetectorElement(newDetector, element);
		}

		// Add each detector ICR control.

		qDebug() << "\tAdding" << newDetector->icrControls().count() << "ICR controls.";

		for (int i = 0, count = newDetector->icrControls().count(); i < count; i++) {
			AMControl *icrControl = newDetector->icrControlAt(i);

			if (icrControl) {
				AMBasicControlDetectorEmulator *icrDetector = new AMBasicControlDetectorEmulator(QString("ICR %1").arg(i+1), QString("ICR %1").arg(i+1), icrControl, 0, 0, 0, AMDetectorDefinitions::ImmediateRead);
				icrDetector->setHiddenFromUsers(false);
				icrDetector->setIsVisible(true);

				addDetectorICR(newDetector, icrDetector);
			}
		}

		result = true;
		emit ge32DetectorsChanged();
	}

	return result;
}

bool BioXASBeamline::removeGe32Detector(BioXAS32ElementGeDetector *detector)
{
	bool result = false;

	if (ge32Detectors_->removeDetector(detector)) {

		// Remove the detector from the appropriate detector sets.

		removeExposedScientificDetector(detector);
		removeExposedDetector(detector);
		removeDefaultXASScanDetector(detector);
		removeDefaultXASScanDetectorOption(detector);

		// Remove each detector element.

		clearDetectorElements(detector);

		result = true;

		emit ge32DetectorsChanged();
	}

	return result;
}

bool BioXASBeamline::clearGe32Detectors()
{
	for (int i = 0, count = ge32Detectors_->count(); i < count; i++)
		removeGe32Detector(qobject_cast<BioXAS32ElementGeDetector*>(ge32Detectors_->at(i)));

	return true;
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

void BioXASBeamline::addTemperatureMonitor(AMTemperatureMonitor *newControl)
{
	if (utilities_)
		utilities_->addTemperatureMonitor(newControl);
}

void BioXASBeamline::removeTemperatureMonitor(AMTemperatureMonitor *control)
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

void BioXASBeamline::setSOEShutter(CLSExclusiveStatesControl *shutter)
{
	if (soeShutter_ != shutter) {

		if (soeShutter_)
			removeShutter(soeShutter_);

		soeShutter_ = shutter;

		if (soeShutter_)
			addShutter(soeShutter_, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

		emit soeShutterChanged(soeShutter_);
	}
}

void BioXASBeamline::setUsingCryostat(bool usingCryostat)
{
	if (usingCryostat_ != usingCryostat) {
		usingCryostat_ = usingCryostat;
		emit usingCryostatChanged(usingCryostat_);
	}
}

void BioXASBeamline::addExposedScalerChannelDetector(AMDetector *detector)
{
	if (detector) {
		addExposedDetector(detector);
		addExposedScientificDetector(detector);
		addDefaultXASScanDetector(detector);
		addDefaultXASScanDetectorOption(detector);
		addDefaultGenericScanDetector(detector);
		addDefaultGenericScanDetectorOption(detector);
	}
}

void BioXASBeamline::removeExposedScalerChannelDetector(AMDetector *detector)
{
	if (detector) {
		removeExposedDetector(detector);
		removeExposedScientificDetector(detector);
		removeDefaultXASScanDetector(detector);
		removeDefaultXASScanDetectorOption(detector);
		removeDefaultGenericScanDetector(detector);
		removeDefaultGenericScanDetectorOption(detector);
	}
}

bool BioXASBeamline::setUsingDiodeDetector(bool usingDetector)
{
	bool result = false;

	if (canUseDiodeDetector() && diodeDetector_ && usingDiodeDetector_ != usingDetector) {

		if (usingDiodeDetector_)
			removeExposedScalerChannelDetector(diodeDetector_);

		usingDiodeDetector_ = usingDetector;

		if (usingDiodeDetector_)
			addExposedScalerChannelDetector(diodeDetector_);

		result = true;

		emit usingDiodeDetectorChanged(usingDiodeDetector_);
	}

	return result;
}

bool BioXASBeamline::setDiodeDetector(CLSBasicScalerChannelDetector *detector)
{
	bool result = false;

	if (diodeDetector_ != detector) {

		if (diodeDetector_)
			disconnect( diodeDetector_, 0, this, 0 );

		diodeDetector_ = detector;

		if (diodeDetector_)
			connect( diodeDetector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		result = true;

		emit diodeDetectorChanged(diodeDetector_);
	}

	return result;
}

bool BioXASBeamline::setUsingPIPSDetector(bool usingDetector)
{
	bool result = false;

	if (canUsePIPSDetector() && pipsDetector_ && usingPIPSDetector_ != usingDetector) {

		if (usingPIPSDetector_)
			removeExposedScalerChannelDetector(pipsDetector_);

		usingPIPSDetector_ = usingDetector;

		if (usingPIPSDetector_)
			addExposedScalerChannelDetector(pipsDetector_);

		result = true;

		emit usingPIPSDetectorChanged(usingPIPSDetector_);
	}

	return result;
}

bool BioXASBeamline::setPIPSDetector(CLSBasicScalerChannelDetector *detector)
{
	bool result = false;

	if (pipsDetector_ != detector) {

		if (pipsDetector_)
			disconnect( pipsDetector_, 0, this, 0 );

		pipsDetector_ = detector;

		if (pipsDetector_)
			connect( pipsDetector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		result = true;

		emit pipsDetectorChanged(pipsDetector_);
	}

	return result;
}

bool BioXASBeamline::setUsingLytleDetector(bool usingDetector)
{
	bool result = false;

	if (canUseLytleDetector() && lytleDetector_ && usingLytleDetector_ != usingDetector) {

		if (usingLytleDetector_)
			removeExposedScalerChannelDetector(lytleDetector_);

		usingLytleDetector_ = usingDetector;

		if (usingLytleDetector_)
			addExposedScalerChannelDetector(lytleDetector_);

		result = true;

		emit usingLytleDetectorChanged(usingLytleDetector_);
	}

	return result;
}

bool BioXASBeamline::setLytleDetector(CLSBasicScalerChannelDetector *detector)
{
	bool result = false;

	if (lytleDetector_ != detector) {

		if (lytleDetector_)
			disconnect( lytleDetector_, 0, this, 0 );

		lytleDetector_ = detector;

		if (lytleDetector_)
			connect( lytleDetector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		result = true;

		emit lytleDetectorChanged(lytleDetector_);
	}

	return result;
}

bool BioXASBeamline::setScalerDwellTimeDetector(AMDetector *detector)
{
	bool result = false;

	if (scalerDwellTimeDetector_ != detector) {

		if (scalerDwellTimeDetector_) {
			disconnect( scalerDwellTimeDetector_, 0, this, 0 );

			removeExposedDetector(scalerDwellTimeDetector_);
			removeDefaultXASScanDetector(scalerDwellTimeDetector_);
			removeDefaultXASScanDetectorOption(scalerDwellTimeDetector_);
			removeDefaultGenericScanDetector(scalerDwellTimeDetector_);
			removeDefaultGenericScanDetectorOption(scalerDwellTimeDetector_);
		}

		scalerDwellTimeDetector_ = detector;

		if (scalerDwellTimeDetector_) {
			connect( scalerDwellTimeDetector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

			addExposedDetector(scalerDwellTimeDetector_);
			addDefaultXASScanDetector(scalerDwellTimeDetector_);
			addDefaultXASScanDetectorOption(scalerDwellTimeDetector_);
			addDefaultGenericScanDetector(scalerDwellTimeDetector_);
			addDefaultGenericScanDetectorOption(scalerDwellTimeDetector_);
		}

		result = true;

		emit scalerDwellTimeDetectorChanged(scalerDwellTimeDetector_);
	}

	return result;
}

bool BioXASBeamline::addDetectorElement(AMDetector *detector, AMDetector *element)
{
	bool result = false;

	// If this is the first element for this detector, create the detector
	// set used in the mapping.

	if (detector && !detectorElementsMap_.contains(detector))
		detectorElementsMap_.insert(detector, new AMDetectorSet(this));

	// Add the new element to the detector's elements.

	AMDetectorSet *elements = detectorElementsMap_.value(detector, 0);

	if (elements && elements->addDetector(element)) {
		addExposedDetector(element);
		result = true;
	}

	return result;
}

bool BioXASBeamline::removeDetectorElement(AMDetector *detector, AMDetector *element)
{
	bool result = false;

	AMDetectorSet *elements = detectorElementsMap_.value(detector, 0);

	// Remove the element from the elements set.

	if (elements && elements->removeDetector(element)) {
		removeExposedDetector(element);
		result = true;
	}

	return result;
}

bool BioXASBeamline::removeDetectorElements(AMDetector *detector)
{
	bool result = false;

	AMDetectorSet *elements = detectorElementsMap_.value(detector, 0);

	// Remove all elements from the elements set.

	if (elements) {
		for (int i = 0, count = elements->count(); i < count; i++)
			removeDetectorElement(detector, elements->at(i));

		result = true;
	}

	return result;
}

bool BioXASBeamline::clearDetectorElements(AMDetector *detector)
{
	bool result = false;

	AMDetectorSet *elements = detectorElementsMap_.value(detector, 0);

	// Remove all elements from the elements set, remove map entry,
	// and delete the set.

	if (elements) {
		removeDetectorElements(detector);

		detectorElementsMap_.remove(detector);
		elements->disconnect();
		elements->deleteLater();
		result = true;
	}

	return result;
}

bool BioXASBeamline::addDetectorICR(AMDetector *detector, AMDetector *icrDetector)
{
	bool result = false;

	// If this is the first ICR for this detector, create the detector
	// set used in the mapping.

	if (detector && !detectorICRsMap_.contains(detector))
		detectorICRsMap_.insert(detector, new AMDetectorSet(this));

	// Add the new ICR to the detector's ICRs.

	AMDetectorSet *icrs = detectorICRsMap_.value(detector, 0);

	if (icrs && icrs->addDetector(icrDetector)) {
		addExposedDetector(icrDetector);
		result = true;
	}

	return result;
}

bool BioXASBeamline::removeDetectorICR(AMDetector *detector, AMDetector *icrDetector)
{
	bool result = false;

	AMDetectorSet *icrs = detectorICRsMap_.value(detector, 0);

	// Remove the ICR detector from the detectors set.

	if (icrs && icrs->removeDetector(icrDetector)) {
		removeExposedDetector(icrDetector);
		result = true;
	}

	return result;
}

bool BioXASBeamline::removeDetectorICRs(AMDetector *detector)
{
	bool result = false;

	AMDetectorSet *icrs = detectorICRsMap_.value(detector, 0);

	// Remove all ICRs from the detectors set.

	if (icrs) {
		for (int i = 0, count = icrs->count(); i < count; i++)
			removeDetectorICR(detector, icrs->at(i));

		result = true;
	}

	return result;
}

bool BioXASBeamline::clearDetectorICRs(AMDetector *detector)
{
	bool result = false;

	AMDetectorSet *icrs = detectorElementsMap_.value(detector, 0);

	// Remove all ICRs from the detectors set, remove map entry,
	// and delete the set.

	if (icrs) {
		removeDetectorElements(detector);

		detectorICRsMap_.remove(detector);
		icrs->disconnect();
		icrs->deleteLater();
		result = true;
	}

	return result;
}

bool BioXASBeamline::addDefaultXASScanDetector(AMDetector *detector)
{
	return defaultXASScanDetectors_->addDetector(detector);
}

bool BioXASBeamline::removeDefaultXASScanDetector(AMDetector *detector)
{
	return defaultXASScanDetectors_->removeDetector(detector);
}

bool BioXASBeamline::clearDefaultXASScanDetectors()
{
	defaultXASScanDetectors_->clear();
	return true;
}

bool BioXASBeamline::addDefaultXASScanDetectorOption(AMDetector *detector)
{
	return defaultXASScanDetectorOptions_->addDetector(detector);
}

bool BioXASBeamline::removeDefaultXASScanDetectorOption(AMDetector *detector)
{
	return defaultXASScanDetectorOptions_->removeDetector(detector);
}

bool BioXASBeamline::clearDefaultXASScanDetectorOptions()
{
	defaultXASScanDetectorOptions_->clear();
	return true;
}

bool BioXASBeamline::addDefaultGenericScanDetector(AMDetector *detector)
{
	return defaultGenericScanDetectors_->addDetector(detector);
}

bool BioXASBeamline::removeDefaultGenericScanDetector(AMDetector *detector)
{
	return defaultGenericScanDetectors_->removeDetector(detector);
}

bool BioXASBeamline::clearDefaultGenericScanDetectors()
{
	defaultGenericScanDetectors_->clear();
	return true;
}

bool BioXASBeamline::addDefaultGenericScanDetectorOption(AMDetector *detector)
{
	return defaultGenericScanDetectorOptions_->addDetector(detector);
}

bool BioXASBeamline::removeDefaultGenericScanDetectorOption(AMDetector *detector)
{
	return defaultGenericScanDetectorOptions_->removeDetector(detector);
}

bool BioXASBeamline::clearDefaultGenericScanDetectorOptions()
{
	defaultGenericScanDetectorOptions_->clear();
	return true;
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

	addTemperatureMonitor(new CLSTemperatureMonitor("TM1407-I00-01", "TM1407-I00-01", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1407-I00-02", "TM1407-I00-02", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1407-I00-03", "TM1407-I00-03", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1407-I00-04", "TM1407-I00-04", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1407-I00-05", "TM1407-I00-05", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I00-01", "TM1607-5-I00-01", this));

	// Utilities - beamline temperature monitors.

	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-01", "TM1607-5-I10-01", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-02", "TM1607-5-I10-02", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-03", "TM1607-5-I10-03", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-04", "TM1607-5-I10-04", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-05", "TM1607-5-I10-05", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-06", "TM1607-5-I10-06", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-07", "TM1607-5-I10-07", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-08", "TM1607-5-I10-08", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-09", "TM1607-5-I10-09", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-10", "TM1607-5-I10-10", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-11", "TM1607-5-I10-11", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-12", "TM1607-5-I10-12", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-13", "TM1607-5-I10-13", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-14", "TM1607-5-I10-14", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I10-15", "TM1607-5-I10-15", this));

	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I20-01", "TM1607-5-I20-01", this));

	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-01", "TM1607-5-I21-01", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-02", "TM1607-5-I21-02", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-03", "TM1607-5-I21-03", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-04", "TM1607-5-I21-04", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-05", "TM1607-5-I21-05", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-06", "TM1607-5-I21-06", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-07", "TM1607-5-I21-07", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-08", "TM1607-5-I21-08", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-09", "TM1607-5-I21-09", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-10", "TM1607-5-I21-10", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I21-11", "TM1607-5-I21-11", this));

	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-01", "TM1607-5-I21-01", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-02", "TM1607-5-I21-02", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-03", "TM1607-5-I21-03", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-04", "TM1607-5-I21-04", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-05", "TM1607-5-I21-05", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-06", "TM1607-5-I21-06", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-07", "TM1607-5-I21-07", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-08", "TM1607-5-I21-08", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-09", "TM1607-5-I21-09", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-10", "TM1607-5-I21-10", this));
	addTemperatureMonitor(new CLSTemperatureMonitor("TM1607-5-I22-11", "TM1607-5-I21-11", this));

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

	// Detector stage motors.

	detectorStageLateralMotors_ = new AMControlSet(this);
	connect( detectorStageLateralMotors_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// 32Ge detectors.

	ge32Detectors_ = new AMDetectorSet(this);
	connect( ge32Detectors_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The default detectors sets for XAS scans.

	defaultXASScanDetectors_ = new AMDetectorSet(this);
	defaultXASScanDetectorOptions_ = new AMDetectorSet(this);

	// The default detectors sets for generic scans.

	defaultGenericScanDetectors_ = new AMDetectorSet(this);
	defaultGenericScanDetectorOptions_ = new AMDetectorSet(this);
}

AMDetector* BioXASBeamline::createScalerDwellTimeDetector(CLSSIS3820Scaler *scaler)
{
	AMDetector *result = 0;

	if (scaler)
		result = createDetectorEmulator("ScalerDwellTimeFeedback", "Scaler Dwell Time Feedback", scaler->dwellTimeControl(), false, true);

	return result;
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
		addExposedDetector(detector);
		addDefaultXASScanDetectorOption(detector);
	}
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	beamStatus_ = 0;
	utilities_ = 0;

	soeShutter_ = 0;

	usingCryostat_ = false;

	usingDiodeDetector_ = false;
	diodeDetector_ = 0;

	usingPIPSDetector_ = false;
	pipsDetector_ = 0;

	usingLytleDetector_ = false;
	lytleDetector_ = 0;

	scalerDwellTimeDetector_ = 0;

	// Setup procedures.

	setupComponents();
}
