#include "BioXASBeamStatusControl.h"

BioXASBeamStatusControl::BioXASBeamStatusControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = None;
	maximumValue_ = Off;

	setEnumStates(QStringList() << "On" << "Off" << "None");
	setMoveEnumStates(QStringList() << "On" << "Off");
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("BeamStatus");
}

BioXASBeamStatusControl::~BioXASBeamStatusControl()
{

}

bool BioXASBeamStatusControl::validValue(double value) const
{
	bool result = false;

	switch (int(value)) {
	case On:
		result = true;
		break;
	case Off:
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

bool BioXASBeamStatusControl::validSetpoint(double value) const
{
	bool result = false;

	switch (int(value)) {
	case On:
		result = true;
		break;
	case Off:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

void BioXASBeamStatusControl::updateValue()
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

AMAction3* BioXASBeamStatusControl::createMoveAction(double newState)
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
