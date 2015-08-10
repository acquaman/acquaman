#ifndef AMGCS2GETCONTROLLERREADYSTATUSCOMMAND_H
#define AMGCS2GETCONTROLLERREADYSTATUSCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * A command representing a request to the controller for its ready status.
  */
class AMGCS2GetControllerReadyStatusCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a controller ready status command.
	  */
	AMGCS2GetControllerReadyStatusCommand();

	/*!
	  * Virtual destructor for the controller ready status command.
	  */
	virtual ~AMGCS2GetControllerReadyStatusCommand() {}

	/*!
	  * Whether the controller is ready or not. If the command has not been run
	  * or was not run successfully the isReady status will be false.
	  */
	bool isReady() const;

protected:
	/*!
	  * Defines the steps taken to ask the controller for its ready status.
	  * \returns True if the ready status of the controller could be determined,
	  * false otherwise.
	  */
	virtual bool runImplementation();

	bool isReady_;
};

#endif // AMGCS2GETCONTROLLERREADYSTATUSCOMMAND_H
