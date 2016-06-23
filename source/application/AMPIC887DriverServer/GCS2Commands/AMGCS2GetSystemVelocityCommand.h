#ifndef AMGCS2GETSYSTEMVELOCITYCOMMAND_H
#define AMGCS2GETSYSTEMVELOCITYCOMMAND_H
#include "AMGCS2Command.h"

/*!
  * A command representing a query to a PI C887.11 controller to report its
  * current system velocity in mm/s.
  */
class AMGCS2GetSystemVelocityCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get system velocity command ready to run on a
	  * controller.
	  */
    AMGCS2GetSystemVelocityCommand();

	/*!
	  * Virtaul destructor for a get system velocity command.
	  */
	virtual ~AMGCS2GetSystemVelocityCommand() {}

	/*!
	  * The reported system velocity in mm/s.
	  * \returns The reported system velocity if the command was run successfully,
	  * 0 otherwise.
	  */
	double systemVelocity() const;

protected:
	/*!
	  * Defines the steps taken to query a controller's system velocity.
	  */
	virtual bool runImplementation();

	double systemVelocity_;
};

#endif // AMGCS2GETSYSTEMVELOCITYCOMMAND_H
