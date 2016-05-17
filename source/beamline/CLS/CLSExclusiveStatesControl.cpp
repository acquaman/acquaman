#include "CLSExclusiveStatesControl.h"
#include "beamline/AMPVControl.h"

CLSExclusiveStatesControl::CLSExclusiveStatesControl(const QString &name, const QString &statusPV, const QString &openPV, const QString &closePV, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(QString("%1").arg(statusPV), statusPV, this));
	addClosedState(new AMSinglePVControl(QString("%1").arg(closePV), closePV, this), Status::Closed, 1);
	addOpenState(new AMSinglePVControl(QString("%1").arg(openPV), openPV, this), Status::Open, 1);
	addBetweenState(Status::Between);
}

CLSExclusiveStatesControl::~CLSExclusiveStatesControl()
{

}

int CLSExclusiveStatesControl::alarmStatus() const
{
	return control_->alarmStatus();
}

int CLSExclusiveStatesControl::alarmSeverity() const
{
	return control_->alarmSeverity();
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

AMControl::FailureExplanation CLSExclusiveStatesControl::open()
{
	return move(Open);
}

AMControl::FailureExplanation CLSExclusiveStatesControl::close()
{
	return move(Closed);
}

bool CLSExclusiveStatesControl::setStatusControl(AMControl *newControl)
{
	bool result = false;

	if (setBaseControl(newControl)) {

		if (newControl) {
			connect( newControl, SIGNAL(valueChanged(double)), this, SLOT(updateStates()) );
			connect( newControl, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)) );
		}

		result = true;
		emit statusChanged(newControl);
	}		

	return result;
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
