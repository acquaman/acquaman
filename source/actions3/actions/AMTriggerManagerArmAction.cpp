#include "AMTriggerManagerArmAction.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMTriggerManager.h"
#include "util/AMErrorMonitor.h"

AMTriggerManagerArmAction::AMTriggerManagerArmAction(AMTriggerManagerArmActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	// Could add a timeout?

	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerArmInfo()->triggerManagerInfo()));
}

AMTriggerManagerArmAction::AMTriggerManagerArmAction(const AMTriggerManagerArmAction &original) :
	AMAction3(original)
{
	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerArmInfo()->triggerManagerInfo()));
}

AMTriggerManagerArmAction::~AMTriggerManagerArmAction()
{

}

void AMTriggerManagerArmAction::onTriggerManagerArmed()
{
	setSucceeded();
}

void AMTriggerManagerArmAction::startImplementation()
{
	// Must have a trigger manager.

	if ( !(triggerManager_ && triggerManager_->isConnected()) ) {
		QString message = QString("Failed to arm trigger manager. The manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERARMACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	// The trigger manager must be able to be armed.

	if (!triggerManager_->canArm()) {
		QString message = QString("Failed to arm trigger manager %1. The manager provided cannot be armed.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERARMACTION_MANAGER_CANNOT_ARM, message);
		setFailed(message);
		return;
	}

	// Set the action as started.

	setStarted();

	// Arm the trigger manager and wait for the manager to signal its armed.

	connect( triggerManager_, SIGNAL(armed()), this, SLOT(onTriggerManagerArmed()) );

	triggerManager_->arm();
}
