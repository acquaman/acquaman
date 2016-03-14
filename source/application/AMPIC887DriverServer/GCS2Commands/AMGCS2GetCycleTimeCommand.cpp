#include "AMGCS2GetCycleTimeCommand.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetCycleTimeCommand::AMGCS2GetCycleTimeCommand()
{
	cycleTime_ = 0;
}

double AMGCS2GetCycleTimeCommand::cycleTime() const
{
	return cycleTime_;
}

bool AMGCS2GetCycleTimeCommand::runImplementation()
{
	// clear last value:
	cycleTime_ = 0;

	bool success = PI_qSCT(controller_->id(), &cycleTime_);

	if(!success) {
		lastError_ = controllerErrorMessage();
		cycleTime_ = 0;
	}

	return success;
}
