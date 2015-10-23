#include "BioXASBeamStatusControl.h"

BioXASBeamStatusControl::BioXASBeamStatusControl(const QString &name, QObject *parent) :
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
	setContextKnownDescription("BeamStatus");
}

BioXASBeamStatusControl::~BioXASBeamStatusControl()
{

}

bool BioXASBeamStatusControl::validValue(double value) const
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

bool BioXASBeamStatusControl::validSetpoint(double value) const
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

QString BioXASBeamStatusControl::valueToString(BioXAS::Beam::Status value) const
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

void BioXASBeamStatusControl::updateValue()
{
	double value = BioXAS::Beam::None;

	if (isConnected()) {
		if (isOn())
			value = BioXAS::Beam::On;
		else
			value = BioXAS::Beam::Off;
	}

	setValue(value);
}

AMAction3* BioXASBeamStatusControl::createMoveAction(double newState)
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
