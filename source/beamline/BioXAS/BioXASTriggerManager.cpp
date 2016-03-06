#include "BioXASTriggerManager.h"

BioXASTriggerManager::BioXASTriggerManager(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	triggerSource_ = 0;
	readMode_ = AMDetectorDefinitions::InvalidReadMode;
	armed_ = false;
	acquiring_ = false;
	triggeredAcquisition_ = false;

	// Current settings.

	setTriggerSource(new AMDetectorTriggerSource(QString("%1%2").arg(name).arg("TriggerSource"), this));
}

BioXASTriggerManager::~BioXASTriggerManager()
{

}

bool BioXASTriggerManager::canTrigger() const
{
	return ( canArm() );
}

bool BioXASTriggerManager::canArm() const
{
	bool canArm = true;

	for (int i = 0, count = detectors_.count(); i < count && canArm; i++)
		canArm = detectors_.at(i)->canArm();

	return canArm;
}

bool BioXASTriggerManager::canAcquire() const
{
	bool canAcquire = true;

	for (int i = 0, count = detectors_.count(); i < count && canAcquire; i++)
		canAcquire = detectors_.at(i)->isReadyForAcquisition();

	return canAcquire;
}

void BioXASTriggerManager::setTriggerSource(AMDetectorTriggerSource *newSource)
{
	if (triggerSource_ != newSource) {

		if (triggerSource_)
			disconnect( triggerSource_, 0, this, 0 );

		triggerSource_ = newSource;

		if (triggerSource_)
			connect( triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(trigger(AMDetectorDefinitions::ReadMode)) );

		emit triggerSourceChanged(triggerSource_);
	}
}

bool BioXASTriggerManager::addDetector(AMDetector *newDetector)
{
	bool result = false;

	if (newDetector && !detectors_.contains(newDetector)) {
		detectors_.append(newDetector);

		connect( newDetector, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
		connect( newDetector, SIGNAL(armed()), this, SLOT(updateArmed()) );
		connect( newDetector, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onDetectorAcquiringChanged()) );

		updateConnected();
		updateArmed();
		updateAcquiring();

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool BioXASTriggerManager::removeDetector(AMDetector *detector)
{
	bool result = false;

	if (detector && detectors_.contains(detector)) {
		detectors_.removeOne(detector);

		disconnect( detector, 0, this, 0 );

		updateConnected();
		updateArmed();
		updateAcquiring();

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool BioXASTriggerManager::clearDetectors()
{
	foreach (AMDetector *detector, detectors_) {
		disconnect( detector, 0, this, 0 );
	}

	detectors_.clear();

	updateConnected();
	updateArmed();
	updateAcquiring();

	emit detectorsChanged();

	return true;
}

void BioXASTriggerManager::trigger(AMDetectorDefinitions::ReadMode readMode)
{
	// An acquisition starts by arming each detector and trigger manager.
	// Once all detectors and managers are armed, they are triggered.

	if (canTrigger()) {

		triggeredAcquisition_ = true;

		emit triggered();

		setReadMode(readMode);

		AMAction3 *armAction = createArmAction();

		if (armAction) {
			connect( armAction, SIGNAL(succeeded()), this, SLOT(onArmActionSucceeded()) );

			connect( armAction, SIGNAL(cancelled()), armAction, SLOT(deleteLater()) );
			connect( armAction, SIGNAL(failed()), armAction, SLOT(deleteLater()) );
			connect( armAction, SIGNAL(succeeded()), armAction, SLOT(deleteLater()) );

			armAction->start();
		}
	}
}

void BioXASTriggerManager::arm()
{
	// Arm each detector.

	if (canArm()) {

		foreach (AMDetector *detector, detectors_)
			detector->arm();
	}
}

void BioXASTriggerManager::acquire(AMDetectorDefinitions::ReadMode readMode)
{
	// Acquire on each detector.

	if (canAcquire()) {
		setReadMode(readMode);
		acquisitionImplementation(readMode);
	}
}

void BioXASTriggerManager::updateConnected()
{
	setConnected( managerConnected() );
}

void BioXASTriggerManager::setArmed(bool isArmed)
{
	if (armed_ != isArmed) {
		armed_ = isArmed;
		emit armedChanged(armed_);
	}
}

void BioXASTriggerManager::updateArmed()
{
	setArmed( managerArmed() );
}

void BioXASTriggerManager::setReadMode(AMDetectorDefinitions::ReadMode newMode)
{
	if (readMode_ != newMode) {
		readMode_ = newMode;
		emit readModeChanged(readMode_);
	}
}

void BioXASTriggerManager::onArmActionSucceeded()
{
	acquire(readMode_);
}

void BioXASTriggerManager::setAcquiring(bool isAcquiring)
{
	if (acquiring_ != isAcquiring) {
		acquiring_ = isAcquiring;
		emit acquiringChanged(acquiring_);
	}
}

void BioXASTriggerManager::updateAcquiring()
{
	setAcquiring( managerAcquiring() );
}

void BioXASTriggerManager::onDetectorAcquiringChanged()
{
	// Updates the manager's acquisition state.

	updateAcquiring();

	// Updates the triggered acquisition flag. If we were doing a triggered
	// acquisition and we are no longer acquiring, then the flag should change
	// to false.

	if (triggeredAcquisition_ && !isAcquiring()) {
		triggeredAcquisition_ = false;

		if (triggerSource_)
			triggerSource_->setSucceeded();
	}
}

void BioXASTriggerManager::acquisitionImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	foreach (AMDetector *detector, detectors_)
		detector->acquire(readMode);
}

bool BioXASTriggerManager::detectorsConnected() const
{
	bool connected = true;

	for (int i = 0, count = detectors_.count(); i < count && connected; i++) // Stop if we find one detector that's not connected.
		connected = detectors_.at(i)->isConnected();

	return connected;
}

bool BioXASTriggerManager::managerConnected() const
{
	return ( detectorsConnected() );
}

bool BioXASTriggerManager::detectorsArmed() const
{
	bool armed = true;

	for (int i = 0, count = detectors_.count(); i < count && armed; i++) // Stop if we find one detector that's not armed.
		armed = detectors_.at(i)->isArmed();

	return armed;
}

bool BioXASTriggerManager::managerArmed() const
{
	return ( detectorsArmed() );
}

bool BioXASTriggerManager::detectorsAcquiring() const
{
	bool acquiring = false;

	for (int i = 0, count = detectors_.count(); i < count && !acquiring; i++) // Stop if we find one detector that's acquiring.
		acquiring = detectors_.at(i)->isAcquiring();

	return acquiring;
}

bool BioXASTriggerManager::managerAcquiring() const
{
	return ( detectorsAcquiring() );
}
