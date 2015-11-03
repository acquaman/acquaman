#ifndef AMGCS2GETRECORDRATECOMMAND_H
#define AMGCS2GETRECORDRATECOMMAND_H

#include "AMGCS2Command.h"

/*!
  * A command representing a query to a PI C887.11 controller for its current
  * record rate. The record rate is given in terms of the controller clock cycle
  * which is, by default, 20 microseconds (us).
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
	  * The retrieved record rate for the controller, in controller cycles (20us).
	  * If the command has not yet been run successfully this will return 0.
	  */
	double recordRate() const;

protected:

	/*!
	  * Defines the steps taken to query the record rate from the controller.
	  */
	virtual bool runImplementation();

	double recordRate_;
};

#endif // AMGCS2GETRECORDRATECOMMAND_H
