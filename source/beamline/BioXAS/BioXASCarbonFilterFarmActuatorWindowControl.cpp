#include "BioXASCarbonFilterFarmActuatorWindowControl.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(AMControl *positionControl, AMControl *statusControl, QObject *parent) :
	BioXASCarbonFilterFarmActuatorControl("Carbon Filter Farm Actuator Window", "", parent)
{
	// Initialize inherited variables.

	value_ = Invalid;
	setpoint_ = Invalid;

	setEnumStates(QStringList() << windowToString(Invalid) << windowToString(None) << windowToString(Bottom) << windowToString(Top));
	setMoveEnumStates(QStringList() << windowToString(None) << windowToString(Bottom) << windowToString(Top));
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm Actuator Control");

	// Current settings.

	setPositionControl(positionControl);
	setStatusControl(statusControl);

	updateStates();
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}

double BioXASCarbonFilterFarmActuatorWindowControl::minimumValue() const
{
	return Invalid;
}

double BioXASCarbonFilterFarmActuatorWindowControl::maximumValue() const
{
	return Top;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = position_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::validValue(double value) const
{
	bool result = false;

	switch ((int)value) {
	case None:
		result = true;
		break;
	case Bottom:
		result = true;
		break;
	case Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::validSetpoint(double value) const
{
	bool result = false;

	switch ((int)value) {
	case None:
		result = true;
		break;
	case Bottom:
		result = true;
		break;
	case Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

QString BioXASCarbonFilterFarmActuatorWindowControl::windowToString(Window window)
{
	QString result = QString();

	switch ((int)value) {
	case None:
		result = "None";
		break;
	case Bottom:
		result = "Bottom";
		break;
	case Top:
		result = "Top";
		break;
	default:
		result = "Invalid";
		break;
	}

	return result;
}

BioXASCarbonFilterFarmActuatorWindowControl::Window BioXASCarbonFilterFarmActuatorWindowControl::windowAtPosition(double position) const
{
	return positionMap_.key(position, Invalid);
}

double BioXASCarbonFilterFarmActuatorWindowControl::positionOfWindow(Window window) const
{
	return positionMap_.value(window, 0);
}

void BioXASCarbonFilterFarmActuatorWindowControl::setWindowPosition(Window window, double position)
{
	positionMap_.insert(window, position);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateConnected()
{
	bool isConnected = (
				position_ && position_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateValue()
{
	if (isConnected()) {
		setValue( windowAtPosition(position_->value()) );

	} else {
		setValue(Invalid);
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving(position_->isMoving());
	}
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowControl::createMoveAction(double windowSetpoint)
{
	AMListAction3 *action = 0;

	Window window = Window(windowSetpoint);

	if (positionMap_.contains(window)) {
		action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

		AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, positionOfWindow(window));
		action->addSubAction(move);

		AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, BioXASCarbonFilterFarm::Position::Valid, TIMEOUT_MOVE);
		action->addSubAction(check);
	}

	return action;
}
