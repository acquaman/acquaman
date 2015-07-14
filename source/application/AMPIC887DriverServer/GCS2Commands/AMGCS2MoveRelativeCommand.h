#ifndef AMGCS2MOVERELATIVECOMMAND_H
#define AMGCS2MOVERELATIVECOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"
/*!
  * A command representing an instruction to a PI C887.11 controller to move the
  * provided axes to their corresponsing relative positions.
  */
class AMGCS2MoveRelativeCommand : public AMGCS2Command
{
public:
	/*!
	  * \brief Creates a new instance of a move relative command which will set
	  * the new target positions based on the provided mapping of axes to positions.
	  *
	  * The new target positions are calculated by adding the given position values
	  * to the last commanded target value. Axes will start moving to the new position
	  * if ALL given targets are within the allowed range and All axes can move.
	  * \param relativeAxisPositons ~ A mapping of axes to the values which are
	  * to be added to their last commanded target positions.
	  */
	AMGCS2MoveRelativeCommand(const AMPIC887AxisMap<double>& relativeAxisPositions);

	/*!
	  * Virtual destructor for a move relative command.
	  */
	virtual ~AMGCS2MoveRelativeCommand() {}

protected:
	/*!
	  * Ensures that the arguments provided to the move relative command are valid:
	  *  - Ensures that the provided axis/position mapping is not empty
	  *  - Ensures than none of the axes in the axis/position mapping are the
	  *    UnknownAxis
	  *  - Ensures no duplicates axes are specified.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps required for instructing a controller to perform a relative
	  * move command based on the axis/position mapping that the command was initialized
	  * with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> relativeAxisPositions_;
};

#endif // AMGCS2MOVERELATIVECOMMAND_H
