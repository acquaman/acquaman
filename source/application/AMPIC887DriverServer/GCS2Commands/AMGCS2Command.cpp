#include "AMGCS2Command.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"

AMGCS2Command::AMGCS2Command()
{
	controllerId_ = -1;
	wasSuccessful_ = false;
}

bool AMGCS2Command::wasSuccessful() const
{
	return wasSuccessful_;
}

QString AMGCS2Command::lastError() const
{
	if(!wasSuccessful_) {
		return lastError_;
	} else {
		return "No Error";
	}
}

void AMGCS2Command::run()
{
	if(controllerId_ < 0) {
		lastError_ = QString("Could not run command: Connection not yet established with controller");
		wasSuccessful_ = false;
	} else if(validateArguments()) {
		wasSuccessful_ = runImplementation();

	} else {
		lastError_ = QString("Could not run command: Validation of arguments failed with message - '%1'").arg(lastError_);
		wasSuccessful_ = false;
	}
}

QString AMGCS2Command::controllerErrorMessage()
{
	int errorCode = PI_GetError(controllerId_);

	int iBufferSize = BUFFER_SIZE;
	AMCArrayHandler<char> cStringHandler(iBufferSize);
	bool success = PI_TranslateError(errorCode, cStringHandler.cArray(), iBufferSize);

	if(!success) {
		return QString("Could not translate returned error code %1").arg(errorCode);
	} else {
		return QString(cStringHandler.cArray());
	}
}

void AMGCS2Command::setControllerId(int id)
{
	controllerId_ = id;
}


QString AMGCS2Command::outputString() const
{
	return QString();
}
