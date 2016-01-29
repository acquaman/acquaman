#include "AMBiStateControl.h"

AMBiStateControl::AMBiStateControl(const QString &name, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{

}

AMBiStateControl::~AMBiStateControl()
{

}

bool AMBiStateControl::isOpen() const
{
	return (int(value()) == Open);
}

bool AMBiStateControl::isClosed() const
{
	return (int(value()) == Closed);
}

bool AMBiStateControl::isBetween() const
{
	return (int(value()) == Between);
}

void AMBiStateControl::addOpenState(AMControl *openTriggerControl, double openStatusValue, double triggerValue)
{
	addState(Open, "Open", openStatusValue, openTriggerControl, triggerValue);
}

void AMBiStateControl::removeOpenState()
{
	removeState(Open);
}

void AMBiStateControl::addClosedState(AMControl *closeTriggerControl, double closedStatusValue, double triggerValue)
{
	addState(Closed, "Closed", closedStatusValue, closeTriggerControl, triggerValue);
}

void AMBiStateControl::removeClosedState()
{
	removeState(Closed);
}

void AMBiStateControl::addBetweenState(double betweenStatusValue)
{
	addReadOnlyState(Between, "Between", betweenStatusValue);
}

void AMBiStateControl::removeBetweenState()
{
	removeState(Between);
}

void AMBiStateControl::updateMoving()
{
	setIsMoving( isBetween() );
}
