#ifndef AMGCS2CURRENTPOSITIONCOMMAND_H
#define AMGCS2CURRENTPOSITIONCOMMAND_H

#include "AMGCS2Command.h"
#include <QHash>
#include <QList>
#include "AMGCS2.h"

/*!
  * A class which represents a query to a PI C887.11 controller to request the
  * current actual positions of the controlled hexapod.
  */
class AMGCS2CurrentPositionCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a position query command.
	  * \param axesToQuery ~ An optional list of the axes whose positions are to
	  * be queried. If none if provided the positions of all axes will be queried.
	  */
	AMGCS2CurrentPositionCommand(const QList<AMGCS2::Axis>& axesToQuery = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for a position query command.
	  */
	virtual ~AMGCS2CurrentPositionCommand() {}

	/*!
	  * A mapping of axis to their current positions as reported by the controller
	  * the last time the command was run. If the command has not been run, or was
	  * not successful when last run, then an empty map is returned.
	  */
	QHash<AMGCS2::Axis, double> axisPositions();

	/*!
	  * A stringified form on the returned axis positions if the command
	  * has been successfully run, or the empty string otherwise.
	  */
	virtual QString outputString() const;
protected:

	/*!
	  * Validates the commands input arguments. Specifically ensures that:
	  *  - All axes specified in axesToQuery are known axes.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken in order to run a position query on the controller.
	  */
	virtual bool runImplementation();

	/// The map of axes to positions returned by the controller when run was called.
	QHash<AMGCS2::Axis, double> axisPositions_;

	/// A list of the axes whose positions are to be queried.
	QList<AMGCS2::Axis> axesToQuery_;
};

#endif // AMGCS2CURRENTPOSITIONCOMMAND_H
