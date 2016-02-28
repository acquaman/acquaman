#include "AMDetectorManager.h"
#include "actions3/actions/AMDetectorManagerArmAction.h"

AMDetectorManager::AMDetectorManager(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	// Initialize class variables.

	triggerSource_ = 0;

	readMode_ = AMDetectorDefinitions::SingleRead;

	connected_ = false;
	armed_ = false;
	triggered_ = false;

	detectorsTriggered_ = false;
	detectorManagersTriggered_ = false;

	detectorTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorTriggered(QObject*)) );

	detectorManagerTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorManagerTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorManagerTriggered(QObject*)) );
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

void AMDetectorManager::setTriggerSource(AMDetectorTriggerSource *newSource)
{
	if (triggerSource_ != newSource) {

		if (triggerSource_)
			disconnect( triggerSource_, 0, this, 0 );

		triggerSource_ = newSource;

		if (triggerSource_)
			connect( triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)) );

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

		detectorTriggeringMapper_->setMapping(newDetector, newDetector);
		connect( newDetector, SIGNAL(acquisitionCancelled()), detectorTriggeringMapper_, SLOT(map()) );
		connect( newDetector, SIGNAL(acquisitionFailed()), detectorTriggeringMapper_, SLOT(map()) );
		connect( newDetector, SIGNAL(acquisitionSucceeded()), detectorTriggeringMapper_, SLOT(map()) );

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

		detectorTriggeringMapper_->removeMappings(detector);
		disconnect( detector, 0, detectorTriggeringMapper_, 0 );

		result = true;

		emit detectorsChanged();
	}

	return result;
}

bool AMDetectorManager::clearDetectors()
{
	foreach (AMDetector *detector, detectors_) {
		disconnect( detector, 0, this, 0 );

		detectorTriggeringMapper_->removeMappings(detector);
		disconnect( detector, 0, detectorTriggeringMapper_, 0 );
	}

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

		detectorManagerTriggeringMapper_->setMapping(newManager, newManager);
		connect( newManager, SIGNAL(triggered()), detectorManagerTriggeringMapper_, SLOT(map()) );

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

		detectorManagerTriggeringMapper_->removeMappings(manager);
		disconnect( manager, SIGNAL(triggered()), detectorManagerTriggeringMapper_, SLOT(map()) );

		result = true;

		emit detectorManagersChanged();
	}

	return result;
}

bool AMDetectorManager::clearDetectorManagers()
{
	foreach (AMDetectorManager *manager, detectorManagers_) {
		disconnect( manager, 0, this, 0 );

		detectorManagerTriggeringMapper_->removeMappings(manager);
		disconnect( manager, SIGNAL(triggered()), detectorManagerTriggeringMapper_, SLOT(map()) );
	}

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
	if (triggerSource_)
		triggerSource_->trigger(readMode);
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

void AMDetectorManager::setTriggered(bool isTriggered)
{
	if (triggered_ != isTriggered) {
		triggered_ = isTriggered;

		// We only want to emit triggered() when triggered_ is set to true.
		// Additionally, triggered_ should not stay true until set to something
		// else, should immediately be reset.

		if (triggered_) {
			triggered_ = false;
			emit triggered();
		}
	}
}

void AMDetectorManager::setReadMode(AMDetectorDefinitions::ReadMode newMode)
{
	if (readMode_ != newMode) {
		readMode_ = newMode;
		emit readModeChanged(readMode_);
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

void AMDetectorManager::updateTriggered()
{
	setTriggered(detectorsTriggered_ && detectorManagersTriggered_);
}

void AMDetectorManager::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode)
{
	setReadMode(readMode);

	// Clear the lists of previously triggered detectors and detector managers.

	detectorsTriggered_ = false;
	triggeredDetectors_.clear();

	detectorManagersTriggered_ = false;
	triggeredDetectorManagers_.clear();

	// Trigger an acquisition for each detector.

	foreach (AMDetector *detector, detectors_)
		detector->acquire(readMode);

	// Trigger each detector manager.

	foreach (AMDetectorManager *manager, detectorManagers_)
		manager->trigger(readMode);
}

void AMDetectorManager::onDetectorTriggered(QObject *detectorObject)
{
	AMDetector *detector = qobject_cast<AMDetector*>(detectorObject);

	if (detector && !triggeredDetectors_.contains(detector))
		triggeredDetectors_.append(detector);

	if (triggeredDetectors_.count() == detectors_.count())
		detectorsTriggered_ = true;

	updateTriggered();
}

void AMDetectorManager::onDetectorManagerTriggered(QObject *managerObject)
{
	AMDetectorManager *manager = qobject_cast<AMDetectorManager*>(managerObject);

	if (manager && !triggeredDetectorManagers_.contains(manager))
		triggeredDetectorManagers_.append(manager);

	if (triggeredDetectorManagers_.count() == detectorManagers_.count())
		detectorManagersTriggered_ = true;

	updateTriggered();
}
