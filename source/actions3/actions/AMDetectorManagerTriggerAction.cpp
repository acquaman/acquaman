#include "AMDetectorManagerTriggerAction.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMDetectorManager.h"
#include "util/AMErrorMonitor.h"

AMDetectorManagerTriggerAction::AMDetectorManagerTriggerAction(AMDetectorManagerTriggerActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	// Could add a timeout?

	// Initialize class variables.

	detectorManager_ = 0;

	triggered_ = false;
	detectorsTriggered_ = false;
	detectorManagersTriggered_ = false;

	detectorTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorTriggered(QObject*)) );

	detectorManagerTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorManagerTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorManagerTriggered(QObject*)) );

	// Current settings.

	setDetectorManager(qobject_cast<AMDetectorManager*>(AMBeamline::bl()->exposedControlByInfo(detectorManagerTriggerInfo()->detectorManagerInfo())));
}

AMDetectorManagerTriggerAction::AMDetectorManagerTriggerAction(const AMDetectorManagerTriggerAction &original) :
	AMAction3(original)
{
	detectorsTriggered_ = false;
	detectorManagersTriggered_ = false;

	detectorTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorTriggered(QObject*)) );

	detectorManagerTriggeringMapper_ = new QSignalMapper(this);
	connect( detectorManagerTriggeringMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorManagerTriggered(QObject*)) );
}

AMDetectorManagerTriggerAction::~AMDetectorManagerTriggerAction()
{

}

void AMDetectorManagerTriggerAction::setDetectorManager(AMDetectorManager *newManager)
{
	if (detectorManager_ != newManager)
		detectorManager_ = newManager;
}

void AMDetectorManagerTriggerAction::setTriggered(bool isTriggered)
{
	if (triggered_ != isTriggered) {
		triggered_ = isTriggered;

		if (triggered_)
			setSucceeded();
	}
}

void AMDetectorManagerTriggerAction::updateTriggered()
{
	setTriggered( detectorsTriggered_ && detectorManagersTriggered_ );
}

void AMDetectorManagerTriggerAction::onArmActionStarted()
{
	setStarted();
}

void AMDetectorManagerTriggerAction::onArmActionCancelled()
{
	setCancelled();
}

void AMDetectorManagerTriggerAction::onArmActionFailed()
{
	QString message = QString("Failed to trigger detector manager %1").arg(detectorManager_->name());
	AMErrorMon::alert(this, AMDETECTORMANAGERTRIGGERACTION_TRIGGER_FAILED, message);
	setFailed(message);
}

void AMDetectorManagerTriggerAction::onArmActionSucceeded()
{
	// Set up mappings for each detector and detector manager.

	foreach (AMDetector *detector, detectors_) {
		detectorTriggeringMapper_->setMapping(detector, detector);
		connect( detector, SIGNAL(acquisitionSucceeded()), detectorTriggeringMapper_, SLOT(map()) );
	}

	foreach (AMDetectorManager *detectorManager, detectorManagers_) {
		detectorManagerTriggeringMapper_->setMapping(detectorManager, detectorManager);
		connect( detectorManager, SIGNAL(triggered()), detectorManagerTriggeringMapper_, SLOT(map()) );
	}

	// Trigger each detector and detector manager.

	foreach (AMDetector *detector, detectors_)
		detector->acquire(AMDetectorDefinitions::ReadMode(detectorManagerTriggerInfo()->readMode()));

	foreach (AMDetectorManager *manager, detectorManagers_)
		manager->trigger(AMDetectorDefinitions::ReadMode(detectorManagerTriggerInfo()->readMode()));
}

void AMDetectorManagerTriggerAction::onDetectorTriggered(QObject *detectorObject)
{
	AMDetector *detector = qobject_cast<AMDetector*>(detectorObject);

	if (detector && !triggeredDetectors_.contains(detector))
		triggeredDetectors_.append(detector);

	if (triggeredDetectors_.count() == detectors_.count())
		detectorsTriggered_ = true;

	updateTriggered();
}

void AMDetectorManagerTriggerAction::onDetectorManagerTriggered(QObject *managerObject)
{
	AMDetectorManager *manager = qobject_cast<AMDetectorManager*>(managerObject);

	if (manager && !triggeredDetectorManagers_.contains(manager))
		triggeredDetectorManagers_.append(manager);

	if (triggeredDetectorManagers_.count() == detectorManagers_.count())
		detectorManagersTriggered_ = true;

	updateTriggered();
}

bool AMDetectorManagerTriggerAction::supportedReadMode(int readMode) const
{
	bool result = false;

	if (readMode == AMDetectorDefinitions::SingleRead)
		result = true;
	else if (readMode == AMDetectorDefinitions::ContinuousRead)
		result = true;

	return result;
}

void AMDetectorManagerTriggerAction::startImplementation()
{
	// Must have a detector manager.

	if ( !(detectorManager_ && detectorManager_->isConnected()) ) {
		QString message = QString("Failed to trigger detector manager. The manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERTRIGGERACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	// The detector manager must be able to be triggered.

	if (!detectorManager_->canTrigger()) {
		QString message = QString("Failed to trigger detector manager %1. The manager provided cannot be triggered.");
		AMErrorMon::alert(this, AMDETECTORMANAGERTRIGGERACTION_MANAGER_CANNOT_TRIGGER, message);
		setFailed(message);
		return;
	}

	// Must have a valid read mode.

	if (!supportedReadMode(detectorManagerTriggerInfo()->readMode())) {
		QString message = QString("Failed to trigger detector manager %1. The acquisition read mode provided is invalid or not supported.");
		AMErrorMon::alert(this, AMDETECTORMANAGERTRIGGERACTION_INVALID_READ_MODE, message);
		setFailed(message);
		return;
	}

	// Initialize lists of detectors and detector managers.

	detectors_ = detectorManager_->detectors();
	detectorManagers_ = detectorManager_->detectorManagers();

	// Create and execute arm action.

	AMAction3 *armAction = detectorManager_->createArmAction();

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
