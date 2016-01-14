#include "BioXASFastShutter.h"

BioXASFastShutter::BioXASFastShutter(const QString &name, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Fast shutter");
	setAllowsMovesWhileMoving(false);
}

BioXASFastShutter::~BioXASFastShutter()
{

}

void BioXASFastShutter::setStatus(AMControl *newStatus)
{
	if (setStatusControl(newStatus))
		emit statusChanged(newStatus);
}

void BioXASFastShutter::addShutterState(int index, const QString &stateName, AMControl *control, double triggerValue)
{
	if (addState(index, stateName, index, control, triggerValue))
		emit statesChanged();
}

void BioXASFastShutter::removeShutterState(int index)
{
	if (removeState(index))
		emit statesChanged();
}

void BioXASFastShutter::clearShutterStates()
{
	if (clearStates())
		emit statesChanged();
}
