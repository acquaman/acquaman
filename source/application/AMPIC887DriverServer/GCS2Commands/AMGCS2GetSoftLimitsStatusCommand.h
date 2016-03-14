#ifndef AMGCS2GETSOFTLIMITSSTATUSCOMMAND_H
#define AMGCS2GETSOFTLIMITSSTATUSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

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
	  * \param axesToQuery ~ An optional list of axes whose soft limit status is to be
	  * returned. If no axes are provided the soft limit status of all axes will
	  * be returned.
	  */
	AMGCS2GetSoftLimitsStatusCommand(const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get soft limits status command
	  */
	virtual ~AMGCS2GetSoftLimitsStatusCommand() {}

	/*!
	  * The soft limit statuses the controller responded with.
	  * \returns The soft limit statuses of the axes of the controller if the command
	  * was run successfully, an empty map otherwise.
	  */
	AMPIC887AxisMap<bool> softLimitStatuses() const;

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

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<bool> softLimitStatuses_;
};

#endif // AMGCS2GETSOFTLIMITSSTATUSCOMMAND_H
