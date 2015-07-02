#include "AMGCS2SetCommandLevelCommand.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetCommandLevelCommand::AMGCS2SetCommandLevelCommand(AMGCS2::ControllerCommandLevel commandLevel,
														   const QString& password)
{
	newCommandLevel_ = commandLevel;
	password_ = password;
}

QString AMGCS2SetCommandLevelCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "Command level not altered.";
	} else {
		return AMGCS2Support::commandLevelToString(newCommandLevel_);
	}
}

bool AMGCS2SetCommandLevelCommand::validateArguments()
{
	if(newCommandLevel_ == AMGCS2::AdvancedCommandLevel && password_.isEmpty()) {
		lastError_ = "Cannot set to advanced command level with no password provided";
		return false;
	}

	if(newCommandLevel_ == AMGCS2::UnknownCommandLevel) {
		lastError_ = "Command level not recognized";
		return false;
	}

	return true;
}

bool AMGCS2SetCommandLevelCommand::runImplementation()
{
	bool success = PI_CCL(controllerId_, int(newCommandLevel_), password_.toStdString().c_str());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
