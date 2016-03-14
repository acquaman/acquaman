#ifndef AMGCS2SETPIVOTPOINTCOMMAND_H
#define AMGCS2SETPIVOTPOINTCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing an instruction to a PI C887.11 controller to update
  * the pivot point of its rotational axes to the provided point.
  */
class AMGCS2SetPivotPointCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set pivot point command which will set the
	  * pivot point of a controller based on the provided map of axes to coordinate.
	  * \param pivotPoints ~ A mapping of axes to its corresponding pivot point
	  * position. Only X, Y and Z axes are valid.
	  */
	AMGCS2SetPivotPointCommand(const AMPIC887AxisMap<double>& pivotPoints);

	/*!
	  * Virtual destructor for a set pivot point command
	  */
	virtual ~AMGCS2SetPivotPointCommand() {}

protected:
	/*!
	  * Ensures that the arguments which the command was initialized with are valid:
	  *  - Ensures that the provided pivot point map is not empty.
	  *  - Ensures that the provided pivot point map does not contain more than
	  *    than the possible number of axes (3).
	  *  - Ensures that none of the provided axes are the UnknownAxis, the UAxis,
	  *    the VAxis or the WAxis.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to instruct a controller to set its pivot point
	  * based on the values which the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> pivotPoints_;
};

#endif // AMGCS2SETPIVOTPOINTCOMMAND_H
