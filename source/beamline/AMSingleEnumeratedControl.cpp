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
}

void AMSingleEnumeratedControl::setBaseControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			removeChildControl(control_);

		control_ = newControl;

		if (control_)
			addChildControl(control_);

		updateStates();

		emit controlChanged(control_);
	}
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

void AMSingleEnumeratedControl::addOption(int index, const QString &optionString, double optionSetpoint, double optionMin, double optionMax)
{
	if (!indices_.contains(index))
		indices_.append(index);

	indexStringMap_.insert(index, optionString);
	indexSetpointMap_.insert(index, optionSetpoint);
	indexMinimumMap_.insert(index, optionMin);
	indexMaximumMap_.insert(index, optionMax);

	updateEnumStates();

	emit optionsChanged();
}

void AMSingleEnumeratedControl::addOption(int index, const QString &optionString, double optionSetpoint)
{
	addOption(index, optionString, optionSetpoint, optionSetpoint, optionSetpoint);
}

void AMSingleEnumeratedControl::removeOption(int index)
{
	indices_.removeOne(index);
	indexStringMap_.remove(index);
	indexSetpointMap_.remove(index);
	indexMinimumMap_.remove(index);
	indexMaximumMap_.remove(index);

	updateEnumStates();

	emit optionsChanged();
}

void AMSingleEnumeratedControl::clearOptions()
{
	indices_.clear();
	indexStringMap_.clear();
	indexSetpointMap_.clear();
	indexMinimumMap_.clear();
	indexMaximumMap_.clear();

	updateEnumStates();

	emit optionsChanged();
}

int AMSingleEnumeratedControl::currentIndex() const
{
	int currentIndex = -1;

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
