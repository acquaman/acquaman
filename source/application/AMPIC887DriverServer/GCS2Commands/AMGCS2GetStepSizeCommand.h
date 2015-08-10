#ifndef AMGCS2GETSTEPSIZECOMMAND_H
#define AMGCS2GETSTEPSIZECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing a query to a PI C887.11 controller to report the step
  * size of the provided axes.
  */
class AMGCS2GetStepSizeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get step size command which will report the step
	  * size of the provided axes.
	  * \param axes ~ An optional list of axes whose step sizes are to be queried.
	  * If none is provided the step size of all axes will be queried.
	  */
	AMGCS2GetStepSizeCommand(const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get step size command
	  */
	virtual ~AMGCS2GetStepSizeCommand() {}

	/*!
	  * A mapping of axis to its step size as reported by the controller.
	  * \returns A mapping of axis to step size if the command was run successfully,
	  * an empty hash otherwise.
	  */
	AMPIC887AxisMap<double> axisStepSizes() const;

protected:
	/*!
	  * Ensures that the arguments which the command was initialized with are valid:
	  *  - Ensures that none of the specified axes are the UnknownAxis.
	  *  - Ensures that the number of axes provided does not exceed the maximum.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the steps sizes of the axes which the
	  * command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<double> axisStepSizes_;
};

#endif // AMGCS2GETSTEPSIZECOMMAND_H
