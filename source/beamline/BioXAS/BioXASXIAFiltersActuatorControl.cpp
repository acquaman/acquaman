#include "BioXASXIAFiltersActuatorControl.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFiltersActuatorControl::BioXASXIAFiltersActuatorControl(const QString &name, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator State");
	setAllowsMovesWhileMoving(false);
}

BioXASXIAFiltersActuatorControl::~BioXASXIAFiltersActuatorControl()
{

}

void BioXASXIAFiltersActuatorControl::setStatus(AMReadOnlyPVControl *newControl)
{
	if (setStatusControl(newControl))
		emit statusChanged(newControl);
}

void BioXASXIAFiltersActuatorControl::addFilterState(int index, const QString &stateName, AMControl *control)
{
	if (addState(index, stateName, index, control, 1))
		emit statesChanged();
}
