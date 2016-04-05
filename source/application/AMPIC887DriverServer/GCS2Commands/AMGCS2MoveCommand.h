#ifndef AMGCS2MOVECOMMAND_H
#define AMGCS2MOVECOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A class which represents a move command in the PIC887.11 GCS2 syntac. Equivalent
  * to the MOV command.
  */
class AMGCS2MoveCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of an AMGCS2MoveCommand.
	  * \param targetPositions ~ A mapping of the axes to be moved to the positions
	  * to which they will be moved.
	  */
	AMGCS2MoveCommand(const AMPIC887AxisMap<double>& targetPositions);

	/*!
	  * Virtual destructor for an AMGCS2MoveCommand
	  */
	virtual ~AMGCS2MoveCommand() {}

protected:
	/*!
	  * Validates the input arguments to the command.
	  *  - Ensures that the map of axis positions provided to the command is not
	  *    empty.
	  *  - Ensures no duplicates axes are specified.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps to take when the move command is run.
	  */
	virtual bool runImplementation();

	/// A mapping of the axes to the positions they will be moved to.
	AMPIC887AxisMap<double> targetPositions_;
};

#endif // AMGCS2MOVECOMMAND_H
