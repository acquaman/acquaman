#include "BioXASSideZebra.h"

BioXASSideZebra::BioXASSideZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	BioXASZebraSoftInputControl *softIn1 = softInputControlAt(0);
	if (softIn1)
		softIn1->setTimeBeforeResetPreference(0.1); // The zebra has a good chance of missing triggers if this value is lower (definitely at 0.01).

	BioXASZebraSoftInputControl *softIn2 = softInputControlAt(1);
	if (softIn2)
		softIn2->setTimeBeforeResetPreference(0);  // The fast shutter control should toggle high or toggle low when triggered.

	BioXASZebraSoftInputControl *softIn3 = softInputControlAt(2);
	if (softIn3)
		softIn3->setTimeBeforeResetPreference(0.1); // The zebra has a good chance of missing triggers if this value is lower (definitely at 0.01).

	BioXASZebraPulseControl *scanPulse = pulseControlAt(0);
	if (scanPulse) {
		scanPulse->setEdgeTriggerPreference(0);
		scanPulse->setInputValuePreference(60); // The 'scan' pulse can always be triggered using soft input 1.
	}

	BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);
	if (xspress3Pulse) {
		xspress3Pulse->setEdgeTriggerPreference(0);
		xspress3Pulse->setInputValuePreference(0);
	}

	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);
	if (fastShutterBlock) {
		fastShutterBlock->setInputValuePreference(0, 61); // The fast shutter can always be triggered using soft input 2.
		fastShutterBlock->setInputValuePreference(1, 0);
	}

	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);
	if (scalerBlock) {
		scalerBlock->setInputValuePreference(0, 62); // The scaler can always be triggered using soft input 3.
		scalerBlock->setInputValuePreference(1, 0);
	}

	qDebug() << "\n\n\n";
}

BioXASSideZebra::~BioXASSideZebra()
{

}

bool BioXASSideZebra::addDetector(AMDetector *newDetector)
{
	// Add detector to the trigger source.

	bool result = BioXASZebra::addDetector(newDetector);

	if (result) {

		// If the detector is of a particular type, make sure the
		// Zebra is appropriately set up.

		if (qobject_cast<CLSBasicScalerChannelDetector*>(newDetector))
			addScalerChannelDetector(qobject_cast<CLSBasicScalerChannelDetector*>(newDetector));

		else if (qobject_cast<BioXAS32ElementGeDetector*>(newDetector))
			addGeDetector(qobject_cast<BioXAS32ElementGeDetector*>(newDetector));
	}

	return result;
}

bool BioXASSideZebra::removeDetector(AMDetector *detector)
{
	// Remove the detector from the trigger source.

	bool result = BioXASZebra::removeDetector(detector);

	if (result) {

		// If the detector was of a particular type, make sure the
		// Zebra is still properly set up.

		if (qobject_cast<CLSBasicScalerChannelDetector*>(detector))
			removeScalerChannelDetector(qobject_cast<CLSBasicScalerChannelDetector*>(detector));

		else if (qobject_cast<BioXAS32ElementGeDetector*>(detector))
			removeGeDetector(qobject_cast<BioXAS32ElementGeDetector*>(detector));
	}

	return result;
}

bool BioXASSideZebra::clearDetectors()
{
	foreach (AMDetector *detector, triggerSource_->detectors())
		removeDetector(detector);

	return true;
}

void BioXASSideZebra::addFastShutter()
{
	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);

	if (fastShutterBlock)
		fastShutterBlock->setInputValuePreference(1, 52); // The fast shutter is triggered by the 'scan' pulse.
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

		// Add the detector.

		BioXASZebra::addDetector(detector);

		if (!scalerChannelDetectors_.contains(detector))
			scalerChannelDetectors_.append(detector);

		// Add the scaler.

		addScaler(detector->scaler());
	}
}

void BioXASSideZebra::removeScalerChannelDetector(CLSBasicScalerChannelDetector *detector)
{

	if (detector) {

		// Remove the detector.

		BioXASZebra::removeDetector(detector);

		if (scalerChannelDetectors_.contains(detector))
			scalerChannelDetectors_.removeOne(detector);

		// Remove the scaler, if there are no more channel detectors.

		if (scalerChannelDetectors_.isEmpty())
			removeScaler(detector->scaler());
	}
}

void BioXASSideZebra::addScaler(CLSSIS3820Scaler *scaler)
{
	if (scaler) {

		// Add scaler detector manager.

		BioXASZebra::addDetectorManager(scaler);

		// If the scaler is a BioXAS scaler, set the trigger source.

		BioXASSIS3820Scaler *bioxasScaler = qobject_cast<BioXASSIS3820Scaler*>(scaler);

		if (bioxasScaler)
			bioxasScaler->setTriggerSource(triggerSource_);

		// Setup the scaler OR block.

		BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);

		if (scalerBlock) {
			qDebug() << "Adding scaler. Updating value preferences for the OR block.";
			scalerBlock->setInputValuePreference(1, 52); // The scaler can be triggered using pulse 1 (#52).
		}
	}
}

void BioXASSideZebra::removeScaler(CLSSIS3820Scaler *scaler)
{
	if (scaler) {

		// Remove scaler detector manager.

		BioXASZebra::removeDetectorManager(scaler);

		// Disconnect the scaler OR block.

		BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);

		if (scalerBlock) {
			qDebug() << "Removing scaler. Updating value preferences for the OR block.";
			scalerBlock->setInputValuePreference(1, 0);
		}
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
