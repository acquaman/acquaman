#include "AMControlMoveActionInfo3.h"

AMControlMoveActionInfo3::AMControlMoveActionInfo3(const AMControlInfo &setpoint, QObject *parent)
	: AMActionInfo3("Control Move", "Control Move", ":/system-run.png", parent)
{
	controlInfo_.setValuesFrom(setpoint);
    isRelative_ = false;

    updateDescriptionText();
}

AMControlMoveActionInfo3::AMControlMoveActionInfo3(const AMControlMoveActionInfo3 &other)
	: AMActionInfo3(other)
{
	controlInfo_.setValuesFrom(*(other.controlInfo()));
    isRelative_ = other.isRelative_;
}

void AMControlMoveActionInfo3::setSetpoint(double setpoint)
{
	controlInfo_.setValue(setpoint);
	setModified(true);
	updateDescriptionText();
}

void AMControlMoveActionInfo3::setControlInfo(const AMControlInfo &controlInfo)
{
	controlInfo_.setValuesFrom(controlInfo);
	setModified(true);
	updateDescriptionText();
}

void AMControlMoveActionInfo3::setIsRelativeMove(bool isRelative)
{
	isRelative_ = isRelative;
	setModified(true);
	updateDescriptionText();
}


void AMControlMoveActionInfo3::updateDescriptionText()
{
	QString d;

	if(isRelativeMove())
		d = QString("Moving %1 by relative %2 %3").arg(controlInfo_.description().isEmpty() ? controlInfo_.name() : controlInfo_.description()).arg(controlInfo_.value()).arg(controlInfo_.units());
	else
		d = QString("Moving %1 to %2 %3").arg(controlInfo_.description().isEmpty() ? controlInfo_.name() : controlInfo_.description()).arg(controlInfo_.value()).arg(controlInfo_.units());

	setShortDescription(d);
	setLongDescription(d);
}


