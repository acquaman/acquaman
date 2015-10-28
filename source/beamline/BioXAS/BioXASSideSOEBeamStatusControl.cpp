#include "BioXASSideSOEBeamStatusControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASSideSOEBeamStatusControl::BioXASSideSOEBeamStatusControl(const QString &name, QObject *parent) :
	BioXASBeamStatusControl(name, parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("SideSOEBeamStatus");

	// Initialize local variables.

	poeBeamStatus_ = 0;
	endstationShutter_ = 0;

	// Current settings.

	updateStates();
}

BioXASSideSOEBeamStatusControl::~BioXASSideSOEBeamStatusControl()
{

}

bool BioXASSideSOEBeamStatusControl::shouldStop() const
{
	bool result = false;

	if (isConnected())
		result = (poeBeamStatus_->shouldStop() && endstationShutter_->shouldStop());

	return result;
}

bool BioXASSideSOEBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (poeBeamStatus_->canMeasure() && endstationShutter_->canMeasure());

	return result;
}

bool BioXASSideSOEBeamStatusControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (poeBeamStatus_->canMove() && endstationShutter_->canMove());

	return result;
}

bool BioXASSideSOEBeamStatusControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (poeBeamStatus_->canStop() && endstationShutter_->canStop());

	return result;
}

bool BioXASSideSOEBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected())
		result = (poeBeamStatus_->isOn() && endstationShutter_->isOpen());

	return result;
}

void BioXASSideSOEBeamStatusControl::setPOEBeamStatus(BioXASSidePOEBeamStatusControl *newControl)
{
	if (poeBeamStatus_ != newControl) {

		if (poeBeamStatus_)
			removeChildControl(poeBeamStatus_);

		poeBeamStatus_ = newControl;

		if (poeBeamStatus_)
			addChildControl(poeBeamStatus_);

		updateStates();

		emit poeBeamStatusChanged(poeBeamStatus_);
	}
}

void BioXASSideSOEBeamStatusControl::setEndstationShutter(CLSBiStateControl *newControl)
{
	if (endstationShutter_ != newControl) {

		if (endstationShutter_)
			removeChildControl(endstationShutter_);

		endstationShutter_ = newControl;

		if (endstationShutter_)
			addChildControl(endstationShutter_);

		updateStates();

		emit endstationShutterChanged(endstationShutter_);
	}
}

void BioXASSideSOEBeamStatusControl::updateConnected()
{
	bool isConnected = (
				poeBeamStatus_ && poeBeamStatus_->isConnected() &&
				endstationShutter_ && endstationShutter_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASSideSOEBeamStatusControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = ( poeBeamStatus_->isMoving() || endstationShutter_->isMoving() );

	setIsMoving(isMoving);
}

AMAction3* BioXASSideSOEBeamStatusControl::createBeamOnAction()
{
	AMListAction3 *beamOn = new AMListAction3(new AMListActionInfo3("Turn beam on", "Turn beam on"), AMListAction3::Sequential);
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(poeBeamStatus_, BioXAS::Beam::On));
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(endstationShutter_, CLSBiStateControl::Open));

	return beamOn;
}

AMAction3* BioXASSideSOEBeamStatusControl::createBeamOffAction()
{
	AMListAction3 *beamOff = new AMListAction3(new AMListActionInfo3("Turn beam off", "Turn beam off"), AMListAction3::Sequential);
	beamOff->addSubAction(AMActionSupport::buildControlMoveAction(endstationShutter_, CLSBiStateControl::Closed));

	return beamOff;
}
