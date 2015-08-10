#ifndef AMGCS2GETPOSITIONUNITSCOMMAND_H
#define AMGCS2GETPOSITIONUNITSCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"
/*!
  * A command representing a query for a PI C887.11 controller to report the
  * position units of the provided axes.
  */
class AMGCS2GetPositionUnitsCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get position units command which will query the
	  * position units of the provided axes.
	  * \param axesToQuery ~ An optional list of the axes whose position units are to be
	  * queried. If none is provided the position units of all axes will be queried.
	  */
	AMGCS2GetPositionUnitsCommand(
			const AMPIC887AxisCollection& axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get position units command.
	  */
	virtual ~AMGCS2GetPositionUnitsCommand() {}

	/*!
	  * A mapping of axes which the command was initialized with to its position
	  * units.
	  * \returns A mapping of axis to position units if the command was run successfully,
	  * an empty hash otherwise.
	  */
	AMPIC887AxisMap<AMGCS2::PositionUnits> axesUnits() const;

protected:
	/*!
	  * Ensures the arguments the command was initialized with are valid:
	  *  - Ensures that none of the axes in the initialized list are the UnknownAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the position units from the controller.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<AMGCS2::PositionUnits> axesUnits_;
};

#endif // AMGCS2GETPOSITIONUNITSCOMMAND_H
