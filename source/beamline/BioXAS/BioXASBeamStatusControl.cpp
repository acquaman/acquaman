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
