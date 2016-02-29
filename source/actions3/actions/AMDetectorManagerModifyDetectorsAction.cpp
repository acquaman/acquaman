#include "AMDetectorManagerModifyManagerAction.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMDetectorManager.h"
#include "util/AMErrorMonitor.h"

AMDetectorManagerModifyManagerAction::AMDetectorManagerModifyManagerAction(AMDetectorManagerModifyManagerActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	// Initialize class variables.

	detectorManager_ = 0;
	detector_ = 0;
	manager_ = 0;

	// Current settings.

	setDetectorManager(qobject_cast<AMDetectorManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->detectorManagerInfo())));
	setDetector(AMBeamline::bl()->exposedDetectorByInfo(modifyManagerInfo()->detectorInfo()));
	setManager(qobject_cast<AMDetectorManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->managerInfo())));
}

AMDetectorManagerModifyManagerAction::AMDetectorManagerModifyManagerAction(const AMDetectorManagerModifyManagerAction &original) :
	AMAction3(original)
{
	// Initialize class variables.

	detectorManager_ = 0;
	detector_ = 0;
	manager_ = 0;

	// Current settings.

	setDetectorManager(qobject_cast<AMDetectorManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->detectorManagerInfo())));
	setDetector(AMBeamline::bl()->exposedDetectorByInfo(modifyManagerInfo()->detectorInfo()));
	setManager(qobject_cast<AMDetectorManager*>(AMBeamline::bl()->exposedControlByInfo(modifyManagerInfo()->managerInfo())));
}

AMDetectorManagerModifyManagerAction::~AMDetectorManagerModifyManagerAction()
{

}

void AMDetectorManagerModifyManagerAction::setDetectorManager(AMDetectorManager *newManager)
{
	if (detectorManager_ != newManager)
		detectorManager_ = newManager;
}

void AMDetectorManagerModifyManagerAction::setDetector(AMDetector *newDetector)
{
	if (detector_ != newDetector)
		detector_ = newDetector;
}

void AMDetectorManagerModifyManagerAction::setManager(AMDetectorManager *newManager)
{
	if (manager_ != newManager)
		manager_ = newManager;
}

bool AMDetectorManagerModifyManagerAction::optionSupported(int option) const
{
	switch (option) {
	case AMDetectorManagerModifyManagerActionInfo::AddDetector:
		return true;
	case AMDetectorManagerModifyManagerActionInfo::RemoveDetector:
		return true;
	case AMDetectorManagerModifyManagerActionInfo::ClearDetectors:
		return true;
	case AMDetectorManagerModifyManagerActionInfo::AddManager:
		return true;
	case AMDetectorManagerModifyManagerActionInfo::RemoveManager:
		return true;
	case AMDetectorManagerModifyManagerActionInfo::ClearManagers:
		return true;
	default:
		return false;
	}
}

void AMDetectorManagerModifyManagerAction::startImplementation()
{
	// Must have a detector manager.

	if ( !(detectorManager_ && detectorManager_->isConnected()) ) {
		QString message = QString("Failed to modify detector manager. The manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR_MANAGER, message);
		setFailed(message);
		return;
	}

	// Must have a valid modification option.

	int option = modifyManagerInfo()->option();

	if (!optionSupported(option)) {
		QString message = QString("Failed to modify detector manager. The modification option provided is invalid or not supported.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_OPTION, message);
		setFailed(message);
		return;
	}

	// May require a valid detector, depending on the option.

	if (option == AMDetectorManagerModifyManagerActionInfo::AddDetector && !(detector_ && detector_->isConnected())) {
		QString message = QString("Failed to modify detector manager. Cannot add new detector: the detector provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR, message);
		setFailed(message);
		return;
	}

	if (option == AMDetectorManagerModifyManagerActionInfo::RemoveDetector && !(detector_)) {
		QString message = QString("Failed to modify detector manager. Cannot remove detector: the detector provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_DETECTOR, message);
		setFailed(message);
		return;
	}

	// May require a valid manager, depending on the option.

	if (option == AMDetectorManagerModifyManagerActionInfo::AddManager && !(manager_ && manager_->isConnected())) {
		QString message = QString("Failed to modify detector manager. Cannot add manager: the manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	if (option == AMDetectorManagerModifyManagerActionInfo::AddManager && (detectorManager_ == manager_)) {
		QString message = QString("Failed to modify detector manager. Cannot add manager to itself!");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	if (option == AMDetectorManagerModifyManagerActionInfo::RemoveManager && !manager_) {
		QString message = QString("Failed to modify detector manager. Cannot remove manager: the manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	// Set the action as started.

	setStarted();

	// Perform the modification.

	bool result = false;

	if (option == AMDetectorManagerModifyManagerActionInfo::AddDetector)
		result = detectorManager_->addDetector(detector_);
	else if (option == AMDetectorManagerModifyManagerActionInfo::RemoveDetector)
		result = detectorManager_->removeDetector(detector_);
	else if (option == AMDetectorManagerModifyManagerActionInfo::ClearDetectors)
		result = detectorManager_->clearDetectors();
	else if (option == AMDetectorManagerModifyManagerActionInfo::AddManager)
		result = detectorManager_->addDetectorManager(manager_);
	else if (option == AMDetectorManagerModifyManagerActionInfo::RemoveManager)
		result = detectorManager_->removeDetectorManager(manager_);
	else if (option == AMDetectorManagerModifyManagerActionInfo::ClearManagers)
		result = detectorManager_->clearDetectorManagers();

	if (result) {
		setSucceeded();

	} else {
		QString message = QString("Failed to modify detector manager.");
		AMErrorMon::alert(this, AMDETECTORMANAGERMODIFYMANAGERACTION_MODIFY_FAILED, message);
		setFailed(message);
	}
}
