#ifndef AMGCS2SETRECORDRATECOMMAND_H
#define AMGCS2SETRECORDRATECOMMAND_H

#include "AMGCS2Command.h"

/*!
  * A command representing a request for a PI C887.11 controller to set the record
  * rate for obtaining position data during a move. The record rate is defined in
  * controller cycles.
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
	  * Defines the steps taken to set the record rate on a controller.
	  */
	virtual bool runImplementation();

	double recordRate_;
};

#endif // AMGCS2SETRECORDRATECOMMAND_H
