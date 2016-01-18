#include "AMEnumeratedControl.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"

AMEnumeratedControl::AMEnumeratedControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 0;

	// Initialize class variables.

	allowsDuplicateOptions_ = false;
}

AMEnumeratedControl::~AMEnumeratedControl()
{

}

bool AMEnumeratedControl::validValue(double value) const
{
	return ( indices_.contains(int(value)) );
}

bool AMEnumeratedControl::validSetpoint(double value) const
{
	return ( moveIndices().contains(int(value)) );
}

QList<int> AMEnumeratedControl::readOnlyIndices() const
{
	QList<int> readOnlyIndices;

	foreach (int index, indices_) {
		if (indexIsReadOnlyIndex(index))
			readOnlyIndices << index;
	}

	return readOnlyIndices;
}

QList<int> AMEnumeratedControl::moveIndices() const
{
	QList<int> moveIndices;

	foreach (int index, indices_) {
		if (indexIsMoveIndex(index))
			moveIndices << index;
	}

	return moveIndices;
}

QList<int> AMEnumeratedControl::indicesNamed(const QString &name) const
{
	return indexStringMap_.keys(name);
}

bool AMEnumeratedControl::hasIndexNamed(const QString &name) const
{
	return (!indicesNamed(name).isEmpty());
}

bool AMEnumeratedControl::indexIsReadOnlyIndex(int index) const
{
	bool result = false;

	if (indexReadOnlyStatusMap_.keys().contains(index) && indexReadOnlyStatusMap_.value(index) == true)
		result = true;

	return result;
}

bool AMEnumeratedControl::indexIsMoveIndex(int index) const
{
	bool result = false;

	if (indexReadOnlyStatusMap_.keys().contains(index) && indexReadOnlyStatusMap_.value(index) == false)
		result = true;

	return result;
}

void AMEnumeratedControl::setAllowsDuplicateOptions(bool newStatus)
{
	if (allowsDuplicateOptions_ != newStatus) {
		allowsDuplicateOptions_ = newStatus;
		emit allowsDuplicationOptionsChanged(allowsDuplicateOptions_);
	}
}

void AMEnumeratedControl::updateStates()
{
	updateConnected();
	updateOptions();
	updateEnumStates();
	updateMinimumValue();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void AMEnumeratedControl::updateEnumStates()
{
	setEnumStates(generateEnumStates());
	setMoveEnumStates(generateMoveEnumStates());
}

void AMEnumeratedControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void AMEnumeratedControl::updateValue()
{
	// Initialize the new value to "Unknown".

	double newValue = enumNames().indexOf("Unknown");

	// Identify the current index.

	if (isConnected())
		newValue = currentIndex();

	// Set the new value.

	setValue(newValue);
}

bool AMEnumeratedControl::addOption(int index, const QString &optionString, bool readOnly)
{
	bool result = false;
	bool proceed = false;

	// First check whether we are in a situation where duplicate value options
	// may be an issue.

	if (!hasIndexNamed(optionString))
		proceed = true;
	else if (hasIndexNamed(optionString) && allowsDuplicateOptions_)
		proceed = true;

	// Proceed with adding the option if duplication isn't an issue.

	if (proceed) {
		if (!indices_.contains(index))
			indices_.append(index);

		indexStringMap_.insert(index, optionString);

		indexReadOnlyStatusMap_.insert(index, readOnly);

		updateEnumStates();

		result = true;
	}

	return result;
}

bool AMEnumeratedControl::removeOption(int index)
{
	bool result = false;

	if (indices_.contains(index)) {
		indices_.removeOne(index);
		indexStringMap_.remove(index);
		indexReadOnlyStatusMap_.remove(index);

		updateEnumStates();

		result = true;
	}

	return result;
}

bool AMEnumeratedControl::clearOptions()
{
	bool result = false;

	if (!indices_.isEmpty()) {
		indices_.clear();
		indexStringMap_.clear();
		indexReadOnlyStatusMap_.clear();

		updateEnumStates();

		result = true;
	}

	return result;
}

QStringList AMEnumeratedControl::generateEnumStates() const
{
	// The move enum states must always be presented first.

	QStringList enumOptions = generateMoveEnumStates();

	// The read-only options second.

	enumOptions << generateReadOnlyEnumStates();

	// We want to have an "Unknown" option--it's the default value.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList AMEnumeratedControl::generateReadOnlyEnumStates() const
{
	QStringList readOnlyOptions;

	foreach (int index, readOnlyIndices()) {
		readOnlyOptions << indexStringMap_.value(index);
	}

	return readOnlyOptions;
}

QStringList AMEnumeratedControl::generateMoveEnumStates() const
{
	QStringList moveOptions;

	foreach (int index, moveIndices()) {
		moveOptions << indexStringMap_.value(index);
	}

	return moveOptions;
}
