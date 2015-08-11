#ifndef AMGCS2GETREFERENCERESULTCOMMAND_H
#define AMGCS2GETREFERENCERESULTCOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * A command representing a query to obtain the reference status of the provided
  * axes to a controller. Move commands cannot be issued to axes which have not
  * yet had a reference move performed.
  */
class AMGCS2GetReferenceResultCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get reference result command which will report
	  * the referenced status of the provided axes.
	  * \param axes ~ An optional list of the axes whose reference status is to
	  * be returned. If none is provided the command will request the reference
	  * status of all axes.
	  */
	AMGCS2GetReferenceResultCommand(
			const AMPIC887AxisCollection axesToQuery = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a get reference results command
	  */
	virtual ~AMGCS2GetReferenceResultCommand() {}

	/*!
	  * A mapping of the axes provided in the initialization of the command to their
	  * current reference status if the command was run successfully, otherwise
	  * an empty map.
	  */
	AMPIC887AxisMap<bool> axesReferenceResults() const;
protected:
	/*!
	  * Ensures that the arugments passed to the command are valid:
	  *  - Ensures that all the axes the command was initialized with are not the
	  *    UnknownAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the reference status of the axes from
	  * a controller.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToQuery_;
	AMPIC887AxisMap<bool> axisReferenceResults_;
};

#endif // AMGCS2GETREFERENCERESULTCOMMAND_H
