#ifndef AMGCS2REFERENCEMOVECOMMAND_H
#define AMGCS2REFERENCEMOVECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
/*!
  * A command representing a request to a controller to perform a reference move
  * on the provided list of axes.
  *
  * NOTE: The success of this command indicates that the command has been able to
  * INSTRUCT the controller to begin the reference move, not that the reference
  * move has been successfully completed. In order to check that the reference move
  * has finished use the AMGCS2GetControllerReadyStatusCommand. To check that the
  * reference move was completed successfully use the AMGCS2GetReferencingResult
  * command.
  */
class AMGCS2ReferenceMoveCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a reference move command which is ready to instruct
	  * a controller to perform a reference move on the provided axes.
	  * \param axesToReference ~ An optional list of axes on which the reference move will
	  * be performed. If none are provided the controller will be instructed to
	  * perform a reference move across all axes.
	  */
	AMGCS2ReferenceMoveCommand(const AMPIC887AxisCollection& axesToReference = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a reference move command.
	  */
	virtual ~AMGCS2ReferenceMoveCommand() {}

protected:

	/*!
	  * Ensures that the arguments passed to the command are valid:
	  *  - Ensures that all axes in initialization list are not UnknownAxis.
	  *  - Ensures no duplicates axes are specified.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps to take in order to instruct a controller to perform
	  * a reference move.
	  */
	virtual bool runImplementation();


	AMPIC887AxisCollection axesToReference_;
};

#endif // AMGCS2REFERENCEMOVECOMMAND_H
