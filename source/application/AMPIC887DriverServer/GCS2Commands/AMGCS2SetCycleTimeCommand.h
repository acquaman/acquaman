#ifndef AMGCS2SETCYCLETIMECOMMAND_H
#define AMGCS2SETCYCLETIMECOMMAND_H
#include "AMGCS2Command.h"

/*!
  * A command representing a request for a PI C887.11 controller to set the cycle
  * time for running a defined motion profile.
  */
class AMGCS2SetCycleTimeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set cycle time command which will set the cycle
	  * time of the controller to the provided value.
	  * \param cycleTime ~ The value to set the controller's cycle time to.
	  */
	AMGCS2SetCycleTimeCommand(double cycleTime);

	/*!
	  * Virtual destructor for an set cycle time command.
	  */
	virtual ~AMGCS2SetCycleTimeCommand() {}

protected:
	/*!
	  * Defines the steps taken to set the cycle time on a controller.
	  */
	virtual bool runImplementation();

	double cycleTime_;
};

#endif // AMGCS2SETCYCLETIMECOMMAND_H
