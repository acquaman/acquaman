/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMInternalControlMoveAction.h"
#include "beamline/AMControl.h"

AMInternalControlMoveAction::AMInternalControlMoveAction(AMControl *control, double setpoint, QObject *parent)
	: AMControlMoveAction(new AMControlMoveActionInfo(
							  AMControlInfo(control ? control->name() : "internalControl",
											setpoint,
											control ? control->minimumValue() : 0,
											control ? control->maximumValue() : 0,
											control ? control->units() : "",
											control ? control->tolerance() : 0,
											control ? control->description() : "[Internal Use Only]")),
						  parent) {
	internalControl_ = control;
}

bool AMInternalControlMoveAction::setSetpoint(double setpoint)
{
	if(state() != AMAction::Constructed)
		return false;

	controlMoveInfo()->setSetpoint(setpoint);
	return true;
}
