#include "AMGCS2GetSystemVelocityCommand.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetSystemVelocityCommand::AMGCS2GetSystemVelocityCommand()
{
	systemVelocity_ = 0;
}

QString AMGCS2GetSystemVelocityCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	return QString("System Velocity: %1mm/s").arg(systemVelocity_);
}

double AMGCS2GetSystemVelocityCommand::systemVelocity() const
{
	return systemVelocity_;
}

bool AMGCS2GetSystemVelocityCommand::runImplementation()
{
	systemVelocity_ = 0;

	bool success = PI_qVLS(controllerId_, &systemVelocity_);

	if(!success) {
		lastError_ = controllerErrorMessage();
		systemVelocity_ = 0;
	}

	return success;
}
