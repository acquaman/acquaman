#ifndef AMACTIONSUPPORT_H
#define AMACTIONSUPPORT_H

#include "actions3/AMAction3.h"
#include "beamline/AMControl.h"
#include "actions3/actions/AMControlMoveAction3.h"

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
}

#endif // AMACTIONSUPPORT_H
