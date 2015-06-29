#include "AMGCS2GetCycleTimeCommand.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetCycleTimeCommand::AMGCS2GetCycleTimeCommand()
{
	cycleTime_ = 0;
}

QString AMGCS2GetCycleTimeCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	return QString("Cycle Time: %1").arg(cycleTime_);
}

double AMGCS2GetCycleTimeCommand::cycleTime() const
{
	return cycleTime_;
}

bool AMGCS2GetCycleTimeCommand::runImplementation()
{
	// clear last value:
	cycleTime_ = 0;

	bool success = PI_qSCT(controllerId_, &cycleTime_);

	if(!success) {
		lastError_ = controllerErrorMessage();
		cycleTime_ = 0;
	}

	return success;
}
