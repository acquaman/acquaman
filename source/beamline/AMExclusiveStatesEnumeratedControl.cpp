#include "AMExclusiveStatesEnumeratedControl.h"
#include "actions3/AMActionSupport.h"

AMExclusiveStatesEnumeratedControl::AMExclusiveStatesEnumeratedControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMSingleEnumeratedControl(name, units, parent, description)
{

}

AMExclusiveStatesEnumeratedControl::~AMExclusiveStatesEnumeratedControl()
{

}

bool AMExclusiveStatesEnumeratedControl::canMove() const
{
	bool result = false;
	QList<AMControl*> controls = indexControlMap_.values();

	// If this control is connected, check whether any index controls that are set
	// can move. If there are no controls, this control returns false.

	if (isConnected() && !controls.isEmpty()) {
		bool canMove = true;

		for (int i = 0, count = controls.count(); i < count && canMove; i++) {
			AMControl *control = controls.at(i);
			canMove = canMove && control && control->canMove();
		}

		result = canMove;
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::canStop() const
{
	bool result = false;
	QList<AMControl*> controls = indexControlMap_.values();

	// If this control is connected, check whether any index controls that are set
	// can stop. If there are no controls, this control returns false.

	if (isConnected() && !controls.isEmpty()) {
		bool canStop = true;

		for (int i = 0, count = controls.count(); i < count && canStop; i++) {
			AMControl *control = controls.at(i);
			canStop = canStop && control && control->canStop();
		}

		result = canStop;
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::setStatusControl(AMControl *newControl)
{
	bool result = false;

	if (setBaseControl(newControl)) {
		result = true;
		emit statusChanged(newControl);
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::addState(int index, const QString &stateName, double statusValue, AMControl *control, double controlTriggerValue)
{
	bool result = false;

	if (AMSingleEnumeratedControl::addValueOption(index, stateName, statusValue, statusValue, statusValue)) {
		setControl(index, control);
		indexTriggerMap_.insert(index, controlTriggerValue);

		updateStates(); // The AMPseudoMotor states.

		result = true;
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::addReadOnlyState(int index, const QString &stateName, double statusValue)
{
	return AMSingleEnumeratedControl::addValueOption(index, stateName, statusValue, statusValue, statusValue, true);
}

bool AMExclusiveStatesEnumeratedControl::removeState(int index)
{
	bool result = false;

	if (AMSingleEnumeratedControl::removeOption(index)) {
		removeControl(index);
		indexTriggerMap_.remove(index);

		result = true;
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::clearStates()
{
	bool result = false;

	if (AMSingleEnumeratedControl::clearOptions()) {
		clearControls();
		indexTriggerMap_.clear();

		result = true;
	}

	return result;
}
#include "actions3/AMListAction3.h"
AMAction3* AMExclusiveStatesEnumeratedControl::createMoveAction(double optionIndex)
{
	AMAction3 *result = 0;

	// Match the given index setpoint with the corresponding control setpoint.
	// Create and return action that moves the control to the control setpoint.
	// We can assume that the optionIndex given is valid, according to the
	// validSetpoint() provided.

	AMControl *triggerControl = indexControlMap_.value(optionIndex, 0);
	QList<double> triggerOptions = indexTriggerMap_.values(optionIndex);

	if (control_ && triggerControl && !triggerOptions.isEmpty()) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(QString("%1 move to %2").arg(control_->name()).arg(optionIndex), QString("%1 move to %2").arg(control_->name()).arg(optionIndex)), AMListAction3::Sequential);
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(triggerControl, triggerOptions.first()));
		moveAction->addSubAction(AMActionSupport::buildControlWaitAction(control_,  indexSetpointMap_.value(optionIndex), 20));

		result = moveAction;
	}

	return result;
}

void AMExclusiveStatesEnumeratedControl::setControl(int index, AMControl *control)
{
	if (indexControlMap_.keys().contains(index) && indexControlMap_.value(index) != control)
		removeControl(index);

	indexControlMap_.insert(index, control);
	addChildControl(control);
}

void AMExclusiveStatesEnumeratedControl::removeControl(int index)
{
	AMControl *toRemove = indexControlMap_.value(index, 0);

	if (toRemove) {
		removeChildControl(toRemove);
		indexControlMap_.remove(index);
	}
}

void AMExclusiveStatesEnumeratedControl::clearControls()
{
	foreach (AMControl *control, indexControlMap_.values())
		removeChildControl(control);

	indexControlMap_.clear();
}
