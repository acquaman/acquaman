#include "AMTriggerManagerTriggerAction.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMTriggerManager.h"
#include "util/AMErrorMonitor.h"

AMTriggerManagerTriggerAction::AMTriggerManagerTriggerAction(AMTriggerManagerTriggerActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	// Could add a timeout?

	// Initialize class variables.

	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerTriggerInfo()->triggerManagerInfo()));

	triggered_ = false;
	detectorsTriggered_ = false;
	triggerManagersTriggered_ = false;

	detectorTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorTriggered(QObject*)) );

	triggerManagerTriggeringMapper_ = new QSignalMapper(this);
	connect( triggerManagerTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onTriggerManagerTriggered(QObject*)) );
}

AMTriggerManagerTriggerAction::AMTriggerManagerTriggerAction(const AMTriggerManagerTriggerAction &original) :
	AMAction3(original)
{
	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerTriggerInfo()->triggerManagerInfo()));

	detectorsTriggered_ = false;
	triggerManagersTriggered_ = false;

	detectorTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorTriggered(QObject*)) );

	triggerManagerTriggeringMapper_ = new QSignalMapper(this);
	connect( triggerManagerTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onTriggerManagerTriggered(QObject*)) );
}

AMTriggerManagerTriggerAction::~AMTriggerManagerTriggerAction()
{

}

void AMTriggerManagerTriggerAction::setTriggered(bool isTriggered)
{
	if (triggered_ != isTriggered) {
		triggered_ = isTriggered;

		if (triggered_)
			setSucceeded();
	}
}

void AMTriggerManagerTriggerAction::updateTriggered()
{
	setTriggered( detectorsTriggered_ && triggerManagersTriggered_ );
}

void AMTriggerManagerTriggerAction::onArmActionStarted()
{
	setStarted();
}

void AMTriggerManagerTriggerAction::onArmActionCancelled()
{
	setCancelled();
}

void AMTriggerManagerTriggerAction::onArmActionFailed()
{
	QString message = QString("Failed to trigger trigger manager %1").arg(triggerManager_->name());
	AMErrorMon::alert(this, AMTRIGGERMANAGERTRIGGERACTION_TRIGGER_FAILED, message);
	setFailed(message);
}

void AMTriggerManagerTriggerAction::onArmActionSucceeded()
{
	// Set up mappings for each detector and trigger manager.

	foreach (AMDetector *detector, detectors_) {
		detectorTriggeringMapper_->setMapping(detector, detector);
		connect( detector, SIGNAL(acquisitionSucceeded()), detectorTriggeringMapper_, SLOT(map()) );
	}

	foreach (AMTriggerManager *triggerManager, triggerManagers_) {
		triggerManagerTriggeringMapper_->setMapping(triggerManager, triggerManager);
		connect( triggerManager, SIGNAL(triggered()), triggerManagerTriggeringMapper_, SLOT(map()) );
	}

	// Trigger each detector and trigger manager.

	foreach (AMDetector *detector, detectors_)
		detector->acquire(AMDetectorDefinitions::ReadMode(triggerManagerTriggerInfo()->readMode()));

	foreach (AMTriggerManager *manager, triggerManagers_)
		manager->trigger(AMDetectorDefinitions::ReadMode(triggerManagerTriggerInfo()->readMode()));
}

void AMTriggerManagerTriggerAction::onDetectorTriggered(QObject *detectorObject)
{
	AMDetector *detector = qobject_cast<AMDetector*>(detectorObject);

	if (detector && !triggeredDetectors_.contains(detector))
		triggeredDetectors_.append(detector);

	if (triggeredDetectors_.count() == detectors_.count())
		detectorsTriggered_ = true;

	updateTriggered();
}

void AMTriggerManagerTriggerAction::onTriggerManagerTriggered(QObject *managerObject)
{
	AMTriggerManager *manager = qobject_cast<AMTriggerManager*>(managerObject);

	if (manager && !triggeredTriggerManagers_.contains(manager))
		triggeredTriggerManagers_.append(manager);

	if (triggeredTriggerManagers_.count() == triggerManagers_.count())
		triggerManagersTriggered_ = true;

	updateTriggered();
}

bool AMTriggerManagerTriggerAction::supportedReadMode(int readMode) const
{
	bool result = false;

	if (readMode == AMDetectorDefinitions::SingleRead)
		result = true;
	else if (readMode == AMDetectorDefinitions::ContinuousRead)
		result = true;

	return result;
}

void AMTriggerManagerTriggerAction::startImplementation()
{
	// Must have a trigger manager.

	if ( !(triggerManager_ && triggerManager_->isConnected()) ) {
		QString message = QString("Failed to trigger trigger manager. The manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERTRIGGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	// The trigger manager must be able to be triggered.

	if (!triggerManager_->canTrigger()) {
		QString message = QString("Failed to trigger trigger manager %1. The manager provided cannot be triggered.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERTRIGGERACTION_MANAGER_CANNOT_TRIGGER, message);
		setFailed(message);
		return;
	}

	// Must have a valid read mode.

	if (!supportedReadMode(triggerManagerTriggerInfo()->readMode())) {
		QString message = QString("Failed to trigger trigger manager %1. The acquisition read mode provided is invalid or not supported.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERTRIGGERACTION_INVALID_READ_MODE, message);
		setFailed(message);
		return;
	}

	// Initialize lists of detectors and trigger managers.

	detectors_ = triggerManager_->detectors();
	triggerManagers_ = triggerManager_->triggerManagers();

	// Create and execute arm action.

	AMAction3 *armAction = triggerManager_->createArmAction();

	if (armAction) {

		connect( armAction, SIGNAL(started()), this, SLOT(onArmActionStarted()) );
		connect( armAction, SIGNAL(cancelled()), this, SLOT(onArmActionCancelled()) );
		connect( armAction, SIGNAL(failed()), this, SLOT(onArmActionFailed()) );
		connect( armAction, SIGNAL(succeeded()), this, SLOT(onArmActionSucceeded()) );

		connect( armAction, SIGNAL(cancelled()), armAction, SLOT(deleteLater()) );
		connect( armAction, SIGNAL(failed()), armAction, SLOT(deleteLater()) );
		connect( armAction, SIGNAL(succeeded()), armAction, SLOT(deleteLater()) );

		armAction->start();
	}
}
