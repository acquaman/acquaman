#include "AMPseudoMotorLadderControl.h"
#include "actions3/AMActionSupport.h"

AMPseudoMotorLadderControl::AMPseudoMotorLadderControl(const QString &name, const QString &units, QObject *parent) :
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

AMPseudoMotorLadderControl::~AMPseudoMotorLadderControl()
{

}

bool AMPseudoMotorLadderControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMeasure();

	return result;
}

bool AMPseudoMotorLadderControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMove();

	return result;
}

bool AMPseudoMotorLadderControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = control_->canStop();

	return result;
}

bool AMPseudoMotorLadderControl::validValue(double value) const
{
	return (value >= 0 && value < enumNames().count());
}

bool AMPseudoMotorLadderControl::validSetpoint(double value) const
{
	return (value >= 0 && value < moveEnumNames().count());
}

QList<int> AMPseudoMotorLadderControl::indicesNamed(const QString &name) const
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

void AMPseudoMotorLadderControl::setControl(AMControl *newControl)
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

void AMPseudoMotorLadderControl::addOption(int index, const QString &optionString, double optionSetpoint, double optionMin, double optionMax)
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

void AMPseudoMotorLadderControl::addOption(int index, const QString &optionString, double optionSetpoint)
{
	addOption(index, optionString, optionSetpoint, optionSetpoint, optionSetpoint);
}

void AMPseudoMotorLadderControl::removeOption(int index)
{
	indices_.removeOne(index);
	indexStringMap_.remove(index);
	indexSetpointMap_.remove(index);
	indexMinimumMap_.remove(index);
	indexMaximumMap_.remove(index);

	updateEnumStates();

	emit optionsChanged();
}

void AMPseudoMotorLadderControl::clearOptions()
{
	indices_.clear();
	indexStringMap_.clear();
	indexSetpointMap_.clear();
	indexMinimumMap_.clear();
	indexMaximumMap_.clear();

	updateEnumStates();

	emit optionsChanged();
}

void AMPseudoMotorLadderControl::updateStates()
{
	updateConnected();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void AMPseudoMotorLadderControl::updateConnected()
{
	bool isConnected = ( control_ && control_->isConnected() );
	setConnected(isConnected);
}

void AMPseudoMotorLadderControl::updateEnumStates()
{
	setEnumStates(generateEnumStates());
	setMoveEnumStates(generateMoveEnumStates());
}

void AMPseudoMotorLadderControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void AMPseudoMotorLadderControl::updateValue()
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

void AMPseudoMotorLadderControl::updateMoving()
{
	bool isMoving = ( control_ && control_->isMoving());
	setIsMoving(isMoving);
}

AMAction3* AMPseudoMotorLadderControl::createMoveAction(double indexSetpoint)
{
	// Match the given index setpoint with the corresponding control setpoint.
	// Create and return action that moves the control to the control setpoint.
	// We can assume that the indexSetpoint given is valid, according to the
	// validSetpoint() provided.

	double controlSetpoint = indexSetpointMap_.value(int(indexSetpoint));
	AMAction3 *action = AMActionSupport::buildControlMoveAction(control_, controlSetpoint);

	return action;
}

QStringList AMPseudoMotorLadderControl::generateEnumStates() const
{
	QStringList enumOptions = generateMoveEnumStates();

	// We want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList AMPseudoMotorLadderControl::generateMoveEnumStates() const
{
	QStringList moveOptions;

	foreach (int index, indices_)
		moveOptions << indexStringMap_.value(index);

	return moveOptions;
}
