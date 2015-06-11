#include "AMGCS2CommandLevelQueryCommand.h"
#include "PI_GCS2_DLL.h"
#include "AMGCS2Support.h"
AMGCS2CommandLevelQueryCommand::AMGCS2CommandLevelQueryCommand()
{
	commandLevel_ = AMGCS2::UnknownCommandLevel;
}

QString AMGCS2CommandLevelQueryCommand::outputString() const
{
	return AMGCS2Support::commandLevelToString(commandLevel_);
}

AMGCS2::ControllerCommandLevel AMGCS2CommandLevelQueryCommand::commandLevel()
{
	return commandLevel_;
}

bool AMGCS2CommandLevelQueryCommand::runImplementation()
{
	int returnedCommandLevel = -1;

	bool success = PI_qCCL(controllerId_, &returnedCommandLevel);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {
		if(returnedCommandLevel == 0) {
			commandLevel_ = AMGCS2::NormalCommandLevel;
		} else {
			commandLevel_ = AMGCS2::AdvancedCommandLevel;
		}
	}

	return success;
}
