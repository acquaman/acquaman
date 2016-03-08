#include "BioXASSideZebra.h"

BioXASSideZebra::BioXASSideZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	BioXASZebraPulseControl *pulse1 = pulseControlAt(0);
	if (pulse1)
		pulse1->setEdgeTriggerPreference(0);

	BioXASZebraPulseControl *pulse3 = pulseControlAt(2);
	if (pulse3)
		pulse3->setEdgeTriggerPreference(0);

	BioXASZebraSoftInputControl *softIn1 = softInputControlAt(0);
	if (softIn1)
		softIn1->setTimeBeforeResetPreference(0.1);

	BioXASZebraSoftInputControl *softIn3 = softInputControlAt(2);
	if (softIn3)
		softIn3->setTimeBeforeResetPreference(0.1);

	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);
	if (fastShutterBlock) {
		fastShutterBlock->setInputValuePreference(0, 61); // The fast shutter can be triggered using soft input 2 (#61).
//		fastShutterBlock->setInputValuePreference(1, 52); // The fast shutter can be triggered using pulse 1 (#52).
		fastShutterBlock->setInputValuePreference(1, 0); // The fast shutter is disconnected from pulse 1, until we want the fast shutter to be triggered with every acquisition point.
	}

	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);
	if (scalerBlock) {
		scalerBlock->setInputValuePreference(0, 52); // The scaler can be triggered using pulse 1 (#52).
		scalerBlock->setInputValuePreference(1, 62); // The scaler can be triggered using soft input 3 (#62).
	}

	BioXASZebraPulseControl *scanPulse = pulseControlAt(0);
	if (scanPulse)
		scanPulse->setInputValuePreference(60); // The 'scan pulse' can be triggered using soft input 1 (#60).

	BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);
	if (xspress3Pulse) {
//		xspress3Pulse->setInputValuePreference(52); // The Xspress3 detector can be triggered using pulse 1 (#52).
		xspress3Pulse->setInputValuePreference(0);
	}
}

BioXASSideZebra::~BioXASSideZebra()
{

}

void BioXASSideZebra::addDetector(AMDetector *newDetector)
{
	// Add detector to the trigger source.

	BioXASZebra::addDetector(newDetector);

	// If the detector is of a particular type, make sure the
	// Zebra is appropriately set up.

	if (qobject_cast<CLSBasicScalerChannelDetector*>(newDetector))
		addScalerChannelDetector(qobject_cast<CLSBasicScalerChannelDetector*>(newDetector));

	else if (qobject_cast<BioXAS32ElementGeDetector*>(newDetector))
		addGeDetector(qobject_cast<BioXAS32ElementGeDetector*>(newDetector));
}

void BioXASSideZebra::removeDetector(AMDetector *detector)
{
	// Remove the detector from the trigger source.

	BioXASZebra::removeDetector(detector);

	// If the detector was of a particular type, make sure the
	// Zebra is still properly set up.

	if (qobject_cast<CLSBasicScalerChannelDetector*>(detector))
		removeScalerChannelDetector(qobject_cast<CLSBasicScalerChannelDetector*>(detector));

	else if (qobject_cast<BioXAS32ElementGeDetector*>(detector))
		removeGeDetector(qobject_cast<BioXAS32ElementGeDetector*>(detector));
}

void BioXASSideZebra::clearDetectors()
{
	foreach (AMDetector *detector, triggerSource_->detectors())
		removeDetector(detector);
}

void BioXASSideZebra::addFastShutter()
{
	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);

	if (fastShutterBlock)
		fastShutterBlock->setInputValuePreference(1, 52);
}

void BioXASSideZebra::removeFastShutter()
{
	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);

	if (fastShutterBlock)
		fastShutterBlock->setInputValuePreference(1, 0);
}

void BioXASSideZebra::addScalerChannelDetector(CLSBasicScalerChannelDetector *detector)
{
	if (detector) {

		// Add the scaler.

		addScaler(detector->scaler());

		// Add the detector.

		BioXASZebra::addDetector(detector);

		if (!scalerChannelDetectors_.contains(detector))
			scalerChannelDetectors_.append(detector);
	}
}

void BioXASSideZebra::removeScalerChannelDetector(CLSBasicScalerChannelDetector *detector)
{
	if (detector) {

		// Remove the detector.

		BioXASZebra::removeDetector(detector);

		scalerChannelDetectors_.removeOne(detector);

		// Remove the scaler, if there are no more channel detectors.

		if (scalerChannelDetectors_.isEmpty())
			removeScaler(detector->scaler());
	}
}

void BioXASSideZebra::addScaler(CLSSIS3820Scaler *scaler)
{
	// Add scaler detector manager.

	BioXASZebra::addDetectorManager(scaler);

	// If the scaler is a BioXAS scaler, set the trigger source.

	BioXASSIS3820Scaler *bioxasScaler = qobject_cast<BioXASSIS3820Scaler*>(scaler);
	bioxasScaler->setTriggerSource(triggerSource_);

	// Setup the scaler OR block.

	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);

	if (scalerBlock)
		scalerBlock->setInputValuePreference(0, 52); // The scaler can be triggered using pulse 1 (#52).
}

void BioXASSideZebra::removeScaler(CLSSIS3820Scaler *scaler)
{
	if (scaler) {

		// Remove scaler detector manager.

		BioXASZebra::removeDetectorManager(scaler);

		// Disconnect the scaler OR block.

		BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);

		if (scalerBlock)
			scalerBlock->setInputValuePreference(0, 0);
	}
}

void BioXASSideZebra::addGeDetector(BioXAS32ElementGeDetector *detector)
{
	if (detector) {

		// Add Ge detector as detector and manager.

		BioXASZebra::addDetector(detector);
		BioXASZebra::addDetectorManager(detector);

		if (!geDetectors_.contains(detector))
			geDetectors_.append(detector);

		// Set the detector trigger source.

		detector->setTriggerSource(triggerSource_);

		// Setup the detector pulse block.

		BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);

		if (xspress3Pulse)
			xspress3Pulse->setInputValuePreference(52);
	}
}

void BioXASSideZebra::removeGeDetector(BioXAS32ElementGeDetector *detector)
{
	if (detector) {

		// Remove the Ge detector as detector and manager.

		BioXASZebra::removeDetector(detector);
		BioXASZebra::removeDetectorManager(detector);

		if (geDetectors_.contains(detector))
			geDetectors_.removeOne(detector);

		// Disconnect the detector pulse block.

		if (geDetectors_.isEmpty()) {
			BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);

			if (xspress3Pulse)
				xspress3Pulse->setInputValuePreference(0);
		}
	}
}
