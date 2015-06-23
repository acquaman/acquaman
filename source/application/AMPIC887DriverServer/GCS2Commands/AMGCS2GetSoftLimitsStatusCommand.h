#ifndef AMGCS2GETSOFTLIMITSSTATUSCOMMAND_H
#define AMGCS2GETSOFTLIMITSSTATUSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include <QList>
#include <QHash>

/*!
  * A command representing a query to a PI C887.11 controller to return the current
  * active status of the specified axes' soft limits.
  */
class AMGCS2GetSoftLimitsStatusCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get soft limits status command which will respond
	  * with the active status of the provided axes.
	  * \param axes ~ An optional list of axes whose soft limit status is to be
	  * returned. If no axes are provided the soft limit status of all axes will
	  * be returned.
	  */
	AMGCS2GetSoftLimitsStatusCommand(const QList<AMGCS2::Axis>& axes = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for a get soft limits status command
	  */
	virtual ~AMGCS2GetSoftLimitsStatusCommand() {}

	/*!
	  * A human readable string representation of the queried axes' soft limit
	  * statuses.
	  * \returns Stringified soft limit status if the command was run successfully,
	  * the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * The soft limit statuses the controller responded with.
	  * \returns The soft limit statuses of the axes of the controller if the command
	  * was run successfully, an empty map otherwise.
	  */
	QHash<AMGCS2::Axis, bool> softLimitStatuses() const;

protected:
	/*!
	  * Ensures that the arguments provided to the command are valid:
	  *  - Ensures that none of the axes specified when the command was initialized
	  *    are the UnknownAxis
	  *  - Ensures that the number of axis specified does not exceed the total number
	  *    of axes.
	  */
	bool validateArguments();

	/*!
	  * Defines the steps taken to query the soft limit statuses of the axes  with
	  * which the command was initialized.
	  */
	bool runImplementation();

	QList<AMGCS2::Axis> axesToQuery_;
	QHash<AMGCS2::Axis, bool> softLimitStatuses_;
};

#endif // AMGCS2GETSOFTLIMITSSTATUSCOMMAND_H
