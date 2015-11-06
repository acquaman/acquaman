#include "BioXASCarbonFilterFarmActuatorWindowControl.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator Window Control");
	setAllowsMovesWhileMoving(false);
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}

QString BioXASCarbonFilterFarmActuatorWindowControl::windowToString(double window)
{
	QString result;

	switch (int(window)) {
	case None:
		result = "None";
		break;
	case Bottom:
		result = "Bottom";
		break;
	case Top:
		result = "Top";
		break;
	default:
		break;
	}

	return result;
}

void BioXASCarbonFilterFarmActuatorWindowControl::setPositionControl(BioXASCarbonFilterFarmActuatorPositionControl *newControl)
{
	setBaseControl(newControl);
}

void BioXASCarbonFilterFarmActuatorWindowControl::addWindow(Option window, double windowSetpoint, double windowMin, double windowMax)
{
	addOption(window, windowToString(window), windowSetpoint, windowMin, windowMax);
}
