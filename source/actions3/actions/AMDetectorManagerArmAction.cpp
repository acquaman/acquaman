#include "AMDetectorManagerArmAction.h"
#include "beamline/AMBeamline.h"
#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMDetectorManager.h"
#include "util/AMErrorMonitor.h"

AMDetectorManagerArmAction::AMDetectorManagerArmAction(AMDetectorManagerArmActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	// Could potentially add a timeout?

	// Initialize class variables.

	detectorManager_ = 0;

	armed_ = false;
	detectorsArmed_ = false;
	detectorManagersArmed_ = false;

	detectorArmingMapper_ = new QSignalMapper(this);
	connect( detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)) );

	detectorManagerArmingMapper_ = new QSignalMapper(this);
	connect( detectorManagerArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorManagerArmed(QObject*)) );

	// Current settings.

	setDetectorManager(AMBeamline::bl()->exposedControlByInfo(detectorManagerArmInfo()->detectorManagerInfo()));
}

AMDetectorManagerArmAction::AMDetectorManagerArmAction(const AMDetectorManagerArmAction &original) :
	AMAction3(original)
{
	// Initialize class variables.

	detectorManager_ = 0;

	armed_ = false;
	detectorsArmed_ = false;
	detectorManagersArmed_ = false;

	detectorArmingMapper_ = new QSignalMapper(this);
	connect( detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)) );

	detectorManagerArmingMapper_ = new QSignalMapper(this);
	connect( detectorManagerArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorManagerArmed(QObject*)) );

	// Current settings.

	setDetectorManager(AMBeamline::bl()->exposedControlByInfo(detectorManagerArmInfo()->detectorManagerInfo()));
}

AMDetectorManagerArmAction::~AMDetectorManagerArmAction()
{

}

void AMDetectorManagerArmAction::setDetectorManager(AMControl *newControl)
{
	if (detectorManager_ != newControl) {

		if (newControl) {
			AMDetectorManager *newManager = qobject_cast<AMDetectorManager*>(newControl);

			if (newManager)
				detectorManager_ = newManager;

		} else {
			detectorManager_ = 0;
		}
	}
}

void AMDetectorManagerArmAction::setArmed(bool isArmed)
{
	if (armed_ != isArmed) {
		armed_ = isArmed;

		if (armed_)
			setSucceeded();
	}
}

void AMDetectorManagerArmAction::updateArmed()
{
	setArmed(detectorsArmed_ && detectorManagersArmed_);
}

void AMDetectorManagerArmAction::onDetectorArmed(QObject *detectorObject) // this is virtually identical to the managerarmed slot below. simplify?
{
	AMDetector *detector = qobject_cast<AMDetector*>(detectorObject);

	if (detector && !armedDetectors_.contains(detector))
		armedDetectors_.append(detector);

	if (armedDetectors_.count() == detectors_.count())
		detectorsArmed_ = true;

	updateArmed();
}

void AMDetectorManagerArmAction::onDetectorManagerArmed(QObject *managerObject)
{
	AMDetectorManager *manager = qobject_cast<AMDetectorManager*>(managerObject);

	if (manager && !armedDetectorManagers_.contains(manager))
		armedDetectorManagers_.append(manager);

	if (armedDetectorManagers_.count() == detectorManagers_.count())
		detectorManagersArmed_ = true;

	updateArmed();
}

void AMDetectorManagerArmAction::startImplementation()
{
	// Must have a detector manager.

	if ( !(detectorManager_ && detectorManager_->isConnected()) ) {
		QString message = QString("Failed to arm detector manager. The manager provided is invalid or not connected.");
		AMErrorMon::alert(this, AMDETECTORMANAGERARMACTION_INVALID_MANAGER, message);
		setFailed(message);
		return;
	}

	// The detector manager must be able to be armed.

	if (!detectorManager_->canArm()) {
		QString message = QString("Failed to arm detector manager %1. The manager provided cannot be armed.");
		AMErrorMon::alert(this, AMDETECTORMANAGERARMACTION_MANAGER_CANNOT_ARM, message);
		setFailed(message);
		return;
	}

	// Initialize lists of detectors and detector managers.

	detectors_ = detectorManager_->detectors();
	detectorManagers_ = detectorManager_->detectorManagers();

	// Set up mappings for each detector and detector manager.

	foreach (AMDetector *detector, detectors_) {
		detectorArmingMapper_->setMapping(detector, detector);
		connect( detector, SIGNAL(armed()), detectorArmingMapper_, SLOT(map()) );
	}

	foreach (AMDetectorManager *detectorManager, detectorManagers_) {
		detectorManagerArmingMapper_->setMapping(detectorManager, detectorManager);
		connect( detectorManager, SIGNAL(armedChanged(bool)), detectorManagerArmingMapper_, SLOT(map()) );
	}

	// Set the action as started.

	setStarted();

	// Arm each detector and detector manager.

	foreach (AMDetector *detector, detectors_)
		detector->arm();

	foreach (AMDetectorManager *manager, detectorManagers_)
		manager->arm();
}
