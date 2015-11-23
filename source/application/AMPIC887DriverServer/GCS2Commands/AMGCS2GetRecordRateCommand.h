#ifndef AMGCS2GETRECORDRATECOMMAND_H
#define AMGCS2GETRECORDRATECOMMAND_H

#define SECS_PER_CYCLE 2e-5
#include "AMGCS2Command.h"

/*!
  * A command representing a query to a PI C887.11 controller for its current
  * record rate. The record rate is given in hertz.
  */
class AMGCS2GetRecordRateCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of the get record rate command
	  */
    AMGCS2GetRecordRateCommand();

	/*!
	  * Virtual destructor for a get record rate command
	  */
	virtual ~AMGCS2GetRecordRateCommand() {}

	/*!
	  * The retrieved record rate for the controller, in hertz.
	  * If the command has not yet been run successfully this will return 0.
	  */
	double recordRate() const;

protected:

	/*!
	  * Defines the steps taken to query the record rate from the controller.
	  */
	virtual bool runImplementation();

	/*!
	  * Helper function which takes the record rate value in cycles (as defined by
	  * the controller) and converts it into a hertz value.
	  */
	double cyclesToRecordRate(int cycles);

	double recordRate_;
};

#endif // AMGCS2GETRECORDRATECOMMAND_H
