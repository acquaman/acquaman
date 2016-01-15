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
	if (AMSingleEnumeratedControl::addValueOption(windowIndex, windowString, positionSetpoint, positionMin, positionMax, false))
		emit windowsChanged();
}

void BioXASCarbonFilterFarmActuatorWindowControl::removeWindow(int windowIndex)
{
	if (AMSingleEnumeratedControl::removeOption(windowIndex))
		emit windowsChanged();
}

void BioXASCarbonFilterFarmActuatorWindowControl::clearWindows()
{
	if (AMSingleEnumeratedControl::clearOptions())
		emit windowsChanged();
}
