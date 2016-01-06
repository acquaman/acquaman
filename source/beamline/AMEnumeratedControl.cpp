#include "AMEnumeratedControl.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"
#include "util/AMUtility.h"

AMEnumeratedControl::AMEnumeratedControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize class variables.

	allowsDuplicateOptions_ = false;

	// Current settings.

	addOption(Unknown, "Unknown");
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

QStringList AMEnumeratedControl::moveEnumNames() const
{
	QStringList result;

	foreach (double index, moveIndices_) {
		QString indexString = indexStringMap_.value(index, QString());

		if (!indexString.isEmpty())
			result << indexString;
	}

	return result;
}

QList<double> AMEnumeratedControl::indicesNamed(const QString &name) const
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

bool AMEnumeratedControl::addOption(double index, const QString &optionString)
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

	if (result)
		emit enumChanged();

	return result;
}

bool AMEnumeratedControl::removeOption(double index)
{
	bool result = false;

	if (indices_.contains(index)) {
		indices_.removeOne(index);
		indexStringMap_.remove(index);

		result = true;
	}

	if (result)
		emit enumChanged();

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

	if (result)
		emit enumChanged();

	return result;
}

bool AMEnumeratedControl::addMoveOption(double index, const QString &optionString)
{
	bool result = false;

	if (addOption(index, optionString)) {
		moveIndices_.append(index);
		result = true;
	}

	return result;
}

double AMEnumeratedControl::currentMinimumValue() const
{
	return AMUtility::minimumFinder(indices_.toVector());
}

double AMEnumeratedControl::currentMaximumValue() const
{
	return AMUtility::maximumFinder(indices_.toVector());
}
