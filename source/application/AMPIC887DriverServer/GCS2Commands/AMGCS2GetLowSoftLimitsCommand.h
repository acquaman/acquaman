#ifndef AMGCS2GETLOWSOFTLIMITSCOMMAND_H
#define AMGCS2GETLOWSOFTLIMITSCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * Command representing a query to a PI C887.11 controller for the soft lower
  * limits of the provided axes.
  */
class AMGCS2GetLowSoftLimitsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get low soft limits command which which will
	  * query the soft lower limits of the provided axes.
	  * \param axesToQuery ~ An optional list of axes whose soft lower limits are to be
	  * queried. If none is provided the soft lower limits of all axes will be
	  * queried.
	  */
	AMGCS2GetLowSoftLimitsCommand(
			const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get low soft limits command
	  */
	virtual ~AMGCS2GetLowSoftLimitsCommand() {}

	/*!
	  * The soft low limit values of the axes which the command was initialized.
	  * \returns The queried soft low limits values if the command was run successfully,
	  * an empty map otherwise.
	  */
	AMPIC887AxisMap<double> axesLowSoftLimits() const;

protected:
	/*!
	  * Ensures that the arguments provided to the command are valid:
	  *  - Ensures that none of the axes which the command was initialized with
	  *    are the UnknownAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query a controller for the soft low limits
	  * of the axes which the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<double> axesLowSoftLimits_;
};

#endif // AMGCS2GETLOWSOFTLIMITSCOMMAND_H
