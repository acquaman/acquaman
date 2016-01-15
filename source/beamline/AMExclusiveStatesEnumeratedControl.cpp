#include "AMExclusiveStatesEnumeratedControl.h"
#include "actions3/AMActionSupport.h"

AMExclusiveStatesEnumeratedControl::AMExclusiveStatesEnumeratedControl(const QString &name, const QString &units, QObject *parent) :
	AMEnumeratedControl(name, units, parent)
{
	// Initialize local variables.

	status_ = 0;
}

AMExclusiveStatesEnumeratedControl::~AMExclusiveStatesEnumeratedControl()
{

}

bool AMExclusiveStatesEnumeratedControl::canMeasure() const
{
	bool result = false;

	if (status_)
		result = status_->canMeasure();

	return result;
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

	if (status_ != newControl) {

		if (status_)
			removeChildControl(status_);

		status_ = newControl;

		if (status_)
			addChildControl(status_);

		updateStates();

		result = true;
	}

	return result;
}

void AMExclusiveStatesEnumeratedControl::updateConnected()
{
	// First, check to see if the status control is connected.

	bool isConnected = status_ && status_->isConnected();

	// If so, iterate through the controls of the added states and
	// check their connected states. If there are no state controls,
	// this control still reports as connected.

	if (isConnected) {
		QList<AMControl*> controls = indexControlMap_.values();

		for (int i = 0, count = controls.count(); i < count && isConnected; i++) {
			AMControl *control = controls.at(i);
			isConnected = isConnected && control && control->isConnected();
		}
	}

	setConnected(isConnected);
}

bool AMExclusiveStatesEnumeratedControl::addState(int index, const QString &stateName, double statusValue, AMControl *control, double controlTriggerValue)
{
	bool result = false;

	if (AMEnumeratedControl::addOption(index, stateName, false)) {
		indexStatusMap_.insert(index, statusValue);
		setControl(index, control);
		indexTriggerMap_.insert(index, controlTriggerValue);

		result = true;
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::removeState(int index)
{
	bool result = false;

	if (AMEnumeratedControl::removeOption(index)) {
		indexStatusMap_.remove(index);
		removeControl(index);
		indexTriggerMap_.remove(index);

		result = true;
	}

	return result;
}

bool AMExclusiveStatesEnumeratedControl::clearStates()
{
	bool result = false;

	if (AMEnumeratedControl::clearOptions()) {
		indexStatusMap_.clear();
		clearControls();
		indexTriggerMap_.clear();

		result = true;
	}

	return result;
}

AMAction3* AMExclusiveStatesEnumeratedControl::createMoveAction(double optionIndex)
{
	AMAction3 *result = 0;

	// Match the given index setpoint with the corresponding control setpoint.
	// Create and return action that moves the control to the control setpoint.
	// We can assume that the optionIndex given is valid, according to the
	// validSetpoint() provided.

	AMControl *control = indexControlMap_.value(optionIndex, 0);
	QList<double> triggerOptions = indexTriggerMap_.values(optionIndex);

	if (control && !triggerOptions.isEmpty())
		result = AMActionSupport::buildControlMoveAction(control, triggerOptions.first());

	return result;
}

int AMExclusiveStatesEnumeratedControl::currentIndex() const
{
	// Initialize the new index to "Unknown".

	int currentIndex = AMEnumeratedControl::Unknown;

	if (status_) {

		// Identify the index corresponding to the control's current value.
		// If there are multiple indices, pick the first. Ideally, this is
		// an 'exclusive' states control--there shouldn't be multiple indices
		// with the same state.

		QList<int> indices = indicesWithStatus(status_->value());

		if (!indices.isEmpty())
			currentIndex = indices.first();
	}

	return currentIndex;
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
