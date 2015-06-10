#ifndef AMGCS2CONTROLLERREADYSTATUSCOMMAND_H
#define AMGCS2CONTROLLERREADYSTATUSCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * A command representing a request to the controller for its ready status.
  */
class AMGCS2ControllerReadyStatusCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a controller ready status command.
	  */
    AMGCS2ControllerReadyStatusCommand();

	/*!
	  * Virtual destructor for the controller ready status command.
	  */
	virtual ~AMGCS2ControllerReadyStatusCommand() {}

	/*!
	  * Whether the controller is ready or not. If the command has not been run
	  * or was not run successfully the isReady status will be false.
	  */
	bool isReady() const;

	/*!
	  * Returns a stringified form of the controllers ready status.
	  */
	virtual QString outputString() const;
protected:
	/*!
	  * Defines the steps taken to ask the controller for its ready status.
	  * \returns True if the ready status of the controller could be determined,
	  * false otherwise.
	  */
	virtual bool runImplementation();

	bool isReady_;
};

#endif // AMGCS2CONTROLLERREADYSTATUSCOMMAND_H
