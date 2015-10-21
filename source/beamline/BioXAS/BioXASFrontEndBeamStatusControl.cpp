#include "BioXASFrontEndBeamStatusControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASFrontEndBeamStatusControl::BioXASFrontEndBeamStatusControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = BioXAS::Beam::None;
	setpoint_ = BioXAS::Beam::None;
	minimumValue_ = BioXAS::Beam::Off;
	maximumValue_ = BioXAS::Beam::None;

	setEnumStates(QStringList() << valueToString(BioXAS::Beam::Off) << valueToString(BioXAS::Beam::On) << valueToString(BioXAS::Beam::None));
	setMoveEnumStates(QStringList() << valueToString(BioXAS::Beam::Off) << valueToString(BioXAS::Beam::On));
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("FrontEndBeamStatus");

	// Initialize local variables.

	shutters_ = 0;
	valves_ = 0;

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
		result = (shutters_->shouldStop() && valves_->shouldStop());

	return result;
}

bool BioXASFrontEndBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (shutters_->canMeasure() && valves_->canMeasure());

	return result;
}

bool BioXASFrontEndBeamStatusControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (shutters_->canMove() && valves_->canMove());

	return result;
}

bool BioXASFrontEndBeamStatusControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (shutters_->canStop() && valves_->canStop());

	return result;
}

bool BioXASFrontEndBeamStatusControl::validValue(double value) const
{
	bool result = false;

	switch (int(value)) {
	case BioXAS::Beam::Off:
		result = true;
		break;
	case BioXAS::Beam::On:
		result = true;
		break;
	case BioXAS::Beam::None:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASFrontEndBeamStatusControl::validSetpoint(double value) const
{
	bool result = false;

	switch (int(value)) {
	case BioXAS::Beam::Off:
		result = true;
		break;
	case BioXAS::Beam::On:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASFrontEndBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected()) {
		result = (shutters_->isOpen() && valves_->isOpen());
	}

	return result;
}

QString BioXASFrontEndBeamStatusControl::valueToString(BioXAS::Beam::Status value) const
{
	QString result;

	switch (value) {
	case BioXAS::Beam::None:
		result = "None";
		break;
	case BioXAS::Beam::On:
		result = "On";
		break;
	case BioXAS::Beam::Off:
		result = "Off";
		break;
	default:
		break;
	}

	return result;
}

void BioXASFrontEndBeamStatusControl::setShutters(BioXASFrontEndShuttersControl *newControl)
{
	if (shutters_ != newControl) {

		if (shutters_)
			removeChildControl(shutters_);

		shutters_ = newControl;

		if (shutters_)
			addChildControl(shutters_);

		updateStates();

		emit shuttersChanged(shutters_);
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

		updateStates();

		emit valvesChanged(valves_);
	}
}

void BioXASFrontEndBeamStatusControl::updateConnected()
{
	bool isConnected = (
				shutters_ && shutters_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASFrontEndBeamStatusControl::updateValue()
{
	double value = BioXAS::Beam::None;

	if (isConnected()) {

		if (isOn())
			value = BioXAS::Beam::On;
		else if (isOff())
			value = BioXAS::Beam::Off;
		else
			value = BioXAS::Beam::None;
	}

	setValue(value);
}

void BioXASFrontEndBeamStatusControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					shutters_->isMoving() ||
					valves_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

AMAction3* BioXASFrontEndBeamStatusControl::createMoveAction(double newState)
{
	AMAction3 *result = 0;

	switch(int(newState)) {
	case BioXAS::Beam::On:
		result = createBeamOnAction();
		break;
	case BioXAS::Beam::Off:
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
	beamOn->addSubAction(AMActionSupport::buildControlMoveAction(shutters_, BioXASFrontEndShuttersControl::Open));

	return beamOn;
}

AMAction3* BioXASFrontEndBeamStatusControl::createBeamOffAction()
{
	AMListAction3 *beamOff = new AMListAction3(new AMListActionInfo3("Turn beam off", "Turn beam off"), AMListAction3::Sequential);
	beamOff->addSubAction(AMActionSupport::buildControlMoveAction(shutters_, BioXASFrontEndShuttersControl::Closed));

	return beamOff;
}
