#include "BioXASValvesControl.h"

BioXASValvesControl::BioXASValvesControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = Open;
	maximumValue_ = None;

	setEnumStates(QStringList() << valueToString(Open) << valueToString(Closed) << valueToString(None));
	setMoveEnumStates(QStringList() << valueToString(Open));

	setContextKnownDescription("ValvesControl");
}

BioXASValvesControl::~BioXASValvesControl()
{

}

bool BioXASValvesControl::validValue(double value) const
{
	bool result = false;

	switch (int(value)) {
	case None:
		result = true;
		break;
	case Open:
		result = true;
		break;
	case Closed:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASValvesControl::validSetpoint(double value) const
{
	bool result = false;

	// Only want to be able to use this control to view current
	// state, and to open valves (not close them).

	if (value == BioXASValvesControl::Open)
		result = true;

	return result;
}

QString BioXASValvesControl::valueToString(BioXASValvesControl::Value value)
{
	QString result;

	switch (value) {
	case Open:
		result = "Open";
		break;
	case Closed:
		result = "Closed";
		break;
	case None:
		result = "None";
		break;
	default:
		break;
	}

	return result;
}

void BioXASValvesControl::updateValue()
{
	Value newValue = None;

	if (isOpen())
		newValue = Open;
	else if (isClosed())
		newValue = Closed;

	setValue(newValue);
}

AMAction3* BioXASValvesControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if ((int)setpoint == Open)
		result = createOpenValvesAction();

	return result;
}
