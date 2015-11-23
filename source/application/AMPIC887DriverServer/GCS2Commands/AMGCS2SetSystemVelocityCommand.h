#ifndef AMGCS2SETSYSTEMVELOCITYCOMMAND_H
#define AMGCS2SETSYSTEMVELOCITYCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * A command representing an instruction to a PI C887.11 controller to update
  * its system velocity to the provided value.
  */
class AMGCS2SetSystemVelocityCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set system velocity command which will set the
	  * system velocity of a controller to the provided value.
	  */
	AMGCS2SetSystemVelocityCommand(double systemVelocity);

	/*!
	  * Virtual destructor for a set system velocity command.
	  */
	virtual ~AMGCS2SetSystemVelocityCommand() {}

protected:
	/*!
	  * Defines the steps taken to set the system velocity of a controller.
	  */
	virtual bool runImplementation();

	double systemVelocity_;
};

#endif // AMGCS2SETSYSTEMVELOCITYCOMMAND_H
