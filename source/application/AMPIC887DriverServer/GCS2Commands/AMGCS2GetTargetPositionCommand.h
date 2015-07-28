#ifndef AMGCS2GETTARGETPOSITIONCOMMAND_H
#define AMGCS2GETTARGETPOSITIONCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing a query for the target positions which the provided
  * axes are currently moving towards.
  *
  * NOTE: If the actual axis positions are required the AMGCS2GetCurrentPositionCommand
  * should be used.
  */
class AMGCS2GetTargetPositionCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get target position command which will report
	  * the target positions of the provided axes.
	  * \param axesToQuery ~ Optional list of axes whose target positions will be returned.
	  * If none is provided the target positions of all axes will be returned.
	  */
	AMGCS2GetTargetPositionCommand(const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get target position command.
	  */
	virtual ~AMGCS2GetTargetPositionCommand() {}

	/*!
	  * The axis target positions returned by the command if the command was run
	  * successfully, an empty map otherwise.
	  */
	AMPIC887AxisMap<double> axisTargetPositions() const;

protected:
	/*!
	  * Ensures the arguments for the command are valid:
	  *  - Ensures that none of the axes in the list the command was initialized
	  *    with are the UnknownAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the target positions of the axes the command
	  * was initialized with from a controller.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> axisTargetPositions_;
	AMPIC887AxisCollection axesToQuery_;
};

#endif // AMGCS2GETTARGETPOSITIONCOMMAND_H
