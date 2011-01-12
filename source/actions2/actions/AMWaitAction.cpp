#include "AMWaitAction.h"

AMWaitAction::AMWaitAction(AMWaitActionInfo* info, QObject *parent) :
	AMAction(info, parent)
{
	secondsSpentPaused_ = 0;
}

AMWaitAction::AMWaitAction(double seconds, QObject* parent) :
	AMAction(new AMWaitActionInfo(seconds), parent)
{
	secondsSpentPaused_ = 0;
}

const AMWaitActionInfo * AMWaitAction::waitInfo() const
{
	return qobject_cast<const AMWaitActionInfo*>(info());
}

void AMWaitAction::startImplementation()
{
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onFinished()));
	timer_.start(int(waitInfo()->secondsToWait()*1000));

	connect(&progressTick_, SIGNAL(timeout()), this, SLOT(onProgressTick()));
	progressTick_.start(1000);

	notifyStarted();
}

void AMWaitAction::pauseImplementation()
{
	timer_.stop();
	progressTick_.stop();

	pausedAtTime_ = QTime::currentTime();
	notifyPaused();
}

void AMWaitAction::resumeImplementation()
{
	secondsSpentPaused_ += double(pausedAtTime_.msecsTo(QTime::currentTime()))/1000.0;
	setExpectedDuration(waitInfo()->secondsToWait()+secondsSpentPaused_);

	double secondsCompleted = elapsedTime() - secondsSpentPaused_;
	timer_.start(waitInfo()->secondsToWait() - secondsCompleted);
	progressTick_.start();

	notifyResumed();
}

void AMWaitAction::cancelImplementation()
{
	timer_.stop();
	notifyCancelled();
}

void AMWaitAction::onProgressTick()
{
	double secondsCompleted = elapsedTime() - secondsSpentPaused_;
	setProgress(secondsCompleted, waitInfo()->secondsToWait());
}

void AMWaitAction::onFinished()
{
	timer_.stop();
	notifySucceeded();
}
