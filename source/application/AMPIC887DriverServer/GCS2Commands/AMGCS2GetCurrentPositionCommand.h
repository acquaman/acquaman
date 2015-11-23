#ifndef AMGCS2GETCURRENTPOSITIONCOMMAND_H
#define AMGCS2GETCURRENTPOSITIONCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"
#include "../AMGCS2.h"

/*!
  * A class which represents a query to a PI C887.11 controller to request the
  * current actual positions of the controlled hexapod.
  */
class AMGCS2GetCurrentPositionCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a position query command.
	  * \param axesToQuery ~ An optional list of the axes whose positions are to
	  * be queried. If none if provided the positions of all axes will be queried.
	  */
	AMGCS2GetCurrentPositionCommand(
			const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a position query command.
	  */
	virtual ~AMGCS2GetCurrentPositionCommand() {}

	/*!
	  * A mapping of axis to their current positions as reported by the controller
	  * the last time the command was run. If the command has not been run, or was
	  * not successful when last run, then an empty map is returned.
	  */
	AMPIC887AxisMap<double> axisPositions();

protected:

	/*!
	  * Validates the commands input arguments. Specifically ensures that:
	  *  - All axes specified in axesToQuery are known axes.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken in order to run a position query on the controller.
	  */
	virtual bool runImplementation();

	/// The map of axes to positions returned by the controller when run was called.
	AMPIC887AxisMap<double> axisPositions_;

	/// A list of the axes whose positions are to be queried.
	AMPIC887AxisCollection axesToQuery_;
};

#endif // AMGCS2GETCURRENTPOSITIONCOMMAND_H
