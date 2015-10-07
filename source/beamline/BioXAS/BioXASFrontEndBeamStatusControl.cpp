#include "BioXASFrontEndBeamStatusControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASFrontEndBeamStatusControl::BioXASFrontEndBeamStatusControl(QObject *parent) :
	BioXASBeamStatusControl("BioXASFrontEndBeamStatus", parent)
{
	// Initialize local variables.

	photonShutterUpstream_ = 0;
	photonShutterDownstream_ = 0;
	safetyShutter_ = 0;
	valves_ = 0;

	// Set inherited settings.

	setContextKnownDescription("FrontEndBeamStatus");

	// Current settings.

	updateStates();
}

BioXASFrontEndBeamStatusControl::~BioXASFrontEndBeamStatusControl()
{

}

bool BioXASFrontEndBeamStatusControl::canMeasure() const
{
	return false;
}

bool BioXASFrontEndBeamStatusControl::canMove() const
{
	return false;
}

bool BioXASFrontEndBeamStatusControl::canStop() const
{
	return false;
}

bool BioXASFrontEndBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected()) {
		result = (valves_->isOpen() && safetyShutter_->isOpen() && photonShutterDownstream_->isOpen() && photonShutterUpstream_->isOpen());
	}

	return result;
}

bool BioXASFrontEndBeamStatusControl::isOff() const
{
	bool result = false;

	if (isConnected()) {
		result = (!valves_->isOpen() || !safetyShutter_->isOpen() || !photonShutterDownstream_->isOpen() || !photonShutterUpstream_->isOpen());
	}

	return result;
}

void BioXASFrontEndBeamStatusControl::setPhotonShutterUpstream(CLSBiStateControl *newControl)
{
	if (photonShutterUpstream_ != newControl) {

		if (photonShutterUpstream_)
			removeChildControl(photonShutterUpstream_);

		photonShutterUpstream_ = newControl;

		if (photonShutterUpstream_)
			addChildControl(photonShutterUpstream_);

		emit photonShutterUpstreamChanged(photonShutterUpstream_);
	}
}

void BioXASFrontEndBeamStatusControl::setPhotonShutterDownstream(CLSBiStateControl *newControl)
{
	if (photonShutterDownstream_ != newControl) {

		if (photonShutterDownstream_)
			removeChildControl(photonShutterDownstream_);

		photonShutterDownstream_ = newControl;

		if (photonShutterDownstream_)
			addChildControl(photonShutterDownstream_);

		emit photonShutterDownstreamChanged(photonShutterDownstream_);
	}
}

void BioXASFrontEndBeamStatusControl::setSafetyShutter(CLSBiStateControl *newControl)
{
	if (safetyShutter_ != newControl) {

		if (safetyShutter_)
			removeChildControl(safetyShutter_);

		safetyShutter_ = newControl;

		if (safetyShutter_)
			addChildControl(safetyShutter_);

		emit safetyShutterChanged(safetyShutter_);
	}
}

void BioXASFrontEndBeamStatusControl::setValves(BioXASValvesControl *newControl)
{
	if (valves_ != newControl) {

		if (valves_)
			removeChildControl(valves_);

		valves_ = newControl;

		if (valves_)
			addChildControl(valves_);
	}
}

void BioXASFrontEndBeamStatusControl::updateConnected()
{
	bool isConnected = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASFrontEndBeamStatusControl::updateValue()
{
	double value = None;

	if (isConnected()) {

		if (isOn())
			value = On;
		else if (isOff())
			value = Off;
		else
			value = None;
	}

	setValue(value);
}

void BioXASFrontEndBeamStatusControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					photonShutterUpstream_->isMoving() ||
					photonShutterDownstream_->isMoving() ||
					safetyShutter_->isMoving() ||
					valves_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

AMAction3* BioXASFrontEndBeamStatusControl::createMoveAction(double newState)
{
	AMAction3 *result = 0;

	switch(int(newState)) {
	case On:
		result = createBeamOnAction();
		break;
	case Off:
		result = createBeamOffAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* BioXASFrontEndBeamStatusControl::createBeamOnAction()
{
	AMListAction3 *beamOn = new AMListAction3(new AMListActionInfo3("Turn beam on", "Turn beam on"), AMListAction3::Sequential);
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(valves_, BioXASValvesControl::Open));
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter_, CLSBiStateControl::Open));
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(photonShutterDownstream_, CLSBiStateControl::Open));
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(photonShutterUpstream_, CLSBiStateControl::Open));

	return beamOn;
}

AMAction3* BioXASFrontEndBeamStatusControl::createBeamOffAction()
{
	AMAction3 *beamOff = AMActionSupport::buildControlMoveAction(photonShutterDownstream_, CLSBiStateControl::Closed);
	return beamOff;
}
