#include "BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize local variables.

	actuatorPosition_ = 0;

	// Initialize inherited variables.

	value_ = Invalid;
	setpoint_ = Invalid;
	minimumValue_ = Top;
	maximumValue_ = Invalid;

	setEnumStates(QStringList() << windowToString(Top) << windowToString(Bottom) << windowToString(None) << windowToString(Invalid));
	setMoveEnumStates(QStringList() << windowToString(Top) << windowToString(Bottom) << windowToString(None));

	setContextKnownDescription("Actuator Window Control");
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = actuatorPosition_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = actuatorPosition_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = actuatorPosition_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::validValue(double value) const
{
	bool result = false;

	switch ((int)value) {
	case Top:
		result = true;
		break;
	case Bottom:
		result = true;
		break;
	case None:
		result = true;
		break;
	case Invalid:
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
	case Top:
		result = true;
		break;
	case Bottom:
		result = true;
		break;
	case None:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

QString BioXASCarbonFilterFarmActuatorWindowControl::windowToString(double window)
{
	QString result = "Unknown";

	switch (int(window)) {
	case Top:
		result = "Top";
		break;
	case Bottom:
		result = "Bottom";
		break;
	case None:
		result = "None";
		break;
	case Invalid:
		result = "Invalid";
		break;
	default:
		break;
	}

	return result;
}

double BioXASCarbonFilterFarmActuatorWindowControl::windowAtPosition(double position) const
{
	double result = -1;

	if (position >= windowMinPositionMap_.value(Top) && position <= windowMaxPositionMap_.value(Top))
		result = Top;
	else if (position >= windowMinPositionMap_.value(Bottom) && position <= windowMaxPositionMap_.value(Bottom))
		result = Bottom;
	else if (position >= windowMinPositionMap_.value(None) && position <= windowMaxPositionMap_.value(None))
		result = None;
	else
		result = Invalid;

	return result;
}

double BioXASCarbonFilterFarmActuatorWindowControl::positionOfWindow(double window) const
{
	double positionMin = windowMinPositionMap_.value(window);
	double positionMax = windowMaxPositionMap_.value(window);

	double position = (positionMin + positionMax) / 2.0;

	return position;
}

void BioXASCarbonFilterFarmActuatorWindowControl::setActuatorPosition(AMControl *newControl)
{
	if (actuatorPosition_ != newControl) {

		if (actuatorPosition_)
			removeChildControl(actuatorPosition_);

		actuatorPosition_ = newControl;

		if (actuatorPosition_)
			addChildControl(actuatorPosition_);

		updateStates();

		emit actuatorPositionChanged(actuatorPosition_);
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::setWindowPosition(double window, double minPosition, double maxPosition)
{
	if (validSetpoint(window) && minPosition <= maxPosition) {
		windowMinPositionMap_.insert(window, minPosition);
		windowMaxPositionMap_.insert(window, maxPosition);
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateConnected()
{
	bool isConnected = ( actuatorPosition_ && actuatorPosition_->isConnected() );

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateValue()
{
	double newValue = BioXASCarbonFilterFarmActuatorWindowControl::Invalid;

	if (isConnected())
		newValue = windowAtPosition(actuatorPosition_->value());

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = actuatorPosition_->isMoving();

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowControl::createMoveAction(double windowSetpoint)
{
	AMAction3 *action = AMActionSupport::buildControlMoveAction(actuatorPosition_, positionOfWindow(windowSetpoint));
	return action;
}
