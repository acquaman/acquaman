#include "AMGCS2StopCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2StopCommand::AMGCS2StopCommand()
{
	statusString_ = "Controller stop not issued";
}

bool AMGCS2StopCommand::runImplementation()
{
	// Reset status string
	statusString_ = "Controller stop not issued";

	bool success = PI_GcsCommandset(controller_->id(), "STP");


	if(success) {

		/* We have to obtain the controller error message, as calling STP always sets
		  the controller's error code to 10, and you can't issue a command if the controller's
		  error message contains a value. Retrieving the error message clears it.
		  Unfortunately there seems to be a bug in the library/controller where the
		  system hangs if you attept to immediately obtain the error message while the
		  axes are still being stopped. As such we need to schedule the clearing of
		  the error message for some period of time after we're done.*/

		statusString_ = "Controller stopped.";
		controller_->clearErrorMessage();

	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}


