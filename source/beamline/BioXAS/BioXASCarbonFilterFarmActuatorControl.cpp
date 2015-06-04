#include "BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(AMControl *positionControl, AMControl *statusControl, QObject *parent) :
	AMPseudoMotorControl("BioXASCarbonFilterFarmActuatorControl", QString(), parent, "BioXAS Carbon Filter Farm Actuator Control")
{
	// Initialize local variables.

	position_ = 0;
	status_ = 0;

	// Initialize inherited variables.

	value_ = BioXASCarbonFilterFarm::Window::Invalid;
	setpoint_ = BioXASCarbonFilterFarm::Window::Invalid;

	setEnumStates(QStringList() << windowToString(BioXASCarbonFilterFarm::Window::Invalid) << windowToString(BioXASCarbonFilterFarm::Window::None) << windowToString(BioXASCarbonFilterFarm::Window::Bottom) << windowToString(BioXASCarbonFilterFarm::Window::Top));
	setMoveEnumStates(QStringList() << windowToString(BioXASCarbonFilterFarm::Window::None) << windowToString(BioXASCarbonFilterFarm::Window::Bottom) << windowToString(BioXASCarbonFilterFarm::Window::Top));
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm Actuator Control");

	// Emit enumChanged signals when connected, value changes, and setpoint changes. This is to make sure the control is viewed as an enum.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Current settings.

	setPositionControl(positionControl);
	setStatusControl(statusControl);

	updateStates();
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

double BioXASCarbonFilterFarmActuatorControl::minimumValue() const
{
	return BioXASCarbonFilterFarm::Window::Invalid;
}

double BioXASCarbonFilterFarmActuatorControl::maximumValue() const
{
	return BioXASCarbonFilterFarm::Window::Top;
}

bool BioXASCarbonFilterFarmActuatorControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMeasure();

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
	case BioXASCarbonFilterFarm::Window::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Window::Bottom:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Window::Top:
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
	case BioXASCarbonFilterFarm::Window::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Window::Bottom:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Window::Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

QString BioXASCarbonFilterFarmActuatorControl::windowToString(double value)
{
	QString result = QString();

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Window::None:
		result = "None";
		break;
	case BioXASCarbonFilterFarm::Window::Bottom:
		result = "Bottom";
		break;
	case BioXASCarbonFilterFarm::Window::Top:
		result = "Top";
		break;
	default:
		result = "Invalid";
		break;
	}

	return result;
}

double BioXASCarbonFilterFarmActuatorControl::stringToWindow(const QString &string)
{
	double result = BioXASCarbonFilterFarm::Window::Invalid;

	if (string == "None")
		result = BioXASCarbonFilterFarm::Window::None;
	else if (string == "Bottom")
		result = BioXASCarbonFilterFarm::Window::Bottom;
	else if (string == "Top")
		result = BioXASCarbonFilterFarm::Window::Top;

	return result;
}

double BioXASCarbonFilterFarmActuatorControl::windowAtPosition(double position) const
{
	return positionMap_.key(position, BioXASCarbonFilterFarm::Window::Invalid);
}

double BioXASCarbonFilterFarmActuatorControl::positionOfWindow(double window) const
{
	return positionMap_.value(window, 0);
}

void BioXASCarbonFilterFarmActuatorControl::setWindowPosition(double window, double position)
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
		setValue(BioXASCarbonFilterFarm::Window::Invalid);
	}
}

void BioXASCarbonFilterFarmActuatorControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving(position_->isMoving());
	}
}

AMAction3* BioXASCarbonFilterFarmActuatorControl::createMoveAction(double windowSetpoint)
{
	AMListAction3 *action = 0;

	if (isConnected() && validSetpoint(windowSetpoint)) {

		if (positionMap_.contains(windowSetpoint)) {
			action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

			AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, positionOfWindow(windowSetpoint));
			action->addSubAction(move);

			AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, BioXASCarbonFilterFarm::Position::Valid, TIMEOUT_MOVE);
			action->addSubAction(check);
		}
	}

	return action;
}
