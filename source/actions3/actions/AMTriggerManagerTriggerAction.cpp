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

	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerTriggerInfo()->triggerManagerInfo()));
}

AMTriggerManagerTriggerAction::AMTriggerManagerTriggerAction(const AMTriggerManagerTriggerAction &original) :
	AMAction3(original)
{
	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerTriggerInfo()->triggerManagerInfo()));
}

AMTriggerManagerTriggerAction::~AMTriggerManagerTriggerAction()
{

}

void AMTriggerManagerTriggerAction::onTriggerManagerTriggered()
{
	setSucceeded();
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

	int readMode = triggerManagerTriggerInfo()->readMode();

	// Must have a supported read mode.

	if (!supportedReadMode(readMode)) {
		QString message = QString("Failed to trigger trigger manager %1. The acquisition read mode provided is invalid or not supported.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERTRIGGERACTION_INVALID_READ_MODE, message);
		setFailed(message);
		return;
	}

	// Set the action as started.

	setStarted();

	// Trigger the trigger manager and wait for the manager to signal it's triggered.

	connect( triggerManager_, SIGNAL(triggered()), this, SLOT(onTriggerManagerTriggered()) );

	triggerManager_->trigger(AMDetectorDefinitions::ReadMode(readMode));
}
