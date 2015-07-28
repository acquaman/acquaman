#ifndef AMGCS2GETSERVOMODECOMMAND_H
#define AMGCS2GETSERVOMODECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
/*!
  * A command representing a query for a PI C887.11 controller to report whether
  * it is in servo mode or not.
  */
class AMGCS2GetServoModeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get servo mode command which will query the servo
	  * mode status of the controller.
	  */
	AMGCS2GetServoModeCommand();

	/*!
	  * Virtual destructor for the get servo mode command
	  */
	virtual ~AMGCS2GetServoModeCommand() {}

	/*!
	  * The servo mode state reported by the controller.
	  */
	bool servoModeState() const;

protected:

	/*!
	  * Defines the steps taken to query the servo mode status of axes from a controller.
	  */
	virtual bool runImplementation();

	bool servoModeState_;
};

#endif // AMGCS2GETSERVOMODECOMMAND_H
