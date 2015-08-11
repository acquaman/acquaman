#ifndef AMGCS2ASYNCMOVECOMMAND_H
#define AMGCS2ASYNCMOVECOMMAND_H
#include "AMGCS2AsyncCommand.h"

#include "../AMGCS2.h"
#include "../AMPIC887AxisMap.h"
#include "AMGCS2MoveCommand.h"

/*!
  * A command representing an instruction to a controller for it to move its axes.
  * This differs from the regular move command as it can be issued, and then the
  * succeeded and failed signals listened to, to be updated when the movement has
  * completed.
  */
class AMGCS2AsyncMoveCommand : public AMGCS2AsyncCommand
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an asynchronous move command which will move the
	  * provided axes to the provided positions.
	  * \param targetPositions ~ A mapping of axes to the positions which the
	  * command will move them towards.
	  */
	explicit AMGCS2AsyncMoveCommand(const AMPIC887AxisMap<double>& targetPositions);

	/*!
	  * Fress the resources owned by the asynchronous move command.
	  */
	virtual ~AMGCS2AsyncMoveCommand();

	/*!
	  * The target positions which the command was initially instructed to move
	  * the axes to.
	  */
	AMPIC887AxisMap<double> targetPositions() const;
protected:

	/*!
	  * Validates the input arguments to the command.
	  *  - Ensures that the map of axis positions provided to the command is not
	  *    empty.
	  *  - Ensures the map of axis positions provided to the command does not contain
	  *    the UnknownAxis.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps to take when the move command is run.
	  */
	virtual bool runImplementation();

	/*!
	  * Defines the steps taken to check whether the move command is still
	  * being performed.
	  */
	virtual void checkRunningState();

	AMGCS2MoveCommand* command_;
	AMPIC887AxisMap<double> targetPositions_;
};

#endif // AMGCS2ASYNCMOVECOMMAND_H
