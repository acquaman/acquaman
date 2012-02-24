#include "AMControlMoveActionInfo.h"

AMControlMoveActionInfo::AMControlMoveActionInfo(const AMControlInfo &setpoint, QObject *parent)
    : AMActionInfo("Control Move", "Control Move", ":/system-run.png", parent)
{
	controlInfo_.setValuesFrom(setpoint);
    isRelative_ = false;

    updateDescriptionText();
}

AMControlMoveActionInfo::AMControlMoveActionInfo(const AMControlMoveActionInfo &other) : AMActionInfo(other) {
	controlInfo_.setValuesFrom(*(other.controlInfo()));
    isRelative_ = other.isRelative_;
}

void AMControlMoveActionInfo::setSetpoint(double setpoint)
{
	controlInfo_.setValue(setpoint);
	setModified(true);
	updateDescriptionText();
}

void AMControlMoveActionInfo::setControlInfo(const AMControlInfo &controlInfo)
{
	controlInfo_.setValuesFrom(controlInfo);
	setModified(true);
	updateDescriptionText();
}

void AMControlMoveActionInfo::setIsRelativeMove(bool isRelative)
{
	isRelative_ = isRelative;
	setModified(true);
	updateDescriptionText();
}


void AMControlMoveActionInfo::updateDescriptionText()
{
	QString d;

	if(isRelativeMove())
		d = QString("Moving %1 by relative %2 %3").arg(controlInfo_.description().isEmpty() ? controlInfo_.name() : controlInfo_.description()).arg(controlInfo_.value()).arg(controlInfo_.units());
	else
		d = QString("Moving %1 to %2 %3").arg(controlInfo_.description().isEmpty() ? controlInfo_.name() : controlInfo_.description()).arg(controlInfo_.value()).arg(controlInfo_.units());

	setShortDescription(d);
	setLongDescription(d);
}


