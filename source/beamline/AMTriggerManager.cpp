#include "AMTriggerManager.h"
#include "actions3/actions/AMTriggerManagerArmAction.h"
#include "actions3/actions/AMTriggerManagerTriggerAction.h"
#include "actions3/actions/AMTriggerManagerModifyManagerAction.h"

AMTriggerManager::AMTriggerManager(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	// Initialize class variables.

	triggerSource_ = 0;

	armed_ = false;
	detectorsArmed_ = false;
	triggerManagersArmed_ = false;

	detectorArmingMapper_ = new QSignalMapper(this);
	connect( detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)) );

	triggerManagerArmingMapper_ = new QSignalMapper(this);
	connect( triggerManagerArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onTriggerManagerArmed(QObject*)) );
}

AMTriggerManager::~AMTriggerManager()
{

}

AMAction3* AMTriggerManager::createAddDetectorAction(AMDetector *detector)
{
	AMAction3 *result = 0;

	if (detector)
		result = new AMTriggerManagerModifyManagerAction(new AMTriggerManagerModifyManagerActionInfo(toInfo(), AMTriggerManagerModifyManagerActionInfo::AddDetector, detector->toInfo()));

	return result;
}

AMAction3* AMTriggerManager::createRemoveDetectorAction(AMDetector *detector)
{
	AMAction3 *result = 0;

	if (detector)
		result = new AMTriggerManagerModifyManagerAction(new AMTriggerManagerModifyManagerActionInfo(toInfo(), AMTriggerManagerModifyManagerActionInfo::RemoveDetector, detector->toInfo()));

	return result;
}

AMAction3* AMTriggerManager::createClearDetectorsAction()
{
	return new AMTriggerManagerModifyManagerAction(new AMTriggerManagerModifyManagerActionInfo(toInfo(), AMTriggerManagerModifyManagerActionInfo::ClearDetectors));
}

AMAction3* AMTriggerManager::createAddTriggerManagerAction(AMTriggerManager *manager)
{
	AMAction3 *result = 0;

	if (manager)
		result = new AMTriggerManagerModifyManagerAction(new AMTriggerManagerModifyManagerActionInfo(toInfo(), AMTriggerManagerModifyManagerActionInfo::AddManager, AMDetectorInfo(), manager->toInfo()));

	return result;
}

AMAction3* AMTriggerManager::createRemoveTriggerManagerAction(AMTriggerManager *manager)
{
	AMAction3 *result = 0;

	if (manager)
		result = new AMTriggerManagerModifyManagerAction(new AMTriggerManagerModifyManagerActionInfo(toInfo(), AMTriggerManagerModifyManagerActionInfo::RemoveManager, AMDetectorInfo(), manager->toInfo()));

	return result;
}

AMAction3* AMTriggerManager::createClearTriggerManagersAction()
{
	return new AMTriggerManagerModifyManagerAction(new AMTriggerManagerModifyManagerActionInfo(toInfo(), AMTriggerManagerModifyManagerActionInfo::ClearManagers));
}

AMAction3* AMTriggerManager::createArmAction()
{
	AMAction3 *result = new AMTriggerManagerArmAction(new AMTriggerManagerArmActionInfo(toInfo()));
	connect( result, SIGNAL(succeeded()), this, SIGNAL(armed()) );

	return result;
}

AMAction3* AMTriggerManager::createTriggerAction(AMDetectorDefinitions::ReadMode readMode)
{
	AMAction3 *result = new AMTriggerManagerTriggerAction(new AMTriggerManagerTriggerActionInfo(toInfo(), readMode));
	connect( result, SIGNAL(succeeded()), this, SIGNAL(triggered()) );

	return result;
}

void AMTriggerManager::setTriggerSource(AMDetectorTriggerSource *newSource)
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

