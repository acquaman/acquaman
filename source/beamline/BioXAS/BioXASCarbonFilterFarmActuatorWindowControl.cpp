#include "BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, const QString &units, BioXASCarbonFilterFarmActuatorControl *actuatorControl, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, units, parent)
{
	// Initialize local variables.

	actuator_ = 0;

	// Initialize inherited variables.

	value_ = BioXASCarbonFilterFarm::Actuator::Invalid;
	setpoint_ = BioXASCarbonFilterFarm::Actuator::Invalid;
	minimumValue_ = BioXASCarbonFilterFarm::Actuator::Invalid;
	maximumValue_ = BioXASCarbonFilterFarm::Actuator::Top;

	setEnumStates(QStringList() << windowToString(BioXASCarbonFilterFarm::Actuator::Invalid) << windowToString(BioXASCarbonFilterFarm::Actuator::None) << windowToString(BioXASCarbonFilterFarm::Actuator::Bottom) << windowToString(BioXASCarbonFilterFarm::Actuator::Top));
	setMoveEnumStates(QStringList() << windowToString(BioXASCarbonFilterFarm::Actuator::None) << windowToString(BioXASCarbonFilterFarm::Actuator::Bottom) << windowToString(BioXASCarbonFilterFarm::Actuator::Top));

	setContextKnownDescription("Actuator Window Control");

	// Current settings.

	setActuatorControl(actuatorControl);
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = actuator_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = actuator_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = actuator_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::validValue(double value) const
{
	bool result = false;

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Actuator::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Actuator::Bottom:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Actuator::Top:
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
	case BioXASCarbonFilterFarm::Actuator::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Actuator::Bottom:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Actuator::Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

QString BioXASCarbonFilterFarmActuatorWindowControl::windowToString(BioXASCarbonFilterFarm::Actuator::Window window)
{
	QString result = QString();

	switch (int(window)) {
	case BioXASCarbonFilterFarm::Actuator::None:
		result = "None";
		break;
	case BioXASCarbonFilterFarm::Actuator::Bottom:
		result = "Bottom";
		break;
	case BioXASCarbonFilterFarm::Actuator::Top:
		result = "Top";
		break;
	default:
		result = "Invalid";
		break;
	}

	return result;
}

BioXASCarbonFilterFarm::Actuator::Window BioXASCarbonFilterFarmActuatorWindowControl::windowAtPosition(double position) const
{
	return positionMap_.key(position, BioXASCarbonFilterFarm::Actuator::Invalid);
}

double BioXASCarbonFilterFarmActuatorWindowControl::positionOfWindow(BioXASCarbonFilterFarm::Actuator::Window window) const
{
	return positionMap_.value(window, 0);
}

void BioXASCarbonFilterFarmActuatorWindowControl::setActuatorControl(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (actuator_ != newControl) {

		if (actuator_)
			removeChildControl(actuator_);

		actuator_ = newControl;

		if (actuator_)
			addChildControl(actuator_);

		emit actuatorControlChanged(actuator_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::setWindowPosition(BioXASCarbonFilterFarm::Actuator::Window window, double position)
{
	positionMap_.insert(window, position);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateConnected()
{
	bool isConnected = ( actuator_ && actuator_->isConnected() );

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateValue()
{
	double newValue = BioXASCarbonFilterFarm::Actuator::Invalid;

	if (isConnected())
		newValue = windowAtPosition(actuator_->value());

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = actuator_->isMoving();

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowControl::createMoveAction(double windowSetpoint)
{
	AMAction3 *action = 0;

	BioXASCarbonFilterFarm::Actuator::Window window = BioXASCarbonFilterFarm::Actuator::Window(windowSetpoint);

	if (positionMap_.contains(window))
		action = AMActionSupport::buildControlMoveAction(actuator_, positionOfWindow(window));

	return action;
}
