#ifndef AMGCS2ASYNCMOVERELATIVECOMMAND_H
#define AMGCS2ASYNCMOVERELATIVECOMMAND_H

#include "../AMPIC887AxisMap.h"
#include "AMGCS2AsyncCommand.h"
#include "../AMGCS2.h"
#include "AMGCS2MoveRelativeCommand.h"

/*!
  * A command representing an instruction for a controller to set the target
  * positions of its axis to their current target positions, plus the relative
  * values provided to the command.
  * Runs asynchronously.
  */
class AMGCS2AsyncMoveRelativeCommand : public AMGCS2AsyncCommand
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an asynchronous move relative command, which will
	  * instruct the controller to move the provided axes by the provided relative
	  * amounts.
	  */
	explicit AMGCS2AsyncMoveRelativeCommand(const AMPIC887AxisMap<double>& relativeAxisPositions);

	/*!
	  * Frees the resources owned by the asynchronous move relative command.
	  */
	virtual ~AMGCS2AsyncMoveRelativeCommand();

	/*!
	  * The relative target positions which were issued to the command on contruction.
	  */
	AMPIC887AxisMap<double> relativeTargetPositions() const;
protected:
	/*!
	  * Ensures that the arguments with which the command was constructed are valid:
	  *  - Ensures that none of the provided axes are the UnknownAxis.
	  *  - Ensures that the provided map of axis relative positions is not empty.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to perform a relative move command on the controller.
	  */
	virtual bool runImplementation();

	/*!
	  * Defines the checks to be made on the current running status of the relative
	  * move.
	  */
	virtual void checkRunningState();

	AMGCS2MoveRelativeCommand* moveRelativeCommand_;
	AMPIC887AxisMap<double> relativeAxisPositions_;
	AMPIC887AxisMap<double> originalTargetPositions_;
};

#endif // AMGCS2ASYNCMOVERELATIVECOMMAND_H
