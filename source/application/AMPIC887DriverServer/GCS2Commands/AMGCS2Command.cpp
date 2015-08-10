#include "AMGCS2Command.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2Command::AMGCS2Command()
{
	controller_ = 0;
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
	if(!controller_) {
		lastError_ = "Could not run command: No controller";
		runningState_ = Failed;
	} else if(controller_->id() < 0) {
		lastError_ = "Could not run command: Conneciton not yet established with controller";
		runningState_ = Failed;
	} else if(controller_->isBusy()) {
		lastError_ = "Could not run command: Controller is busy";
		runningState_ = Failed;
	} else if(validateArguments()) {
		if(runImplementation()) {
			runningState_ = Succeeded;
		} else {
			runningState_ = Failed;
		}
	} else {
		lastError_ = QString("Could not run command: Validation of arguments failed (%1))")
				.arg(lastError_);
		runningState_ = Failed;
	}
}

QString AMGCS2Command::controllerErrorMessage()
{
	int errorCode = PI_GetError(controller_->id());

	int iBufferSize = BUFFER_SIZE;
	AMCArrayHandler<char> cStringHandler(iBufferSize);
	bool success = PI_TranslateError(errorCode, cStringHandler.cArray(), iBufferSize);

	if(!success) {
		return QString("Could not translate returned error code %1").arg(errorCode);
	} else {
		return QString(cStringHandler.cArray());
	}
}

void AMGCS2Command::setController(AMPIC887Controller* controller)
{
	controller_ = controller;
}

AMGCS2Command::CommandType AMGCS2Command::commandType() const
{
	return commandType_;
}
