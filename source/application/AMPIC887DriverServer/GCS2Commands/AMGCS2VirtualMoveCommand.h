#ifndef AMGCS2VIRTUALMOVECOMMAND_H
#define AMGCS2VIRTUALMOVECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"
/*!
  * \brief A command representing a query to a PI C887.11 controller for whether
  * it is safe from the current platform position to approach the given axis
  * target positions.
  *
  * NOTE: The AMGCS2MoveCommand automatically performs this check before moving,
  * as such it is not neccessary to use this command before moving. This command
  * can be used to check whether it is safe to perform a move while guaranteeing
  * that no move will be performed if it is safe.
  */
class AMGCS2VirtualMoveCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a virtual move command which will check whether
	  * the provided axes positions are approachable.
	  */
	AMGCS2VirtualMoveCommand(const AMPIC887AxisMap<double>& axisPositions);

	/*!
	  * Virtual destructor for a virtual move command
	  */
	virtual ~AMGCS2VirtualMoveCommand() {}

	/*!
	  * Whether the axis positions which the command was initialized with are
	  * safe to approach.
	  * \returns True if the command was run successfully and the axis positions
	  * which the command was initialized with are safe to approach, false otherwise.
	  */
	bool isMoveSafe() const;

protected:
	/*!
	  * Ensures that the arguments which the command was initialized with are valid:
	  *  - Ensures that the map of axis positions is not empty
	  *  - Ensures that the map of axis positions does not contain the UknownAxis
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query whether the axis positions which the command
	  * was initialized with are safe to approach.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<double> axisPositions_;
	bool isMoveSafe_;
};

#endif // AMGCS2VIRTUALMOVECOMMAND_H
