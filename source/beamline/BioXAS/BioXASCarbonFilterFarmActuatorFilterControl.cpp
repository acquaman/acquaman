#include "BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "actions3/AMActionSupport.h"

BioXASCarbonFilterFarmActuatorFilterControl::BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMSingleEnumeratedControl(name, units, parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator Filter Control");
	setAllowsMovesWhileMoving(false);
	setAllowsDuplicateOptions(false);
}

BioXASCarbonFilterFarmActuatorFilterControl::~BioXASCarbonFilterFarmActuatorFilterControl()
{

}

QList<int> BioXASCarbonFilterFarmActuatorFilterControl::indicesWithFilter(double filter) const
{
	QList<int> results;

	foreach (int index, indices_) {
		if (indexFilterMap_.value(index) == filter)
			results << index;
	}

	return results;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::hasFilter(double filter) const
{
	bool result = (!indicesWithFilter(filter).isEmpty());
	return result;
}

double BioXASCarbonFilterFarmActuatorFilterControl::filterAt(int index) const
{
	return indexFilterMap_.value(index, -1);
}

void BioXASCarbonFilterFarmActuatorFilterControl::setWindowControl(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	if (setBaseControl(newControl))
		emit windowChanged(newControl);
}

void BioXASCarbonFilterFarmActuatorFilterControl::addFilter(int windowIndex, double filter)
{
	QString filterString = QString::number(filter, 'f', 0);

	if (!hasIndexNamed(filterString)) { // We only want unique-looking options available for this control.

		indexFilterMap_.insert(windowIndex, filter); // Because each window will only ever have one filter, we can use the window index as the filter index.
		addValueOption(windowIndex, filterString, windowIndex);
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::removeFilter(int windowIndex)
{
	indexFilterMap_.remove(windowIndex);
	removeOption(windowIndex);
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearFilters()
{
	indexFilterMap_.clear();
	filterWindowPreferenceMap_.clear();
	clearOptions();
}

void BioXASCarbonFilterFarmActuatorFilterControl::setWindowPreference(double filter, int windowIndex)
{
	filterWindowPreferenceMap_.insert(filter, windowIndex);
}

void BioXASCarbonFilterFarmActuatorFilterControl::removeWindowPreference(double filter)
{
	filterWindowPreferenceMap_.remove(filter);
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearWindowPreferences()
{
	filterWindowPreferenceMap_.clear();
}

AMAction3* BioXASCarbonFilterFarmActuatorFilterControl::createMoveAction(double indexSetpoint)
{
	AMAction3 *action = 0;

	// Convert the desired indexSetpoint to a corresponding filter value.

	double filter = indexFilterMap_.value(int(indexSetpoint));

	// Check to see if there is a preferred window to use to achieve the given
	// filter value. If there is no preference, the move action is identical
	// to AMSingleEnumeratedControl::createMoveAction.

	if (filterWindowPreferenceMap_.keys().contains(filter)) {
		int windowIndex = filterWindowPreferenceMap_.value(filter);

		// Need to check that the window index is a valid setpoint for this control.

		action = AMActionSupport::buildControlMoveAction(control_, windowIndex);

	} else {
		action = AMSingleEnumeratedControl::createMoveAction(indexSetpoint);
	}

	return action;
}
