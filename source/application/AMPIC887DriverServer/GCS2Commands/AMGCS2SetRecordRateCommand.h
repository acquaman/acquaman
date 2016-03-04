#ifndef AMGCS2SETRECORDRATECOMMAND_H
#define AMGCS2SETRECORDRATECOMMAND_H

#include "AMGCS2Command.h"

/*!
  * A command representing a request for a PI C887.11 controller to set the record
  * rate for obtaining position data during a move. The record rate given in hertz
  */
class AMGCS2SetRecordRateCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set record rate command which will set the record
	  * rate to the provided value.
	  * \param recordRate ~ The record rate to set the controller to.
	  */
	AMGCS2SetRecordRateCommand(double recordRate);

	/*!
	  * Virtual destructor for an set record rate command.
	  */
	virtual ~AMGCS2SetRecordRateCommand() {}

protected:

	/*!
	  * Ensures that the arguments passed to the command are valid:
	  *  - Ensures that the record rate is < 1000000 cycles (0.05 Hz)
	  *  - Ensures that the record rate is > 1 cycle (50000 Hz)
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to set the record rate on a controller.
	  */
	virtual bool runImplementation();

	/*!
	  * Helper funciton which converts the record rate in hertz, to cycles (which
	  * is required by the controller).
	  */
	int recordRateToCycles(double recordRate);

	int cycleRate_;
};

#endif // AMGCS2SETRECORDRATECOMMAND_H
