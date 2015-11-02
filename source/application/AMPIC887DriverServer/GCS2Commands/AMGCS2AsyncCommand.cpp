#include "AMGCS2AsyncCommand.h"
#include <QTimerEvent>
#include "../AMPIC887Controller.h"
AMGCS2AsyncCommand::AMGCS2AsyncCommand(qint64 timeoutMs) :
	QObject(0), AMGCS2Command()
{
	timeoutMs_ = timeoutMs;
	commandType_ = Asynchronous;
}

void AMGCS2AsyncCommand::run()
{
	startTime_ = QTime::currentTime();
	timerId_ = startTimer(TIMER_CHECK_INTERVAL_MS);

	// We make a switch to the running state here, even if the command fails to
	// eventually be run on the controller (eg. if the valiation of arguments fails)
	// This ensures that no async command can fail or succeed without being started.
	runningState_ = Running;
	emit started(this);

	if(!controller_) {
		lastError_ = "Could not run command: No controller";
		runningState_ = Failed;
		emit failed(this);
	} else if(controller_->id() < 0) {
		lastError_ = QString("Could not run command: Connection not yet established with controller");
		runningState_ = Failed;
		emit failed(this);
	} else if(controller_->isBusy()) {
		lastError_ = QString("Could not run command: Controller is busy");
		runningState_ = Failed;
		emit failed(this);
	} else if(validateArguments()) {
		if(!runImplementation()) {
			runningState_ = Failed;
			emit failed(this);
		}
	} else {
		lastError_ = QString("Could not run command: Validation of arguments failed with message - '%1'").arg(lastError_);
		runningState_ = Failed;
		emit failed(this);
	}

}

void AMGCS2AsyncCommand::timerEvent(QTimerEvent *)
{
	// Can't perform checks when controller is busy.
	if(controller_->isBusy()) {
		return;
	}

	if(startTime_.msecsTo(QTime::currentTime()) >= timeoutMs_) {
		runningState_ = Failed;
		lastError_ = "Async command timer timed out";
		killTimer(timerId_);
		emit failed(this);
	} else if(runningState_ == Running) {
		checkRunningState();
		switch (runningState_) {
		case NotStarted:
			// This should never happen
			runningState_ = Failed;
			lastError_ = "Attempt to check running state of not started command";
			emit failed(this);
			killTimer(timerId_);
			break;
		case Running:
			// Do Nothing
			break;
		case Succeeded:
			emit succeeded(this);
			killTimer(timerId_);
			break;
		case Failed:
			emit failed(this);
			killTimer(timerId_);
			break;
		}
	}
}



