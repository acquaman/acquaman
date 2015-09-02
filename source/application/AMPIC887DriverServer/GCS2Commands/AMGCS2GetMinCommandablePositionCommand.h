#ifndef AMGCS2GETMINCOMMANDABLEPOSITIONCOMMAND_H
#define AMGCS2GETMINCOMMANDABLEPOSITIONCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing a query to a PI C887.11 controller to report the minimum
  * commandable position for the provided axes.
  */
class AMGCS2GetMinCommandablePositionCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get min commandable position command which will
	  * report the minimum commandable position of the provided axes.
	  * \param axesToQuery ~ An optional list of axes whose minimum commandable position
	  * will be queried. If none is provided the minimum commandable position of
	  * all axes will be queried.
	  */
	AMGCS2GetMinCommandablePositionCommand(const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for the get min commandable position command.
	  */
	virtual ~AMGCS2GetMinCommandablePositionCommand() {}

	/*!
	  * The reported minimum commandable positions of the axes the command was
	  * initialized with.
	  * \returns A map of axes to min commandable position if the command was run
	  * successfully, an empty hash otherwise.
	  */
	AMPIC887AxisMap<double> minCommandablePositions() const;

protected:
	/*!
	  * Ensures the arguments with which the command was initialzied are valid:
	  *  - Ensures that none of the axes in query list are the UnknownAxis
	  *  - Ensures that the number of provided axes is not greater than the total
	  *    number of axes.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the min commandable positions from a
	  * controller.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<double> minCommandablePositions_;
};

#endif // AMGCS2GETMINCOMMANDABLEPOSITIONCOMMAND_H
