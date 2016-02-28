#include "AMDetectorManager.h"
#include "actions3/actions/AMDetectorManagerArmAction.h"
#include "actions3/actions/AMDetectorManagerTriggerAction.h"

AMDetectorManager::AMDetectorManager(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	// Initialize class variables.

	triggerSource_ = 0;

	connected_ = false;
	armed_ = false;
}

AMDetectorManager::~AMDetectorManager()
{

}

bool AMDetectorManager::detectorsConnected() const
{
	bool connected = true;

	for (int i = 0, count = detectors_.count(); i < count && connected; i++)
		connected = detectors_.at(i)->isConnected();

	return connected;
}

bool AMDetectorManager::detectorManagersConnected() const
{
	bool connected = true;

	for (int i = 0, count = detectorManagers_.count(); i < count && connected; i++)
		connected = detectorManagers_.at(i)->isConnected();

	return connected;
}

bool AMDetectorManager::detectorsArmed() const
{
	bool armed = true;

	for (int i = 0, count = detectors_.count(); i < count && armed; i++) {
		AMDetector *detector = detectors_.at(i);

		if ( !(detector && detector->isArmed()) )
			armed = false;
	}

	return armed;
}

bool AMDetectorManager::detectorManagersArmed() const
{
	bool armed = true;

	for (int i = 0, count = detectorManagers_.count(); i < count && armed; i++) {
		AMDetectorManager *manager = detectorManagers_.at(i);

		if ( !(manager && manager->isArmed()) )
			armed = false;
	}

	return armed;
}

AMAction3* AMDetectorManager::createArmAction()
{
	return new AMDetectorManagerArmAction(new AMDetectorManagerArmActionInfo(toInfo()));
}

AMAction3* AMDetectorManager::createTriggerAction(AMDetectorDefinitions::ReadMode readMode)
{
	return new AMDetectorManagerTriggerAction(new AMDetectorManagerTriggerActionInfo(toInfo(), readMode));
}

void AMDetectorManager::setTriggerSource(AMDetectorTriggerSource *newSource)
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

bool AMDetectorManager::addDetector(AMDetector *newDetector)
{
	bool result = false;

	if (newDetector && !detectors_.contains(newDetector)) {
		detectors_.append(newDetector);

		connect( newDetector, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
		connect( newDetector, SIGNAL(armed()), this, SLOT(updateArmed()) );

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool AMDetectorManager::removeDetector(AMDetector *detector)
{
	bool result = false;

	if (detector && detectors_.contains(detector)) {
		detectors_.removeOne(detector);

		disconnect( detector, 0, this, 0 );

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool AMDetectorManager::clearDetectors()
{
	foreach (AMDetector *detector, detectors_)
		disconnect( detector, 0, this, 0 );

	detectors_.clear();

	emit detectorsChanged();

	return true;
}

bool AMDetectorManager::addDetectorManager(AMDetectorManager *newManager)
{
	bool result = false;

	if (newManager && detectorManagers_.contains(newManager)) {
		detectorManagers_.append(newManager);

		connect( newManager, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
		connect( newManager, SIGNAL(armedChanged(bool)), this, SLOT(updateArmed()) );

		result = true;

		emit detectorManagersChanged();
	}

	return result;
}

bool AMDetectorManager::removeDetectorManager(AMDetectorManager *manager)
{
	bool result = false;

	if (manager && detectorManagers_.contains(manager)) {
		detectorManagers_.removeOne(manager);

		disconnect( manager, 0, this, 0 );

		result = true;

		emit detectorManagersChanged();
	}

	return result;
}

bool AMDetectorManager::clearDetectorManagers()
{
	foreach (AMDetectorManager *manager, detectorManagers_)
		disconnect( manager, 0, this, 0 );

	detectorManagers_.clear();

	emit detectorManagersChanged();

	return true;
}

void AMDetectorManager::arm()
{
	// Creates and executes an arm action.

	AMAction3 *armAction = createArmAction();

	if (armAction) {
		connect( armAction, SIGNAL(cancelled()), armAction, SLOT(deleteLater()) );
		connect( armAction, SIGNAL(failed()), armAction, SLOT(deleteLater()) );
		connect( armAction, SIGNAL(succeeded()), armAction, SLOT(deleteLater()) );

		armAction->start();
	}
}

void AMDetectorManager::trigger(AMDetectorDefinitions::ReadMode readMode)
{
	// Creates and executes a trigger action.

	AMAction3 *triggerAction = createTriggerAction(readMode);

	if (triggerAction) {
		connect( triggerAction, SIGNAL(succeeded()), this, SIGNAL(triggered()) ); // This, potentially problematic.

		connect( triggerAction, SIGNAL(cancelled()), triggerAction, SLOT(deleteLater()) );
		connect( triggerAction, SIGNAL(failed()), triggerAction, SLOT(deleteLater()) );
		connect( triggerAction, SIGNAL(succeeded()), triggerAction, SLOT(deleteLater()) );
	}
}

void AMDetectorManager::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void AMDetectorManager::setArmed(bool isArmed)
{
	if (armed_ != isArmed) {
		armed_ = isArmed;
		emit armedChanged(armed_);
	}
}

void AMDetectorManager::updateConnected()
{
	setConnected(detectorsConnected() && detectorManagersConnected());
}

void AMDetectorManager::updateArmed()
{
	setArmed(detectorsArmed() && detectorManagersArmed());
}
