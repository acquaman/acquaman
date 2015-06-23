#include "AMGCS2SetCycleTimeCommand.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetCycleTimeCommand::AMGCS2SetCycleTimeCommand(double cycleTime)
{
	cycleTime_ = cycleTime;
}

bool AMGCS2SetCycleTimeCommand::runImplementation()
{
	bool success = PI_SCT(controllerId_, cycleTime_);

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
