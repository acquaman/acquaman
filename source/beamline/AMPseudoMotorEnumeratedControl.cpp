#include "AMPseudoMotorEnumeratedControl.h"
#include "actions3/AMActionSupport.h"

AMPseudoMotorEnumeratedControl::AMPseudoMotorEnumeratedControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	control_ = 0;

	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 0;
}

AMPseudoMotorEnumeratedControl::~AMPseudoMotorEnumeratedControl()
{

}

bool AMPseudoMotorEnumeratedControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMeasure();

	return result;
}

bool AMPseudoMotorEnumeratedControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMove();

	return result;
}

bool AMPseudoMotorEnumeratedControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = control_->canStop();

	return result;
}

bool AMPseudoMotorEnumeratedControl::validValue(double value) const
{
	return (value >= 0 && value < enumNames().count());
}

bool AMPseudoMotorEnumeratedControl::validSetpoint(double value) const
{
	return (value >= 0 && value < moveEnumNames().count());
}

QList<int> AMPseudoMotorEnumeratedControl::indicesNamed(const QString &name) const
{
	return indexStringMap_.keys(name);
}

QList<int> AMPseudoMotorLadderControl::indicesContaining(double setpoint) const
{
	QList<int> results;

	foreach (int index, indices_) {
		double optionMin = indexMinimumMap_.value(index);
		double optionMax = indexMaximumMap_.value(index);

		if (setpoint >= optionMin && setpoint <= optionMax)
			results << index;
	}
}

void AMPseudoMotorEnumeratedControl::setControl(AMControl *newControl)
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

void AMPseudoMotorEnumeratedControl::addOption(int index, const QString &optionString, double optionSetpoint, double optionMin, double optionMax)
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

void AMPseudoMotorEnumeratedControl::addOption(int index, const QString &optionString, double optionSetpoint)
{
	addOption(index, optionString, optionSetpoint, optionSetpoint, optionSetpoint);
}

void AMPseudoMotorEnumeratedControl::removeOption(int index)
{
	indices_.removeOne(index);
	indexStringMap_.remove(index);
	indexSetpointMap_.remove(index);
	indexMinimumMap_.remove(index);
	indexMaximumMap_.remove(index);

	updateEnumStates();

	emit optionsChanged();
}

void AMPseudoMotorEnumeratedControl::clearOptions()
{
	indices_.clear();
	indexStringMap_.clear();
	indexSetpointMap_.clear();
	indexMinimumMap_.clear();
	indexMaximumMap_.clear();

	updateEnumStates();

	emit optionsChanged();
}

void AMPseudoMotorEnumeratedControl::updateStates()
{
	updateConnected();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void AMPseudoMotorEnumeratedControl::updateConnected()
{
	bool isConnected = ( control_ && control_->isConnected() );
	setConnected(isConnected);
}

void AMPseudoMotorEnumeratedControl::updateEnumStates()
{
	setEnumStates(generateEnumStates());
	setMoveEnumStates(generateMoveEnumStates());
}

void AMPseudoMotorEnumeratedControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void AMPseudoMotorEnumeratedControl::updateValue()
{
	// Initialize the new value to "Unknown".

	double newValue = enumNames().indexOf("Unknown");

	// Identify the index corresponding to the control's current value.
	// If there are multiple indices, pick the first.

	if (isConnected()) {
		QList<int> indices = indicesContaining(control_->value());

		if (!indices.isEmpty())
			newValue = indices_.first();
	}

	// Set the new value.

	setValue(newValue);
}

void AMPseudoMotorEnumeratedControl::updateMoving()
{
	bool isMoving = ( control_ && control_->isMoving());
	setIsMoving(isMoving);
}

AMAction3* AMPseudoMotorEnumeratedControl::createMoveAction(double indexSetpoint)
{
	// Match the given index setpoint with the corresponding control setpoint.
	// Create and return action that moves the control to the control setpoint.
	// We can assume that the indexSetpoint given is valid, according to the
	// validSetpoint() provided.

	double controlSetpoint = indexSetpointMap_.value(int(indexSetpoint));
	AMAction3 *action = AMActionSupport::buildControlMoveAction(control_, controlSetpoint);

	return action;
}

QStringList AMPseudoMotorEnumeratedControl::generateEnumStates() const
{
	QStringList enumOptions = generateMoveEnumStates();

	// We want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList AMPseudoMotorEnumeratedControl::generateMoveEnumStates() const
{
	QStringList moveOptions;

	foreach (int index, indices_)
		moveOptions << indexStringMap_.value(index);

	return moveOptions;
}
