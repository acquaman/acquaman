#include "AMGCS2GetRecorderOptionsCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetRecorderOptionsCommand::AMGCS2GetRecorderOptionsCommand()
{
}

QString AMGCS2GetRecorderOptionsCommand::recordOptions() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	return optionResults_;
}

bool AMGCS2GetRecorderOptionsCommand::runImplementation()
{
	// Clear previous results
	optionResults_.clear();

	AMCArrayHandler<char> results(BUFFER_SIZE);

	bool success = PI_qHDR(controller_->id(), results.cArray(), BUFFER_SIZE);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {
		optionResults_ = results.cArray();
	}

	return success;
}
