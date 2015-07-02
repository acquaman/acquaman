#include "AMGCS2AsyncCommand.h"
#include <QTimerEvent>
AMGCS2AsyncCommand::AMGCS2AsyncCommand(double timeoutMs) :
	QObject(0), AMGCS2Command()
{
	timeoutMs_ = timeoutMs;
	runningState_ = NotStarted;
}

void AMGCS2AsyncCommand::run()
{
	startTime_ = QTime::currentTime();
	timerId_ = startTimer(TIMER_CHECK_INTERVAL_MS);

	if(controllerId_ < 0) {
		lastError_ = QString("Could not run command: Connection not yet established with controller");
		runningState_ = Failed;
		emit failed(this);
	} else if(validateArguments()) {
		if(runImplementation()) {
			runningState_ = Running;
			emit started(this);
		}

	} else {
		lastError_ = QString("Could not run command: Validation of arguments failed with message - '%1'").arg(lastError_);
		runningState_ = Failed;
		emit failed(this);
	}

}

void AMGCS2AsyncCommand::timerEvent(QTimerEvent *)
{
	if(startTime_.msecsTo(QTime::currentTime()) >= timeoutMs_) {
		lastError_ = "Async command timer timed out";
		killTimer(timerId_);
		emit failed(this);
	} else {
		isFinishedImplementation();
		switch (runningState_) {
		case NotStarted:
			// This should never happen
			lastError_ = "Attempt to check running state of not started command";
			emit failed(this);
			killTimer(timerId_);
			break;
		case Running:
			// Do Nothing
			break;
		case Succeeded:
			emit succeeded(this);
			wasSuccessful_ = true;
			killTimer(timerId_);
			break;
		case Failed:
			emit failed(this);
			killTimer(timerId_);
			break;
		}
	}
}



