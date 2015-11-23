#ifndef AMGCS2SETHIGHSOFTLIMITSCOMMAND_H
#define AMGCS2SETHIGHSOFTLIMITSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"
/*!
  * A command representing an instruction to a PI C887.11 controller to set the
  * soft high limits of the provided axes.
  */
class AMGCS2SetHighSoftLimitsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set high soft limits command which will set the
	  * high soft limits of the provided axes to the provided values.
	  * \param axisHighLimits ~ A mapping of axes to the values which their high
	  * soft limits will be set to.
	  */
	AMGCS2SetHighSoftLimitsCommand(const AMPIC887AxisMap<double>& axisHighLimits);

	/*!
	  * Virtual destructor for a set high soft limits command.
	  */
	virtual ~AMGCS2SetHighSoftLimitsCommand() {}

protected:
	/*!
	  * Ensures the validity of the arguments which the command was initialized
	  * with:
	  *  - Ensures that none of the axes in the axisHighLimits map are the
	  *    UnknownAxis.
	  *  - Ensures no duplicates axes are specified.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to instruct a controller to set the soft high
	  * limits of the axes to the values which the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> axisHighLimits_;
};

#endif // AMGCS2SETHIGHSOFTLIMITSCOMMAND_H
