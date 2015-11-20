#ifndef SGMACTIONSUPPORT_H
#define SGMACTIONSUPPORT_H

#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

/// This namespace provides some convenience methods for using AMActions specific to SGM.
namespace SGMActionSupport
{
	/// Helper method that takes an AMControl and a desired setpoint and creates all necessary components.  Caller is responsible for memory.
	inline AMAction3 *buildSampleChamberVacuumMoveAction(double setpoint)
	{
		SGMSampleChamberVacuumMoveActionInfo *actionInfo = new SGMSampleChamberVacuumMoveActionInfo(AMNumber(setpoint));
		SGMSampleChamberVacuumMoveAction *action = new SGMSampleChamberVacuumMoveAction(actionInfo);
		return action;
	}
}

#endif // SGMACTIONSUPPORT_H
