#include "AMGCS2SetCycleTimeCommand.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2SetCycleTimeCommand::AMGCS2SetCycleTimeCommand(double cycleTime)
{
	cycleTime_ = cycleTime;
}

bool AMGCS2SetCycleTimeCommand::runImplementation()
{
	bool success = PI_SCT(controller_->id(), cycleTime_);

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
