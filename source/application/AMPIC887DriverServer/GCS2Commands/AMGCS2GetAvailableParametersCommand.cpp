#include "AMGCS2GetAvailableParametersCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetAvailableParametersCommand::AMGCS2GetAvailableParametersCommand()
{
}

QString AMGCS2GetAvailableParametersCommand::availableParameters() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	return parameterResults_;
}

bool AMGCS2GetAvailableParametersCommand::runImplementation()
{
	AMCArrayHandler<char> availableParameterResults(BUFFER_SIZE);

	bool success = PI_qHPA(controller_->id(),
						   availableParameterResults.cArray(),
						   BUFFER_SIZE);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		parameterResults_ = availableParameterResults.cArray();
	}

	return success;

}
