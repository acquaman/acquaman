#include "AMGCS2GetDeviceIdentificationCommand.h"

#include "PI_GCS2_DLL.h"
#include "util/AMCArrayHandler.h"
AMGCS2GetDeviceIdentificationCommand::AMGCS2GetDeviceIdentificationCommand()
{
}

QString AMGCS2GetDeviceIdentificationCommand::outputString() const
{
	if(wasSuccessful_)	{
		return deviceIdenfiticationString_;
	} else {
		return "";
	}
}

bool AMGCS2GetDeviceIdentificationCommand::runImplementation()
{
	// Clear any previous results
	deviceIdenfiticationString_ = "";

	AMCArrayHandler<char> responseString(BUFFER_SIZE);

	bool success = PI_qIDN(controllerId_, responseString.cArray(), BUFFER_SIZE);

	if(success) {
		deviceIdenfiticationString_ = QString(responseString.cArray());
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
