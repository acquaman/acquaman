#include "AMGCS2SetServoModeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2SetServoModeCommand::AMGCS2SetServoModeCommand(bool activeStatus)
{
	activeStatus_ = activeStatus;
}

bool AMGCS2SetServoModeCommand::runImplementation()
{
	/*
	  Althought the controller and library docs specify that you need to provide
	  the axis and active status to alter servo mode, when it's actually called
	  (either through the library or on the controller itself) changing the mode
	  for one axis changes them for all. In order to remove confusion the interface
	  for this command is designed to reflect the actual semantics of the command.
	  */
	AMCArrayHandler<int> servoModesHandler(AXIS_COUNT);
	QString axesString("X Y Z U V W");

	int activeStatusValue = 0;

	if(activeStatus_) {
		activeStatusValue = 1;
	} else {
		activeStatusValue = 0;
	}

	servoModesHandler.cArray()[0] = activeStatusValue;
	servoModesHandler.cArray()[1] = activeStatusValue;
	servoModesHandler.cArray()[2] = activeStatusValue;
	servoModesHandler.cArray()[3] = activeStatusValue;
	servoModesHandler.cArray()[4] = activeStatusValue;
	servoModesHandler.cArray()[5] = activeStatusValue;

	bool success = PI_SVO(controller_->id(),
						  axesString.toStdString().c_str(),
						  servoModesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
