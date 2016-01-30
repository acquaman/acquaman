#include "CLSExclusiveStatesControl.h"
#include "beamline/AMPVControl.h"

CLSExclusiveStatesControl::CLSExclusiveStatesControl(const QString &name, const QString &statusPV, const QString &openPV, const QString &closePV, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(QString("%1%2").arg(name).arg("Status"), statusPV, this));
	addOpenState(new AMSinglePVControl(QString("%1%2").arg(name).arg("Open"), openPV, this), Status::Open, 1);
	addClosedState(new AMSinglePVControl(QString("%1%2").arg(name).arg("Close"), closePV, this), Status::Closed, 1);
	addBetweenState(Status::Between);
}

CLSExclusiveStatesControl::~CLSExclusiveStatesControl()
{

}

bool CLSExclusiveStatesControl::isOpen() const
{
	return (int(value()) == Open);
}

bool CLSExclusiveStatesControl::isClosed() const
{
	return (int(value()) == Closed);
}

bool CLSExclusiveStatesControl::isBetween() const
{
	return (int(value()) == Between);
}

void CLSExclusiveStatesControl::updateMoving()
{
	setIsMoving( isBetween() );
}

void CLSExclusiveStatesControl::addOpenState(AMControl *openTriggerControl, double openStatusValue, double triggerValue)
{
	addState(Open, "Open", openStatusValue, openTriggerControl, triggerValue);
}

void CLSExclusiveStatesControl::removeOpenState()
{
	removeState(Open);
}

void CLSExclusiveStatesControl::addClosedState(AMControl *closeTriggerControl, double closedStatusValue, double triggerValue)
{
	addState(Closed, "Closed", closedStatusValue, closeTriggerControl, triggerValue);
}

void CLSExclusiveStatesControl::removeClosedState()
{
	removeState(Closed);
}

void CLSExclusiveStatesControl::addBetweenState(double betweenStatusValue)
{
	addReadOnlyState(Between, "Between", betweenStatusValue);
}

void CLSExclusiveStatesControl::removeBetweenState()
{
	removeState(Between);
}

