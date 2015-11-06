#include "BioXASCarbonFilterFarmActuatorWindowControl.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent) :
	AMPseudoMotorLadderControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator Window Control");
	setAllowsMovesWhileMoving(false);
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}
