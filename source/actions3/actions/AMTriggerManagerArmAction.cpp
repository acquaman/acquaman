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

	// Initialize class variables.

	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerArmInfo()->triggerManagerInfo()));

	armed_ = false;
	detectorsArmed_ = false;
	triggerManagersArmed_ = false;

	detectorArmingMapper_ = new QSignalMapper(this);
	connect( detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)) );

	triggerManagerArmingMapper_ = new QSignalMapper(this);
	connect( triggerManagerArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onTriggerManagerArmed(QObject*)) );
}

AMTriggerManagerArmAction::AMTriggerManagerArmAction(const AMTriggerManagerArmAction &original) :
	AMAction3(original)
{
	// Initialize class variables.

	triggerManager_ = qobject_cast<AMTriggerManager*>(AMBeamline::bl()->exposedControlByInfo(triggerManagerArmInfo()->triggerManagerInfo()));

	armed_ = false;
	detectorsArmed_ = false;
	triggerManagersArmed_ = false;

	detectorArmingMapper_ = new QSignalMapper(this);
	connect( detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)) );

	triggerManagerArmingMapper_ = new QSignalMapper(this);
	connect( triggerManagerArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onTriggerManagerArmed(QObject*)) );
}

AMTriggerManagerArmAction::~AMTriggerManagerArmAction()
{

}

void AMTriggerManagerArmAction::setArmed(bool isArmed)
{
	if (armed_ != isArmed) {
		armed_ = isArmed;

		if (armed_)
			setSucceeded();
	}
}

void AMTriggerManagerArmAction::updateArmed()
{
	setArmed(detectorsArmed_ && triggerManagersArmed_);
}

void AMTriggerManagerArmAction::onDetectorArmed(QObject *detectorObject)
{
	AMDetector *detector = qobject_cast<AMDetector*>(detectorObject);

	if (detector && !armedDetectors_.contains(detector))
		armedDetectors_.append(detector);

	if (armedDetectors_.count() == detectors_.count())
		detectorsArmed_ = true;

	updateArmed();
}

void AMTriggerManagerArmAction::onTriggerManagerArmed(QObject *managerObject)
{
	AMTriggerManager *manager = qobject_cast<AMTriggerManager*>(managerObject);

	if (manager && !armedTriggerManagers_.contains(manager))
		armedTriggerManagers_.append(manager);

	if (armedTriggerManagers_.count() == triggerManagers_.count())
		triggerManagersArmed_ = true;

	updateArmed();
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

	// Initialize lists of detectors and trigger managers.

	detectors_ = triggerManager_->detectors();
	triggerManagers_ = triggerManager_->triggerManagers();

	// Set up mappings for each detector and trigger manager.

	foreach (AMDetector *detector, detectors_) {
		detectorArmingMapper_->setMapping(detector, detector);
		connect( detector, SIGNAL(armed()), detectorArmingMapper_, SLOT(map()) );
	}

	foreach (AMTriggerManager *triggerManager, triggerManagers_) {
		triggerManagerArmingMapper_->setMapping(triggerManager, triggerManager);
		connect( triggerManager, SIGNAL(armedChanged(bool)), triggerManagerArmingMapper_, SLOT(map()) );
	}

	// Set the action as started.

	setStarted();

	// Arm each detector and trigger manager.

	foreach (AMDetector *detector, detectors_)
		detector->arm();

	foreach (AMTriggerManager *manager, triggerManagers_)
		manager->arm();
}
