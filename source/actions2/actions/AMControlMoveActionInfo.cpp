#include "AMControlMoveActionInfo.h"

AMControlMoveActionInfo::AMControlMoveActionInfo(const AMControlInfo &setpoint, QObject *parent)
	: AMActionInfo(QString("Moving %1 to %2 %3").arg(setpoint.description().isEmpty() ? setpoint.name() : setpoint.description()).arg(setpoint.value()).arg(setpoint.units()),
				   QString("Moving %1 to %2 %3").arg(setpoint.description().isEmpty() ? setpoint.name() : setpoint.description()).arg(setpoint.value()).arg(setpoint.units()),
				   ":/system-run.png", parent)
{
	controlInfo_.setValuesFrom(setpoint);
}

AMControlMoveActionInfo::AMControlMoveActionInfo(const AMControlMoveActionInfo &other) : AMActionInfo(other) {
	controlInfo_.setValuesFrom(*(other.controlInfo()));
}

void AMControlMoveActionInfo::setSetpoint(double setpoint)
{
	controlInfo_.setValue(setpoint);
}
