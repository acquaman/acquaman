#ifndef AMGCS2GETCYCLETIMECOMMAND_H
#define AMGCS2GETCYCLETIMECOMMAND_H

#include "AMGCS2Command.h"

/*!
  * A command representing a query to a PI C887.11 controller for its current
  * cycle time.
  */
class AMGCS2GetCycleTimeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get cycle time command.
	  */
    AMGCS2GetCycleTimeCommand();

	/*!
	  * Virtual destructor for a get cycle time command.
	  */
	virtual ~AMGCS2GetCycleTimeCommand() {}

	/*!
	  * The cycle time from the controller.
	  * \returns The cycle time value reported by the controller if the command
	  * was run successfully, 0 otherwise.
	  */
	double cycleTime() const;

protected:

	/*!
	  * Defines the steps taken to query the cycle time from a controller.
	  */
	virtual bool runImplementation();

	double cycleTime_;
};

#endif // AMGCS2GETCYCLETIMECOMMAND_H
