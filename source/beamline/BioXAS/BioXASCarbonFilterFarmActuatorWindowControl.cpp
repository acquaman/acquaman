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

void BioXASCarbonFilterFarmActuatorWindowControl::setPositionControl(BioXASCarbonFilterFarmActuatorPositionControl *newControl)
{
	if (setBaseControl(newControl))
		emit positionChanged(newControl);
}

void BioXASCarbonFilterFarmActuatorWindowControl::addWindow(int windowIndex, const QString &windowString, double positionSetpoint, double positionMin, double positionMax)
{
	addValueOption(windowIndex, windowString, positionSetpoint, positionMin, positionMax);
}

void BioXASCarbonFilterFarmActuatorWindowControl::removeWindow(int windowIndex)
{
	removeOption(windowIndex);
}

void BioXASCarbonFilterFarmActuatorWindowControl::clearWindows()
{
	clearOptions();
}
