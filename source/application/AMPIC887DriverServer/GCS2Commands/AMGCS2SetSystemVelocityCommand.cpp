#include "AMGCS2SetSystemVelocityCommand.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetSystemVelocityCommand::AMGCS2SetSystemVelocityCommand(double systemVelocity)
{
	systemVelocity_ = systemVelocity;
}

bool AMGCS2SetSystemVelocityCommand::runImplementation()
{
	bool success = PI_VLS(controllerId_, systemVelocity_);

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
