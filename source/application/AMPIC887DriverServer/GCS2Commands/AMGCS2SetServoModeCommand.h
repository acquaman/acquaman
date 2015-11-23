#ifndef AMGCS2SETSERVOMODECOMMAND_H
#define AMGCS2SETSERVOMODECOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include <QList>
#include <QHash>
/*!
  * A command representing an instruction to a PI C887.11 controller to set the
  * servo mode to the provided value.
  */
class AMGCS2SetServoModeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a set servo mode command which will set the servo
	  * mode of all axes of the controller based on the provided value.
	  * \param activeStatus ~ True if the command should set servo mode to active,
	  * false if the command should set the servo mode to inactive.
	  */
	AMGCS2SetServoModeCommand(bool activeStatus);

	/*!
	  * Virtual destructor for a set servo mode command.
	  */
	virtual ~AMGCS2SetServoModeCommand() {}

protected:

	/*!
	  * Defines the steps taken to set the servo mode of a controller.
	  */
	bool runImplementation();

	bool activeStatus_;
};

#endif // AMGCS2SETSERVOMODECOMMAND_H
