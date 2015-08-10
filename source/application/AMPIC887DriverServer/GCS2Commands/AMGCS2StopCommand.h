#ifndef AMGCS2STOPCOMMAND_H
#define AMGCS2STOPCOMMAND_H
#include "AMGCS2Command.h"

/*!
  * A command which represents an instruction to a PI C887.11 Controller to stop
  * all axes immediates and set its error code to 10 (Controller was stopped by
  * command).
  */
class AMGCS2StopCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a stop command, ready to be issued to a controller.
	  */
    AMGCS2StopCommand();

	/*!
	  * Virtual destructor for a stop command.
	  */
	virtual ~AMGCS2StopCommand() {}

protected:
	/*!
	  * Defines the steps taken to issue a stop all command to the controller.
	  */
	virtual bool runImplementation();

	QString statusString_;
};

#endif // AMGCS2STOPCOMMAND_H
