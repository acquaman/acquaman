#include "AMGCS2GetAvailableParametersCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetAvailableParametersCommand::AMGCS2GetAvailableParametersCommand()
{
}

QString AMGCS2GetAvailableParametersCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	} else {
		return QString("NOTE: The following information applies to the raw controller command set, not necessarily these drivers. For programmer referenece only.\n%1")
				.arg(parameterResults_);
	}
}

bool AMGCS2GetAvailableParametersCommand::runImplementation()
{
	AMCArrayHandler<char> availableParameterResults(BUFFER_SIZE);

	bool success = PI_qHPA(controllerId_,
						   availableParameterResults.cArray(),
						   BUFFER_SIZE);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		parameterResults_ = availableParameterResults.cArray();
	}

	return success;

}