bool AMTriggerManager::addDetector(AMDetector *newDetector)
{
	bool result = false;

	if (newDetector && !detectors_.contains(newDetector)) {
		detectors_.append(newDetector);

		connect( newDetector, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		detectorArmingMapper_->setMapping(newDetector, newDetector);
		connect( newDetector, SIGNAL(armed()), detectorArmingMapper_, SLOT(map()) );

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool AMTriggerManager::removeDetector(AMDetector *detector)
{
	bool result = false;

	if (detector && detectors_.contains(detector)) {
		detectors_.removeOne(detector);

		disconnect( detector, 0, this, 0 );

		detectorArmingMapper_->removeMappings(detector);
		disconnect( detector, 0, detectorArmingMapper_, 0 );

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool AMTriggerManager::clearDetectors()
{
	foreach (AMDetector *detector, detectors_) {
		disconnect( detector, 0, this, 0 );

		detectorArmingMapper_->removeMappings(detector);
		disconnect( detector, 0, detectorArmingMapper_, 0 );
	}

	detectors_.clear();

	emit detectorsChanged();

	return true;
}

bool AMTriggerManager::addTriggerManager(AMTriggerManager *newManager)
{
	bool result = false;

	if (newManager && !triggerManagers_.contains(newManager) && this != newManager) {
		triggerManagers_.append(newManager);

		connect( newManager, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		triggerManagerArmingMapper_->setMapping(newManager, newManager);
		connect( newManager, SIGNAL(armed()), triggerManagerArmingMapper_, SLOT(map()) );

		result = true;

		emit triggerManagersChanged();
	}

	return result;
}

bool AMTriggerManager::removeTriggerManager(AMTriggerManager *manager)
{
	bool result = false;

	if (manager && triggerManagers_.contains(manager)) {
		triggerManagers_.removeOne(manager);

		disconnect( manager, 0, this, 0 );

		triggerManagerArmingMapper_->removeMappings(manager);
		disconnect( manager, 0, triggerManagerArmingMapper_, 0 );

		result = true;

		emit triggerManagersChanged();
	}

	return result;
}

bool AMTriggerManager::clearTriggerManagers()
{
	foreach (AMTriggerManager *manager, triggerManagers_) {
		disconnect( manager, 0, this, 0 );

		triggerManagerArmingMapper_->removeMappings(manager);
		disconnect( manager, 0, triggerManagerArmingMapper_, 0 );
	}

	triggerManagers_.clear();

	emit triggerManagersChanged();

	return true;
}

void AMTriggerManager::arm()
{
	// Clear the lists of previously armed detectors and trigger managers.

	armedDetectors_.clear();
	armedTriggerManagers_.clear();

	// Arm each detector and trigger manager.

	foreach (AMDetector *detector, detectors_)
		detector->arm();

	foreach (AMTriggerManager *manager, triggerManagers_)
		manager->arm();
}

void AMTriggerManager::trigger(AMDetectorDefinitions::ReadMode readMode)
{
	AMAction3 *triggerAction = createTriggerAction(readMode);

	if (triggerAction) {

		// Make connections.

		connect( triggerAction, SIGNAL(cancelled()), triggerAction, SLOT(deleteLater()) );
		connect( triggerAction, SIGNAL(failed()), triggerAction, SLOT(deleteLater()) );
		connect( triggerAction, SIGNAL(succeeded()), triggerAction, SLOT(deleteLater()) );

		// Run action.

		triggerAction->start();
	}
}

void AMTriggerManager::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void AMTriggerManager::updateConnected()
{
	setConnected(managerConnected());
}

void AMTriggerManager::setArmed(bool isArmed)
{
	if (armed_ != isArmed) {
		armed_ = isArmed;

		if (armed_)
			emit armed();
	}
}

void AMTriggerManager::updateArmed()
{
	setArmed(managerArmed());
}

void AMTriggerManager::onDetectorArmed(QObject *detectorObject)
{
	AMDetector *detector = qobject_cast<AMDetector*>(detectorObject);

	if (detector && !armedDetectors_.contains(detector))
		armedDetectors_.append(detector);

	if (armedDetectors_.count() == detectors_.count())
		detectorsArmed_ = true;

	updateArmed();
}

void AMTriggerManager::onTriggerManagerArmed(QObject *managerObject)
{
	AMTriggerManager *manager = qobject_cast<AMTriggerManager*>(managerObject);

	if (manager && !armedTriggerManagers_.contains(manager))
		armedTriggerManagers_.append(manager);

	if (armedTriggerManagers_.count() == triggerManagers_.count())
		triggerManagersArmed_ = true;

	updateArmed();
}

bool AMTriggerManager::detectorsConnected() const
{
	bool connected = true;

	for (int i = 0, count = detectors_.count(); i < count && connected; i++)
		connected = detectors_.at(i)->isConnected();

	return connected;
}

bool AMTriggerManager::triggerManagersConnected() const
{
	bool connected = true;

	for (int i = 0, count = triggerManagers_.count(); i < count && connected; i++)
		connected = triggerManagers_.at(i)->isConnected();

	return connected;
}

bool AMTriggerManager::managerConnected() const
{
	return ( detectorsConnected() && triggerManagersConnected() );
}

bool AMTriggerManager::detectorsArmed() const
{
	bool armed = true;

	for (int i = 0, count = detectors_.count(); i < count && armed; i++)
		armed = detectors_.at(i)->isArmed();

	return armed;
}

bool AMTriggerManager::triggerManagersArmed() const
{
	bool armed = true;

	for (int i = 0, count = triggerManagers_.count(); i < count && armed; i++)
		armed = triggerManagers_.at(i)->isArmed();

	return armed;
}

bool AMTriggerManager::managerArmed() const
{
	return (detectorsArmed() && triggerManagersArmed());
}
