#ifndef AMGCS2GETLIMITSWITCHSTATUSCOMMAND_H
#define AMGCS2GETLIMITSWITCHSTATUSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing a query to a C887.11 controller for it to report whether
  * the provided axes have limit switches.
  */
class AMGCS2GetLimitSwitchStatusCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a get limit switch status command which will
	  * report whether the provided axes have limit switches.
	  * \param axesToQuery ~ An optional list of the axes whose limit switch status is
	  * to be reported. If none is provided the limit switch status of all axes
	  * is to be returned.
	  */
	AMGCS2GetLimitSwitchStatusCommand(
			const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get limit switch status command.
	  */
	virtual ~AMGCS2GetLimitSwitchStatusCommand() {}

	/*!
	  * The limit switch statuses of the axes which the command was initialized
	  * with, if the command was run succesfully, an empty map otherwise.
	  */
	AMPIC887AxisMap<bool> limitSwitchStatuses() const;

protected:
	/*!
	  * Ensures that the arguments the command was initialized with are valid:
	  *  - Ensures that none of the axis in the initialized list are the UnknownAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the limit switch statuses of the axes
	  * the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<bool> limitSwitchStatuses_;
};

#endif // AMGCS2GETLIMITSWITCHSTATUSCOMMAND_H
