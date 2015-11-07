#include "BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "actions3/AMActionSupport.h"

BioXASCarbonFilterFarmActuatorFilterControl::BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMSingleEnumeratedControl(name, units, parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator Filter Control");
	setAllowsMovesWhileMoving(false);
}

BioXASCarbonFilterFarmActuatorFilterControl::~BioXASCarbonFilterFarmActuatorFilterControl()
{

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
	// Because each window will only ever have one filter, we can use the window index as the filter index.

	indexFilterMap_.insert(windowIndex, filter);
	addValueOption(windowIndex, QString::number(filter, 'f', 0), windowIndex);
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

int BioXASCarbonFilterFarmActuatorFilterControl::currentIndex() const
{
	int currentIndex = -1;

	if (isConnected()) {

		// Because each window will only have one filter, the filter
		// index is identical to the current window index.

		int windowIndex = int(control_->value());

		// Check that there is a filter value associated with this index.

		if (indexFilterMap_.keys().contains(windowIndex))
			currentIndex = windowIndex;
	}

	return currentIndex;
}

