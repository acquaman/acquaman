#ifndef AMGCS2ASYNCMOVECOMMAND_H
#define AMGCS2ASYNCMOVECOMMAND_H
#include "AMGCS2AsyncCommand.h"

#include <QHash>
#include "../AMGCS2.h"
#include "AMGCS2MoveCommand.h"

class AMGCS2AsyncMoveCommand : public AMGCS2AsyncCommand
{
    Q_OBJECT
public:
	explicit AMGCS2AsyncMoveCommand(const QHash<AMGCS2::Axis, double>& axisPositions);

	virtual ~AMGCS2AsyncMoveCommand();

	/*!
	  * Validates the input arguments to the command.
	  *  - Ensures that the map of axis positions provided to the command is not
	  *    empty.
	  *  - Ensures no duplicates axes are specified.
	  */
	virtual bool validateArguments();

	/*!
	  * Returns "Movement Succeeded" if the movement has fully completed, otherwise
	  * returns the empty string.
	  */
	virtual QString outputString() const;

protected:

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
