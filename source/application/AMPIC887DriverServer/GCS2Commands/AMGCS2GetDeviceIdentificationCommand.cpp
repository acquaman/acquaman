#include "AMGCS2GetDeviceIdentificationCommand.h"

#include "PI_GCS2_DLL.h"
#include "util/AMCArrayHandler.h"
#include "../AMPIC887Controller.h"
AMGCS2GetDeviceIdentificationCommand::AMGCS2GetDeviceIdentificationCommand()
{
}

QString AMGCS2GetDeviceIdentificationCommand::deviceIdentification() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	return deviceIdenfiticationString_;
}

bool AMGCS2GetDeviceIdentificationCommand::runImplementation()
{
	// Clear any previous results
	deviceIdenfiticationString_ = "";

	AMCArrayHandler<char> responseString(BUFFER_SIZE);

	bool success = PI_qIDN(controller_->id(), responseString.cArray(), BUFFER_SIZE);

	if(success) {
		deviceIdenfiticationString_ = QString(responseString.cArray());
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}


