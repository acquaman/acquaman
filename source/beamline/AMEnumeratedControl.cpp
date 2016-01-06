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
	updateMinimumValue();
	updateMaximumValue();
	updateValue();
	updateMoving();
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

bool AMEnumeratedControl::addOption(int index, const QString &optionString)
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

		result = true;
	}

	return result;
}
