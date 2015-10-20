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

QString BioXASValvesControl::valueToString(BioXASValvesControl::Value value) const
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
		result = createOpenValvesAction(valveSet_);

	return result;
}

bool BioXASValvesControl::valvesCanMeasure(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool canMeasure = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && canMeasure; valveIndex++) {
			AMControl *valve = valveSet->at(valveIndex);
			canMeasure &= (valve && valve->canMeasure());
		}

		result = canMeasure;
	}

	return result;
}

bool BioXASValvesControl::valvesCanMove(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool canMove = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && canMove; valveIndex++) {
			AMControl *valve = valveSet->at(valveIndex);
			canMove &= (valve && valve->canMove());
		}

		result = canMove;
	}

	return result;
}

bool BioXASValvesControl::valvesOpen(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool valvesOpen = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && valvesOpen; valveIndex++) {
			CLSBiStateControl *valveControl = qobject_cast<CLSBiStateControl*>(valveSet->at(valveIndex));

			if (valveControl) {
				valvesOpen &= valveControl->isOpen();
			} else {
				valvesOpen = false;
			}
		}

		result = valvesOpen;
	}

	return result;
}

bool BioXASValvesControl::valvesClosed(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool valvesClosed = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && valvesClosed; valveIndex++) {
			CLSBiStateControl *valveControl = qobject_cast<CLSBiStateControl*>(valveSet->at(valveIndex));

			if (valveControl) {
				valvesClosed &= valveControl->isClosed();
			} else {
				valvesClosed = false;
			}
		}

		result = valvesClosed;
	}

	return result;
}
