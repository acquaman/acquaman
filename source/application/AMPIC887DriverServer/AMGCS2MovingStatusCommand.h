#ifndef AMGCS2MOVINGSTATUSCOMMAND_H
#define AMGCS2MOVINGSTATUSCOMMAND_H

#include <QList>
#include <QFlags>
#include "AMGCS2Command.h"
#include "AMGCS2.h"

/*!
  * A command which represents a query to a PI C887.11 controller about the movement
  * statuses of its axes.
  */
class AMGCS2MovingStatusCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a moving status command to query the status of the
	  * controller axes.
	  */
	AMGCS2MovingStatusCommand();

	/*!
	  *  Virtual moving status command destructor.
	  */
	virtual ~AMGCS2MovingStatusCommand() {}

	/*!
	  * A set of flags which describe the movement statuses of the axes of the
	  * controller. If the axis movement statuses cannot be returned (if the command
	  * failed or has not yet been run) then the flag UnknownAxisMovementStatus
	  * will be true.
	  */
	QFlags<AMGCS2::AxisMovementStatus> movementStatuses();

	/*!
	  * Returns a stringified version of the axes movement statuses.
	  */
	virtual QString outputString() const;
protected:

	/*!
	  * Defines the steps taken to run a query on the motion status of axes on the
	  * controller.
	  */
	virtual bool runImplementation();

	QFlags<AMGCS2::AxisMovementStatus> movementStatuses_;
};

#endif // AMGCS2MOVINGSTATUSCOMMAND_H
