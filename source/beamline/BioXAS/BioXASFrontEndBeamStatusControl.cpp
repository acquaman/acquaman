#include "BioXASFrontEndBeamStatusControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASFrontEndBeamStatusControl::BioXASFrontEndBeamStatusControl(QObject *parent) :
	BioXASBeamStatusControl("BioXASFrontEndBeamStatus", parent)
{
	// Initialize local variables.

	photonShutterUpstream_ = 0;
	vacuumValve_ = 0;
	fastValve_ = 0;
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

bool BioXASFrontEndBeamStatusControl::shouldStop() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterDownstream_->shouldStop() && safetyShutter_->shouldStop() && valves_->shouldStop());

	return result;
}

bool BioXASFrontEndBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterUpstream_->canMeasure() && vacuumValve_->canMeasure() && fastValve_->canMeasure() && photonShutterDownstream_->canMeasure() && safetyShutter_->canMeasure() && valves_->canMeasure());

	return result;
}

bool BioXASFrontEndBeamStatusControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterDownstream_->canMove() && safetyShutter_->canMove() && valves_->canMove());

	return result;
}

bool BioXASFrontEndBeamStatusControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterDownstream_->canStop() && safetyShutter_->canStop() && valves_->canStop());

	return result;
}

bool BioXASFrontEndBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected()) {
		result = (valves_->isOpen() && safetyShutter_->isOpen() && photonShutterDownstream_->isOpen() && (fastValve_->value() == 1) && (vacuumValve_->value() == 1) && (photonShutterUpstream_->value() == 1));
	}

	return result;
}

void BioXASFrontEndBeamStatusControl::setPhotonShutterUpstream(AMReadOnlyPVControl *newControl)
{
	qDebug() << "\n\nBioXASFrontEndBeamStatusControl: setting photon shutter upstream control.";

	if (photonShutterUpstream_ != newControl) {

		qDebug() << "Control is a new control.";

		if (photonShutterUpstream_)
			removeChildControl(photonShutterUpstream_);

		photonShutterUpstream_ = newControl;

		if (photonShutterUpstream_)
			addChildControl(photonShutterUpstream_);

		emit photonShutterUpstreamChanged(photonShutterUpstream_);
	}
}

void BioXASFrontEndBeamStatusControl::setVacuumValve(AMReadOnlyPVControl *newControl)
{
	if (vacuumValve_ != newControl) {

		if (vacuumValve_)
			removeChildControl(vacuumValve_);

		vacuumValve_ = newControl;

		if (vacuumValve_)
			addChildControl(vacuumValve_);

		emit vacuumValveChanged(vacuumValve_);
	}
}

void BioXASFrontEndBeamStatusControl::setFastValve(AMReadOnlyPVControl *newControl)
{
	if (fastValve_ != newControl) {

		if (fastValve_)
			removeChildControl(fastValve_);

		fastValve_ = newControl;

		if (fastValve_)
			addChildControl(fastValve_);

		emit fastValveChanged(fastValve_);
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
				vacuumValve_ && vacuumValve_->isConnected() &&
				fastValve_ && fastValve_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	if (isConnected)
		qDebug() << "\n\nFront end beam status control is connected.";
	else
		qDebug() << "\n\nFront end beam status control is NOT connected:\n" << toString();

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
					vacuumValve_->isMoving() ||
					fastValve_->isMoving() ||
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

	// The first actions are to check the front-most shutters and valves.
	// We don't actually have control over these, so these are wait actions to achieve a desired value.

	beamOn->addSubAction(AMActionSupport::buildControlWaitAction(fastValve_, 1));
	beamOn->addSubAction(AMActionSupport::buildControlWaitAction(vacuumValve_, 1));
	beamOn->addSubAction(AMActionSupport::buildControlWaitAction(photonShutterUpstream_, 1));

	// The next steps are for shutters and valves that we do have control over.
	// All valves and safety shutters need to be open in order to open the downstream photon shutter.

	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(valves_, BioXASValvesControl::Open));
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter_, CLSBiStateControl::Open));
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(photonShutterDownstream_, CLSBiStateControl::Open));


	return beamOn;
}

AMAction3* BioXASFrontEndBeamStatusControl::createBeamOffAction()
{
	AMListAction3 *beamOff = new AMListAction3(new AMListActionInfo3("Turn beam off", "Turn beam off"), AMListAction3::Sequential);
	beamOff->addSubAction(AMActionSupport::buildControlMoveAction(photonShutterDownstream_, CLSBiStateControl::Closed));
	beamOff->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter_, CLSBiStateControl::Closed));

	return beamOff;
}
