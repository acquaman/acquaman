#ifndef AMGCS2SETLOWSOFTLIMITSCOMMAND_H
#define AMGCS2SETLOWSOFTLIMITSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"
/*!
  * A command representing an instruction to a PI C887.11 controller to set the
  * soft low limits of the provided axes.
  */
class AMGCS2SetLowSoftLimitsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set low soft limits command which will set the
	  * low soft limits of the provided axes to the provided values.
	  * \param axisLowLimits ~ A mapping of axes to the values which their low
	  * soft limits will be set to.
	  */
	AMGCS2SetLowSoftLimitsCommand(const AMPIC887AxisMap<double>& axisLowLimits);

	/*!
	  * Virtual destructor for a set low soft limits command.
	  */
	virtual ~AMGCS2SetLowSoftLimitsCommand() {}

protected:
	/*!
	  * Ensures the validity of the arguments which the command was initialized
	  * with:
	  *  - Ensures that none of the axes in the axisLowLimits map are the
	  *    UnknownAxis.
	  *  - Ensures no duplicates axes are specified.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to instruct a controller to set the soft lower
	  * limits of the axes to the values which the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> axisLowLimits_;
};

#endif // AMGCS2SETLOWSOFTLIMITSCOMMAND_H
