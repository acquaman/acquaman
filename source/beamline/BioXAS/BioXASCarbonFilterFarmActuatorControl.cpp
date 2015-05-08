#include "BioXASCarbonFilterFarmActuatorControl.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(AMControl *positionControl, AMControl *statusControl, QObject *parent) :
	AMPseudoMotorControl("BioXASCarbonFilterFarmActuatorControl", QString(), parent, "BioXAS Carbon Filter Farm Actuator Control")
{
	// Initialize local variables.

	position_ = 0;
	status_ = 0;

//	position_ = new AMPVControl("Carbon Filter Farm Actuator Position Control", name+":mm:fbk", name+":mm", name+":stop", this, 0.001);
//	connect( position_, SIGNAL(valueChanged(double)), this, SLOT(updateWindow()) );
//	addChildControl(position_);

//	status_ = new AMReadOnlyPVControl("Carbon Filter Farm Actuator Status Control", name+":inPosition", this);
//	addChildControl(status_);

	// Initialize inherited variables.

	value_ = Window::Invalid;
	setpoint_ = Window::Invalid;

	setEnumStates(QStringList() << windowToString(Window::Invalid) << windowToString(Window::None) << windowToString(Window::Bottom) << windowToString(Window::Top));
	setMoveEnumStates(QStringList() << windowToString(Window::None) << windowToString(Window::Bottom) << windowToString(Window::Top));
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm Actuator Control");

	// Emit enumChanged signals when connected, value changes, and setpoint changes. This is to make sure the control is viewed as an enum.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Current settings.

	setPositionControl(positionControl);
	setStatusControl(statusControl);

	updateValue();
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

bool BioXASCarbonFilterFarmActuatorControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = position_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = position_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::validValue(double value) const
{
	bool result = false;

	switch ((int)value) {
	case Window::None:
		result = true;
		break;
	case Window::Bottom:
		result = true;
		break;
	case Window::Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::validSetpoint(double value) const
{
	bool result = false;

	switch ((int)value) {
	case Window::None:
		result = true;
		break;
	case Window::Bottom:
		result = true;
		break;
	case Window::Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

QString BioXASCarbonFilterFarmActuatorControl::windowToString(Window::Selection value)
{
	QString result = QString();

	switch (value) {
	case Window::None:
		result = "None";
		break;
	case Window::Bottom:
		result = "Bottom";
		break;
	case Window::Top:
		result = "Top";
		break;
	default:
		result = "Invalid";
		break;
	}

	return result;
}

BioXASCarbonFilterFarmActuatorControl::Window::Selection BioXASCarbonFilterFarmActuatorControl::stringToWindow(const QString &string)
{
	Window::Selection result = Window::Invalid;

	if (string == "None")
		result = Window::None;
	else if (string == "Bottom")
		result = Window::Bottom;
	else if (string == "Top")
		result = Window::Top;

	return result;
}

BioXASCarbonFilterFarmActuatorControl::Window::Selection BioXASCarbonFilterFarmActuatorControl::window(double index)
{
	Window::Selection result = Window::Invalid;

	switch ((int)index) {
	case Window::None:
		result = Window::None;
		break;
	case Window::Bottom:
		result = Window::Bottom;
		break;
	case Window::Top:
		result = Window::Top;
		break;
	default:
		break;
	}

	return result;
}

BioXASCarbonFilterFarmActuatorControl::Window::Selection BioXASCarbonFilterFarmActuatorControl::windowAtPosition(double position) const
{
	return positionMap_.key(position, Window::Invalid);
}

double BioXASCarbonFilterFarmActuatorControl::positionOfWindow(Window::Selection window) const
{
	return positionMap_.value(window, 0);
}

void BioXASCarbonFilterFarmActuatorControl::setWindowPosition(Window::Selection window, double position)
{
	positionMap_.insert(window, position);
}

void BioXASCarbonFilterFarmActuatorControl::setPositionControl(AMControl *newControl)
{
	if (position_ != newControl) {

		if (position_) {
			removeChildControl(position_);
		}

		position_ = newControl;

		if (position_) {
			addChildControl(position_);
		}
	}
}

void BioXASCarbonFilterFarmActuatorControl::setStatusControl(AMControl *newControl)
{
	if (status_ != newControl) {

		if (status_) {
			removeChildControl(status_);
		}

		status_ = newControl;

		if (status_) {
			addChildControl(status_);
		}
	}
}

void BioXASCarbonFilterFarmActuatorControl::updateConnected()
{
	bool isConnected = (
				position_ && position_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorControl::updateValue()
{
	if (isConnected()) {
		setValue( windowAtPosition(position_->value()) );

	} else {
		setValue(Window::Invalid);
	}
}

void BioXASCarbonFilterFarmActuatorControl::updateIsMoving()
{
	if (isConnected()) {
		setIsMoving(position_->isMoving());
	}
}

AMAction3* BioXASCarbonFilterFarmActuatorControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = 0;

	if (isConnected() && validSetpoint(setpoint)) {
		Window::Selection windowSetpoint = window(setpoint);

		if (positionMap_.contains(windowSetpoint)) {
			action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

			AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, positionOfWindow(windowSetpoint));
			action->addSubAction(move);

			AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, Position::Valid, TIMEOUT_MOVE);
			action->addSubAction(check);
		}
	}

	return action;
}
