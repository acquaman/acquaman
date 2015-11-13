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
#include <QDebug>
AMControl::FailureExplanation AMEnumeratedControl::move(double setpoint)
{
	qDebug() << "\n\nAMEnumeratedControl setpoint:" << setpoint;

	// Check to see whether the given setpoint converts to a valid option index.
	// The two may not be the same, as in the case where valid options don't start with 0.
	// In all cases where the name is unique, however, the enum setpoint and the option
	// setpoint are related by the enum name.

	if (setpoint < 0 || setpoint >= enumNames().count()) {
		AMErrorMon::alert(this, AMENUMERATEDCONTROL_INVALID_ENUM_SETPOINT, QString("Failed to move %1: the enum setpoint %2 given is outside the range of valid enum indices.").arg(name()).arg(QString::number(setpoint)));
		return AMControl::OtherFailure;
	}

	QList<int> optionIndices;
	QString enumName = enumNames().at(int(setpoint));

	if (indexStringMap_.values().contains(enumName))
		optionIndices = indexStringMap_.keys(enumName);

	if (optionIndices.isEmpty()) {
		AMErrorMon::alert(this, AMENUMERATEDCONTROL_INVALID_OPTION_SETPOINT, QString("Failed to move %1: control has no valid option index corresponding to setpoint %2.").arg(name()).arg(QString::number(setpoint)));
		return AMControl::OtherFailure;
	}

	// A valid option index has been found. Proceed with normal move to option index.

	int option = optionIndices.first();

	return AMPseudoMotorControl::move(option);
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

void AMEnumeratedControl::addOption(int index, const QString &optionString)
{
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

		updateEnumStates();

		emit optionsChanged();
	}
}

void AMEnumeratedControl::removeOption(int index)
{
	indices_.removeOne(index);
	indexStringMap_.remove(index);

	updateEnumStates();

	emit optionsChanged();
}

void AMEnumeratedControl::clearOptions()
{
	indices_.clear();
	indexStringMap_.clear();

	updateEnumStates();

	emit optionsChanged();
}

QStringList AMEnumeratedControl::generateEnumStates() const
{
	QStringList enumOptions = generateMoveEnumStates();

	// We want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	if (!enumOptions.contains("Unknown"))
		enumOptions << "Unknown";

	return enumOptions;
}

QStringList AMEnumeratedControl::generateMoveEnumStates() const
{
	QStringList moveOptions;

	foreach (int index, indices_)
		moveOptions << indexStringMap_.value(index);

	return moveOptions;
}
