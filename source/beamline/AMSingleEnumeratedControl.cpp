#include "AMSingleEnumeratedControl.h"
#include "actions3/AMActionSupport.h"

AMSingleEnumeratedControl::AMSingleEnumeratedControl(const QString &name, const QString &units, QObject *parent) :
	AMEnumeratedControl(name, units, parent)
{
	// Initialize local variables.

	control_ = 0;
}

AMSingleEnumeratedControl::~AMSingleEnumeratedControl()
{

}

bool AMSingleEnumeratedControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMeasure();

	return result;
}

bool AMSingleEnumeratedControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMove();

	return result;
}

bool AMSingleEnumeratedControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = control_->canStop();

	return result;
}

QList<int> AMSingleEnumeratedControl::indicesContaining(double setpoint) const
{
	QList<int> results;

	foreach (int index, indices_) {
		double optionMin = indexMinimumMap_.value(index);
		double optionMax = indexMaximumMap_.value(index);

		if (setpoint >= optionMin && setpoint <= optionMax)
			results << index;
	}

	return results;
}

bool AMSingleEnumeratedControl::setBaseControl(AMControl *newControl)
{
	bool result = false;

	if (control_ != newControl) {

		if (control_)
			removeChildControl(control_);

		control_ = newControl;

		if (control_)
			addChildControl(control_);

		updateStates();

		result = true;
	}

	return result;
}

void AMSingleEnumeratedControl::updateConnected()
{
	bool isConnected = ( control_ && control_->isConnected() );
	setConnected(isConnected);
}

void AMSingleEnumeratedControl::updateMoving()
{
	bool isMoving = ( control_ && control_->isMoving());
	setIsMoving(isMoving);
}

bool AMSingleEnumeratedControl::addValueOption(int index, const QString &optionString, double optionSetpoint, double optionMin, double optionMax)
{
	bool result = false;

	if (AMEnumeratedControl::addOption(index, optionString)) {
		indexSetpointMap_.insert(index, optionSetpoint);
		indexMinimumMap_.insert(index, optionMin);
		indexMaximumMap_.insert(index, optionMax);

		result = true;
	}

	return result;
}

bool AMSingleEnumeratedControl::addValueOption(int index, const QString &optionString, double optionSetpoint)
{
	addValueOption(index, optionString, optionSetpoint, optionSetpoint, optionSetpoint);
}

bool AMSingleEnumeratedControl::removeOption(int index)
{
	bool result = false;

	if (AMEnumeratedControl::removeOption(index)) {
		indexSetpointMap_.remove(index);
		indexMinimumMap_.remove(index);
		indexMaximumMap_.remove(index);

		result = true;
	}

	return result;
}

bool AMSingleEnumeratedControl::clearOptions()
{
	bool result = false;

	if (AMEnumeratedControl::clearOptions()) {
		indexSetpointMap_.clear();
		indexMinimumMap_.clear();
		indexMaximumMap_.clear();

		result = true;
	}

	return result;
}

int AMSingleEnumeratedControl::currentIndex() const
{
	// Initialize the new index to "Unknown".

	int currentIndex = AMEnumeratedControl::Unknown;

	if (control_) {

		// Identify the index corresponding to the control's current value.
		// If there are multiple indices, pick the first.

		QList<int> indices = indicesContaining(control_->value());

		if (!indices.isEmpty())
			currentIndex = indices.first();
	}

	return currentIndex;
}

AMAction3* AMSingleEnumeratedControl::createMoveAction(double indexSetpoint)
{
	// Match the given index setpoint with the corresponding control setpoint.
	// Create and return action that moves the control to the control setpoint.
	// We can assume that the indexSetpoint given is valid, according to the
	// validSetpoint() provided.

	double controlSetpoint = indexSetpointMap_.value(int(indexSetpoint));
	AMAction3 *action = AMActionSupport::buildControlMoveAction(control_, controlSetpoint);

	return action;
}
