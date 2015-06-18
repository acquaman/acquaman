#include "AMGCS2GetRecorderOptionsCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetRecorderOptionsCommand::AMGCS2GetRecorderOptionsCommand()
{
}

QString AMGCS2GetRecorderOptionsCommand::outputString() const
{
	if(wasSuccessful_) {
		return QString("NOTE: The following information applys to the raw controller command set, not necessarily these drivers. For programmer referenece only.\n%1")
				.arg(optionResults_);
	} else {
		return "";
	}
}

bool AMGCS2GetRecorderOptionsCommand::runImplementation()
{
	// Clear previous results
	optionResults_.clear();

	AMCArrayHandler<char> results(BUFFER_SIZE);

	bool success = PI_qHDR(controllerId_, results.cArray(), BUFFER_SIZE);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {
		optionResults_ = results.cArray();
	}

	return success;
}
