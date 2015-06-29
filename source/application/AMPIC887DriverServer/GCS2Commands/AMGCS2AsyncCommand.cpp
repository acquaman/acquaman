#include "AMGCS2AsyncCommand.h"
#include <QTimerEvent>
AMGCS2AsyncCommand::AMGCS2AsyncCommand(double timeoutMs) :
	QObject(0)
{
	timeoutMs_ = timeoutMs;
}

void AMGCS2AsyncCommand::run()
{
	startTime_ = QTime::currentTime();
	timerId_ = startTimer(TIMER_CHECK_INTERVAL_MS);
	AMGCS2Command::run();
}

void AMGCS2AsyncCommand::timerEvent(QTimerEvent *event)
{

	if(startTime_.msecsTo(QTime::currentTime()) >= timeoutMs_) {
		lastError_ = "Async command timer timed out";

	} else {
		RunningState returnedRunningState = isFinishedImplementation();

		switch(returnedRunningState) {
		case Completed:
			killTimer(timerId_);
			emit completed();
			break;
		case Failed:
			killTimer(timerId_);
			emit failed();
			break;
		default:
			// Do nothing.
			break;
		}
	}
}


