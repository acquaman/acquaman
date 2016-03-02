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


#ifndef AMACTIONSUPPORT_H
#define AMACTIONSUPPORT_H

#include "actions3/AMAction3.h"
#include "beamline/AMControl.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/actions/AMChangeToleranceAction.h"
#include "actions3/actions/AMControlCalibrateAction.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/actions/AMTimeoutAction.h"

/// This namespace provides some convenience methods for using AMActions.
namespace AMActionSupport
{
	/// Helper method that takes an AMControl and a desired setpoint and creates all necessary components.  Caller is responsible for memory.
	inline AMAction3 *buildControlMoveAction(AMControl *control, double setpoint)
	{
		AMControlInfo info = control->toInfo();
		info.setValue(setpoint);
		AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(info);
		AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, control);
		return action;
	}

	/// Helper method same as above but includes AMControlMoveActionInfo3::setIsRelativeMove
	inline AMAction3 *buildControlMoveAction(AMControl *control, double setpoint, bool isRelative)
	{
		AMControlInfo info = control->toInfo();
		info.setValue(setpoint);
		AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(info);
		actionInfo->setIsRelativeMove(isRelative);
		AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, control);
		return action;
	}

	/// Helper method same as above but includes AMControlMoveActionInfo3::setIsRelativeMove and ::setIsRelativeFromSetpoint
	inline AMAction3 *buildControlMoveAction(AMControl *control, double setpoint, bool isRelative, bool isRelativeFromSetPoint)
	{
		AMControlInfo info = control->toInfo();
		info.setValue(setpoint);
		AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(info);
		actionInfo->setIsRelativeMove(isRelative);
		actionInfo->setIsRelativeFromSetpoint(isRelativeFromSetPoint);
		AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, control);
		return action;
	}

	/// Helper method that takes an AMControl and a desired setpoint and creates all necessary components.  Caller is responsible for memory.
	inline AMAction3 *buildControlWaitAction(AMControl *control, double setpoint, double timeout = 10, AMControlWaitActionInfo::MatchType matchType = AMControlWaitActionInfo::MatchEqual)
	{
		AMControlInfo info = control->toInfo();
		info.setValue(setpoint);
		AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(info, timeout, matchType);
		AMControlWaitAction *action = new AMControlWaitAction(actionInfo, control);
		return action;
	}

	/// Helper method that takes an AMControl and a desired tolerance and returns a valid AMChangeToleranceAction.  Caller is responsible for memory.
	inline AMAction3 *buildChangeToleranceAction(AMControl *control, double tolerance)
	{
		AMControlInfo info = control->toInfo();
		info.setTolerance(tolerance);
		AMChangeToleranceActionInfo *actionInfo = new AMChangeToleranceActionInfo(info, tolerance);
		AMChangeToleranceAction *action = new AMChangeToleranceAction(actionInfo, control);
		return action;
	}

	/// Helper method that takes an AMControl and two values (oldValue, newValue), and returna a valid AMControlCalibrateAction. Caller is responsible for memory.
	inline AMAction3 *buildControlCalibrateAction(AMControl *control, double oldValue, double newValue)
	{
		AMControlInfo info = control->toInfo();
		AMControlCalibrateActionInfo *actionInfo = new AMControlCalibrateActionInfo(info, AMNumber(oldValue), AMNumber(newValue));
		AMControlCalibrateAction *action = new AMControlCalibrateAction(actionInfo, control);
		return action;
	}

	/// Helper method that takes a time interval in seconds, and returns a new AMWaitAction. Caller is responsible for memory.
	inline AMAction3 *buildWaitAction(double timeSeconds)
	{
		AMWaitActionInfo *actionInfo = new AMWaitActionInfo(timeSeconds);
		AMWaitAction *action = new AMWaitAction(actionInfo);
		return action;
	}

	/// Helper method that takes a time interval in seconds and a description of what the action is waiting for, and returns a new AMTimeoutAction. Caller is responsible for memory.
	inline AMAction3 *buildTimeoutAction(double timeSeconds, const QString &description)
	{
		AMTimeoutActionInfo *actionInfo = new AMTimeoutActionInfo(timeSeconds, description);
		AMTimeoutAction *action = new AMTimeoutAction(actionInfo);
		return action;
	}
}

#endif // AMACTIONSUPPORT_H
