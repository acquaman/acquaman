#ifndef AMGCS2SETSTEPSIZECOMMAND_H
#define AMGCS2SETSTEPSIZECOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing an instruction for a PI C887.11 controller to alter
  * the step sizes of axis to the provided values.
  */
class AMGCS2SetStepSizeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set step size command which will alter the step
	  * sizes of the provided axes to the provided values.
	  * \param axisStepSizes ~ A mapping of axes to new step size values
	  */
	AMGCS2SetStepSizeCommand(const AMPIC887AxisMap<double>& axisStepSizes);

	/*!
	  * Virtual destructor for a set step size command.
	  */
	virtual ~AMGCS2SetStepSizeCommand() {}

protected:
	/*!
	  * Ensures that the arguments which the command was initialized with are valid:
	  *  - Ensures that the axisStepSizes map is not empty
	  *  - Ensures that none of the axis in the axisStepSizes map are the UnknownAxis
	  *  - Ensures that the number of axes provided is not greater than the total
	  *    number of axes.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps required for instructing a controller to alter the step
	  * sizes values based on the mapping the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> axisStepSizes_;
};

#endif // AMGCS2SETSTEPSIZECOMMAND_H
