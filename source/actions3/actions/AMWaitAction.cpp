#include "AMWaitAction.h"

AMWaitAction::AMWaitAction(AMWaitActionInfo *info, QObject *parent)
	: AMAction3(info, parent)
{
	waitTimer_.setSingleShot(true);
	elapsedTime_ = 0.0;
}

AMWaitAction::AMWaitAction(const AMWaitAction &other)
	: AMAction3(other)
{
	waitTimer_.setSingleShot(true);
	elapsedTime_ = 0.0;
}

AMAction3 *AMWaitAction::createCopy() const
{
	return new AMWaitAction(*this);
}

void AMWaitAction::startImplementation()
{
	connect(&waitTimer_, SIGNAL(timeout()), this, SLOT(onWaitTimerTimeout()));
	connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(onProgressTimerTimeout()));

	waitTimer_.start(waitActionInfo()->waitTime()*1000);
	waitTime_.start();
	progressTimer_.start(1000);
	setStarted();
}

void AMWaitAction::pauseImplementation()
{
	waitTimer_.stop();
	progressTimer_.stop();
	elapsedTime_ += waitTime_.elapsed()/1000.0;
	setPaused();
}

void AMWaitAction::resumeImplementation()
{
	waitTimer_.start((waitActionInfo()->waitTime()-elapsedTime_)*1000);
	progressTimer_.start(1000);
	waitTime_.restart();
	setResumed();
}

void AMWaitAction::cancelImplementation()
{
	waitTimer_.stop();
	progressTimer_.stop();
	waitTimer_.disconnect(this);
	progressTimer_.disconnect(this);
	setCancelled();
}

void AMWaitAction::skipImplementation(const QString &command)
{
	Q_UNUSED(command)
	waitTimer_.stop();
	progressTimer_.stop();
	waitTimer_.disconnect(this);
	progressTimer_.disconnect(this);
	setSucceeded();
}

void AMWaitAction::onWaitTimerTimeout()
{
	setSucceeded();
}

void AMWaitAction::onProgressTimerTimeout()
{
	setProgress((elapsedTime_ + waitTime_.elapsed()/1000.0), waitActionInfo()->waitTime());
}
