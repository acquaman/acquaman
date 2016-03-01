#include "AMTriggerManagerModifyManagerAction.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMTriggerManager.h"
#include "util/AMErrorMonitor.h"

AMTriggerManagerModifyManagerAction::AMTriggerManagerModifyManagerAction(AMTriggerManagerModifyManagerActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->triggerManagerInfo()));
	detector_ = AMBeamline::bl()->exposedDetectorByInfo(modifyManagerInfo()->detectorInfo());
	manager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->managerInfo()));
}

AMTriggerManagerModifyManagerAction::AMTriggerManagerModifyManagerAction(const AMTriggerManagerModifyManagerAction &original) :
	AMAction3(original)
{
	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->triggerManagerInfo()));
	detector_ = AMBeamline::bl()->exposedDetectorByInfo(modifyManagerInfo()->detectorInfo());
	manager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->managerInfo()));
}

AMTriggerManagerModifyManagerAction::~AMTriggerManagerModifyManagerAction()
{

}

bool AMTriggerManagerModifyManagerAction::optionSupported(int option) const
{
	switch (option) {
	case AMTriggerManagerModifyManagerActionInfo::AddDetector:
		return true;
	case AMTriggerManagerModifyManagerActionInfo::RemoveDetector:
		return true;
	case AMTriggerManagerModifyManagerActionInfo::ClearDetectors:
		return true;
	case AMTriggerManagerModifyManagerActionInfo::AddManager:
		return true;
	case AMTriggerManagerModifyManagerActionInfo::RemoveManager:
		return true;
	case AMTriggerManagerModifyManagerActionInfo::ClearManagers:
		return true;
	default:
		return false;
	}
}
#include <QDebug>
void AMTriggerManagerModifyManagerAction::startImplementation()
{
	qDebug() << "\n\nModifying trigger manager" << triggerManager_->name();

	// Must have a trigger manager.

	if ( !(triggerManager_ && triggerManager_->isConnected()) ) {
		QString message = QString("Failed to modify trigger manager. The manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR_MANAGER, message);
		setFailed(message);
		return;
	}

	// Must have a valid modification option.

	int option = modifyManagerInfo()->option();

	if (!optionSupported(option)) {
		QString message = QString("Failed to modify trigger manager. The modification option provided is invalid or not supported.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_OPTION, message);
		setFailed(message);
		return;
	}

	// May require a valid detector, depending on the option.

	if (option == AMTriggerManagerModifyManagerActionInfo::AddDetector && !(detector_ && detector_->isConnected())) {
		QString message = QString("Failed to modify trigger manager. Cannot add new detector: the detector provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR, message);
		setFailed(message);
		return;
	}

	if (option == AMTriggerManagerModifyManagerActionInfo::RemoveDetector && !(detector_)) {
		QString message = QString("Failed to modify trigger manager. Cannot remove detector: the detector provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR, message);
		setFailed(message);
		return;
	}

	// May require a valid manager, depending on the option.

	if (option == AMTriggerManagerModifyManagerActionInfo::AddManager && !(manager_ && manager_->isConnected())) {
		QString message = QString("Failed to modify trigger manager. Cannot add manager: the manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	if (option == AMTriggerManagerModifyManagerActionInfo::AddManager && (triggerManager_ == manager_)) {
		QString message = QString("Failed to modify trigger manager. Cannot add manager to itself!");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	if (option == AMTriggerManagerModifyManagerActionInfo::RemoveManager && !manager_) {
		QString message = QString("Failed to modify trigger manager. Cannot remove manager: the manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	// Set the action as started.

	setStarted();

	// Perform the modification.

	bool result = false;

	if (option == AMTriggerManagerModifyManagerActionInfo::AddDetector)
		result = triggerManager_->addDetector(detector_);
	else if (option == AMTriggerManagerModifyManagerActionInfo::RemoveDetector)
		result = triggerManager_->removeDetector(detector_);
	else if (option == AMTriggerManagerModifyManagerActionInfo::ClearDetectors)
		result = triggerManager_->clearDetectors();
	else if (option == AMTriggerManagerModifyManagerActionInfo::AddManager)
		result = triggerManager_->addTriggerManager(manager_);
	else if (option == AMTriggerManagerModifyManagerActionInfo::RemoveManager)
		result = triggerManager_->removeTriggerManager(manager_);
	else if (option == AMTriggerManagerModifyManagerActionInfo::ClearManagers)
		result = triggerManager_->clearTriggerManagers();

	if (result) {
		setSucceeded();

	} else {
		QString message = QString("Failed to modify trigger manager.");
		AMErrorMon::alert(this, AMTRIGGERMANAGERMODIFYMANAGERACTION_MODIFY_FAILED, message);
		setFailed(message);
	}
}
