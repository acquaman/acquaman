#include "AMGCS2CommandLevelQueryCommand.h"
#include "PI_GCS2_DLL.h"
AMGCS2CommandLevelQueryCommand::AMGCS2CommandLevelQueryCommand()
{
	commandLevel_ = AMGCS2::UnknownCommandLevel;
}

QString AMGCS2CommandLevelQueryCommand::outputString() const
{
	switch(commandLevel_) {
	case AMGCS2::NormalCommandLevel:
		return "Normal Command Level";
	case AMGCS2::AdvancedCommandLevel:
		return "Advanced Command Level";
	case AMGCS2::UnknownCommandLevel:
		return "Unknown Command Level";
	}

	return "Unknown Command Level";
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
