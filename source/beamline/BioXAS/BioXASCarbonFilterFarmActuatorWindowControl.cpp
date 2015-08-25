#include "BioXASCarbonFilterFarmActuatorWindowControl.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, const QString &units, AMControl *positionControl, AMControl *statusControl, QObject *parent) :
	BioXASCarbonFilterFarmActuatorControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = BioXASCarbonFilterFarm::Actuator::Invalid;
	setpoint_ = BioXASCarbonFilterFarm::Actuator::Invalid;
	minimumValue_ = BioXASCarbonFilterFarm::Actuator::Invalid;
	maximumValue_ = BioXASCarbonFilterFarm::Actuator::Top;

	setEnumStates(QStringList() << windowToString(Invalid) << windowToString(None) << windowToString(Bottom) << windowToString(Top));
	setMoveEnumStates(QStringList() << windowToString(None) << windowToString(Bottom) << windowToString(Top));

	setContextKnownDescription("Filter Farm Actuator Window Control");

	// Current settings.

	setPositionControl(positionControl);
	setStatusControl(statusControl);

	updateStates();
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

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

	switch ((int)value) {
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
	return positionMap_.key(position, Invalid);
}

double BioXASCarbonFilterFarmActuatorWindowControl::positionOfWindow(BioXASCarbonFilterFarm::Actuator::Window window) const
{
	return positionMap_.value(window, 0);
}

void BioXASCarbonFilterFarmActuatorWindowControl::setWindowPosition(BioXASCarbonFilterFarm::Actuator::Window window, double position)
{
	positionMap_.insert(window, position);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateValue()
{
	if (isConnected()) {
		setValue( windowAtPosition(position_->value()) );

	} else {
		setValue(BioXASCarbonFilterFarm::Actuator::Invalid);
	}
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowControl::createMoveAction(double windowSetpoint)
{
	AMListAction3 *action = 0;

	BioXASCarbonFilterFarm::Actuator::Window window = BioXASCarbonFilterFarm::Actuator::Window(windowSetpoint);

	if (positionMap_.contains(window)) {
		action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

		AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, positionOfWindow(window));
		action->addSubAction(move);

		AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, BioXASCarbonFilterFarm::Actuator::InPosition, TIMEOUT_MOVE);
		action->addSubAction(check);
	}

	return action;
}
