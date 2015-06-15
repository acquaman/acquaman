#include "AMGCS2GetControllerReadyStatusCommand.h"

#include "PI_GCS2_DLL.h"
AMGCS2GetControllerReadyStatusCommand::AMGCS2GetControllerReadyStatusCommand()
{
	isReady_ = false;
}

bool AMGCS2GetControllerReadyStatusCommand::isReady() const
{
	return isReady_;
}

QString AMGCS2GetControllerReadyStatusCommand::outputString() const
{
	if(isReady_) {
		return "Controller is ready.";
	} else {
		return "Controller NOT ready.";
	}
}

bool AMGCS2GetControllerReadyStatusCommand::runImplementation()
{
	// Clear previous results (assume false if not successful)
	isReady_ = false;

	int readyStatusReturn;
	bool success = PI_IsControllerReady(controllerId_, &readyStatusReturn);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {
		isReady_ = (readyStatusReturn == 1);
	}

	return success;
}




