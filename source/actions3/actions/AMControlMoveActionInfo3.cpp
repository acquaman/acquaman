/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMControlMoveActionInfo3.h"

 AMControlMoveActionInfo3::~AMControlMoveActionInfo3(){}
AMControlMoveActionInfo3::AMControlMoveActionInfo3(const AMControlInfo &setpoint, QObject *parent)
	: AMActionInfo3("Control Move", "Control Move", ":/system-run.png", parent)
{
	controlInfo_.setValuesFrom(setpoint);
	isRelative_ = false;
	isRelativeFromSetpoint_ = false;

	updateDescriptionText();
}

AMControlMoveActionInfo3::AMControlMoveActionInfo3(const AMControlMoveActionInfo3 &other)
	: AMActionInfo3(other)
{
	controlInfo_.setValuesFrom(*(other.controlInfo()));
	isRelative_ = other.isRelativeMove();
	isRelativeFromSetpoint_ = other.isRelativeFromSetpoint();
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

void AMControlMoveActionInfo3::setIsRelativeFromSetpoint(bool isRelativeFromSetpoint){
	isRelativeFromSetpoint_ = isRelativeFromSetpoint;
	setModified(true);
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


