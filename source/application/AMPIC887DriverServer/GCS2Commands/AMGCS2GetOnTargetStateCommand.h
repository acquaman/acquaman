#ifndef AMGCS2GETONTARGETSTATECOMMAND_H
#define AMGCS2GETONTARGETSTATECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"
/*!
  * Command representing a query to a PI C887.11 controller to report whether the
  * provided axes have reached their targets.
  */
class AMGCS2GetOnTargetStateCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get on target state command which will report whether
	  * the provided axes have reached their target positions.
	  * \param axesToQuery ~ An optional list of axes whose on target state is to be reported,
	  * if none is provided the on target state of all axes will be returned.
	  */
	AMGCS2GetOnTargetStateCommand(const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get on target state command
	  */
	virtual ~AMGCS2GetOnTargetStateCommand() {}

	/*!
	  * A mapping of the axes which the command was initialized with to its on
	  * target state reported by the controller.
	  * \returns A mapping axes to on target state if the command was run successfully,
	  * an empty map otherwise.
	  */
	AMPIC887AxisMap<bool> onTargetStates() const;

protected:
	/*!
	  * Ensures the arguments with which the command was initialized are valid:
	  *  - Ensures that none of the axes which the command was initialized with
	  *    are the UnknownAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the on target states of the axes which
	  * the command was initialized with.
	  */
	virtual bool runImplementation();

	AMPIC887AxisMap<bool> onTargetStates_;
	AMPIC887AxisCollection axesToQuery_;
};

#endif // AMGCS2GETONTARGETSTATECOMMAND_H
