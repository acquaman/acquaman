#ifndef AMGCS2ASYNCMOVECOMMAND_H
#define AMGCS2ASYNCMOVECOMMAND_H
#include "AMGCS2AsyncCommand.h"

#include <QHash>
#include "../AMGCS2.h"
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
	  * \param axisPositions ~ A mapping of axes to the positions which the
	  * command will move them towards.
	  */
	explicit AMGCS2AsyncMoveCommand(const QHash<AMGCS2::Axis, double>& axisPositions);

	/*!
	  * Fress the resources owned by the asynchronous move command.
	  */
	virtual ~AMGCS2AsyncMoveCommand();

	/*!
	  * Returns "Movement Succeeded" if the movement has fully completed, otherwise
	  * returns the empty string.
	  */
	virtual QString outputString() const;

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
	virtual void isFinishedImplementation();

	AMGCS2MoveCommand* command_;
	QHash<AMGCS2::Axis, double> axesToMove_;
};

#endif // AMGCS2ASYNCMOVECOMMAND_H
