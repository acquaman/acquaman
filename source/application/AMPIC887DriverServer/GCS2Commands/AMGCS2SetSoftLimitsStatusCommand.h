#ifndef AMGCS2SETSOFTLIMITSSTATUSCOMMAND_H
#define AMGCS2SETSOFTLIMITSSTATUSCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMPIC887AxisMap.h"
#include "../AMGCS2.h"
/*!
  * A command representing an instruction to a PI C887.11 controller to activate/
  * deactivate the soft limits on the provided axes.
  */
class AMGCS2SetSoftLimitsStatusCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set soft limits status command which will activate
	  * /deactivate the soft limits based on the provided mapping of axis to
	  * soft limit active status
	  * \param softLimitStatuses ~ A mapping of axes to the active status of its
	  * soft limit.
	  */
	AMGCS2SetSoftLimitsStatusCommand(const AMPIC887AxisMap<bool>& softLimitStatuses);

	/*!
	  * Virtual destructor for a set soft limit status command
	  */
	virtual ~AMGCS2SetSoftLimitsStatusCommand() {}

protected:
	/*!
	  * Ensures that the arguments passed to the command are valid:
	  *  - Ensures that the softLimitStatuses is not empty.
	  *  - Ensures that none of the axes in softLimitStatuses are the UnknownAxis
	  *  - Ensures that the number of axes provided does not exceed the total number
	  *    of axes.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to instruct a controller to set the active status
	  * of the axes soft limits.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<bool> softLimitStatuses_;
};

#endif // AMGCS2SETSOFTLIMITSSTATUSCOMMAND_H
