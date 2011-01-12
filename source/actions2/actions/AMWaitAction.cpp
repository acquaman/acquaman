#include "AMWaitAction.h"

AMWaitAction::AMWaitAction(AMWaitActionInfo* info, QObject *parent) :
	AMAction(info, parent)
{
}

AMWaitAction::AMWaitAction(double seconds, QObject* parent) :
	AMAction(new AMWaitActionInfo(seconds), parent)
{
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
	progressTick_.start(200);

	notifyStarted();
}

void AMWaitAction::pauseImplementation()
{
	timer_.stop();
	progressTick_.stop();

	notifyPaused();
}

void AMWaitAction::resumeImplementation()
{
	double secondsCompleted = runningTime();
	timer_.start(int((waitInfo()->secondsToWait() - secondsCompleted)*1000));
	progressTick_.start();

	notifyResumed();
}

void AMWaitAction::cancelImplementation()
{
	timer_.stop();
	progressTick_.stop();
	notifyCancelled();
}

void AMWaitAction::onProgressTick()
{
	double secondsCompleted = runningTime();
	setProgress(secondsCompleted, waitInfo()->secondsToWait());
}

void AMWaitAction::onFinished()
{
	timer_.stop();
	notifySucceeded();
}
