#include "AMGCS2Command.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"

AMGCS2Command::AMGCS2Command()
{
	controllerId_ = -1;
	runningState_ = NotStarted;
	commandType_ = Synchronous;
}

AMGCS2Command::RunningState AMGCS2Command::runningState() const
{
	return runningState_;
}

QString AMGCS2Command::lastError() const
{
	if(runningState_ == Failed) {
		return lastError_;
	} else {
		return "No Error";
	}
}

void AMGCS2Command::run()
{
	if(controllerId_ < 0) {
		lastError_ = QString("Could not run command: Connection not yet established with controller");
		runningState_ = Failed;
	} else if(validateArguments()) {
		if(runImplementation()) {
			runningState_ = Succeeded;
		}

	} else {
		lastError_ = QString("Could not run command: Validation of arguments failed with message - '%1'").arg(lastError_);
		runningState_ = Failed;
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

AMGCS2Command::CommandType AMGCS2Command::commandType() const
{
	return commandType_;
}

QString AMGCS2Command::outputString() const
{
	return QString();
}

