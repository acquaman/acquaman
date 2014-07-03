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


#include "AMControlStopAction.h"

#include "beamline/AMControl.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

 AMControlStopAction::~AMControlStopAction(){}
AMControlStopAction::AMControlStopAction(AMControlStopActionInfo *info, AMControl *control, QObject *parent)
	: AMAction3(info, parent)
{
	if (control)
		control_ = control;

	else
		control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
}

AMControlStopAction::AMControlStopAction(const AMControlStopAction &other)
	: AMAction3(other)
{
	const AMControlStopActionInfo *info = qobject_cast<const AMControlStopActionInfo *>(other.info());

	if (info)
		control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
	else
		control_ = 0;
}

void AMControlStopAction::startImplementation()
{
	AMControlStopActionInfo *controlStopInfo = qobject_cast<AMControlStopActionInfo *>(info());
	const AMControlInfo& setpoint = *(controlStopInfo->controlInfo());

	// If you still don't have a control, check the exposed controls one last time.
	if (!control_)
		control_ = AMBeamline::bl()->exposedControlByInfo(setpoint);

	// Must have a control, and it must be able to move.
	if(!control_) {
		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_NO_CONTROL,
						  QString("There was an error moving the control '%1' into position, because the control was not found. Please report this problem to the Acquaman developers.").arg(setpoint.name()));
		setFailed();
		return;
	}

	// check we can stop...
	if(!control_->canStop()) {
		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_CANT_STOP,
						  QString("There was an error moving the control '%1' into position, because the control was not connected and ready to move. Please report this problem to the beamline staff.")
						  .arg(control_->name()));
		setFailed();
		return;
	}

	connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));

	bool errorOnStart = control_->stop();

	if (!errorOnStart){

		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_IMMEDIATE_FAILURE,
						  QString("There was an immediate error stopping control %1.").arg(control_->name()));
		setFailed();
		return;
	}
}

void AMControlStopAction::onMovingChanged(bool isMoving)
{
	// This is really bad.  The control should be moving already and then stopping... not restarting.
	if (isMoving){

		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_COULDNT_STOP_FAILURE,
						  QString("The control %1 was unable to stop.").arg(control_->name()));
		setFailed();
	}

	else
		setSucceeded();
}
