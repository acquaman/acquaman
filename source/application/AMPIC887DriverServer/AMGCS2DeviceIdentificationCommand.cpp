#include "AMGCS2DeviceIdentificationCommand.h"

#include "PI_GCS2_DLL.h"
#include "util/AMCArrayHandler.h"
AMGCS2DeviceIdentificationCommand::AMGCS2DeviceIdentificationCommand()
{
}

QString AMGCS2DeviceIdentificationCommand::outputString() const
{
	if(wasSuccessful())	{
		return deviceIdenfiticationString_;
	} else {
		return "";
	}
}

bool AMGCS2DeviceIdentificationCommand::runImplementation()
{
	AMCArrayHandler<char> responseString(BUFFER_SIZE);

	bool success = PI_qIDN(controllerId_, responseString.cArray(), BUFFER_SIZE);

	if(success) {
		deviceIdenfiticationString_ = QString(responseString.cArray());
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
