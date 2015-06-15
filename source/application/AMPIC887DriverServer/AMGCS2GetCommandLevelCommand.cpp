#include "AMGCS2GetCommandLevelCommand.h"
#include "PI_GCS2_DLL.h"
#include "AMGCS2Support.h"
AMGCS2GetCommandLevelCommand::AMGCS2GetCommandLevelCommand()
{
	commandLevel_ = AMGCS2::UnknownCommandLevel;
}

QString AMGCS2GetCommandLevelCommand::outputString() const
{
	return AMGCS2Support::commandLevelToString(commandLevel_);
}

AMGCS2::ControllerCommandLevel AMGCS2GetCommandLevelCommand::commandLevel()
{
	return commandLevel_;
}

bool AMGCS2GetCommandLevelCommand::runImplementation()
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
