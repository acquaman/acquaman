#include "AMGCS2ControllerReadyStatusCommand.h"

#include "PI_GCS2_DLL.h"
AMGCS2ControllerReadyStatusCommand::AMGCS2ControllerReadyStatusCommand()
{
	isReady_ = false;
}

bool AMGCS2ControllerReadyStatusCommand::isReady() const
{
	return isReady_;
}

QString AMGCS2ControllerReadyStatusCommand::outputString() const
{
	if(isReady_) {
		return "Controller is ready.";
	} else {
		return "Controller NOT ready.";
	}
}

bool AMGCS2ControllerReadyStatusCommand::runImplementation()
{
	int readyStatusReturn;
	bool success = PI_IsControllerReady(controllerId_, &readyStatusReturn);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {
		isReady_ = (readyStatusReturn == 1);
	}

	return success;
}




