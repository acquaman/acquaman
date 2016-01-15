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

	// Current settings.

	addOption(Unknown, "Unknown", true);
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
	return ( indices_.contains(int(value)) );
}

QList<int> AMEnumeratedControl::indicesNamed(const QString &name) const
{
	return indexStringMap_.keys(name);
}

bool AMEnumeratedControl::hasIndexNamed(const QString &name) const
{
	return (!indicesNamed(name).isEmpty());
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

	double newValue = Unknown;

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

#include <QDebug>
QStringList AMEnumeratedControl::generateEnumStates() const
{
	QStringList enumOptions = generateMoveEnumStates();

	qDebug() << "\n\n" << name() << "move options: " << enumOptions;

	// We want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << indexStringMap_.value(Unknown);

	qDebug() << name() << "enum options: " << enumOptions;

	return enumOptions;
}

QStringList AMEnumeratedControl::generateMoveEnumStates() const
{
	QStringList moveOptions;

	foreach (int index, indices_) {
		if (indexReadOnlyStatusMap_.contains(index) && (indexReadOnlyStatusMap_.value(index) == false))
			moveOptions << indexStringMap_.value(index);
	}

	qDebug() << name() << "move options:" << moveOptions;

	return moveOptions;
}
