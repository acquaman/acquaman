#include "AMTimer.h"

AMTimer::AMTimer(QObject *parent)
{
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
	timerInterval_ = 0;
	isPaused_ = false;
}

int AMTimer::interval() const
{
	return timer_.interval();
}

bool AMTimer::isActive() const {
	return ( timer_.isActive() || isPaused_ );
}

bool AMTimer::isRunning() const
{
	return timer_.isActive();
}

bool AMTimer::isSingleShot() const
{
	return timer_.isSingleShot();
}

bool AMTimer::isPaused() const
{
	return ( timer_.isActive() && isPaused_ );
}

int AMTimer::timeRunning() const
{

}

int AMTimer::timeSinceTimeout() const
{
	if(isActive()) {
		return intervalStartTime_.msecsTo(QTime::currentTime());
	} else {
		return 0;
	}
}

int AMTimer::timeSinceStarted() const
{
	if(isActive()) {
		return startTime_.msecsTo(QTime::currentTime());
	} else {
		return 0;
	}
}

void AMTimer::setInterval(int msec)
{
	timerInterval_ = msec;
	timer_.setInterval(msec);
}

void AMTimer::setSingleShot(bool singleShot)
{
	timer_.setSingleShot(singleShot);
}

void AMTimer::start(int msec)
{
	startTime_ = QTime::currentTime();
	isPaused_ = false;
	timeLastPaused_ = QTime();
	timeWhilePaused_ = 0;
	timer_.start(msec);
}

void AMTimer::start()
{
	startTime_ = QTime::currentTime();
	isPaused_ = false;
	timeLastPaused_ = QTime();
	timeWhilePaused_ = 0;
	timer_.start();
}

void AMTimer::stop()
{
	timer_.stop();
}

void AMTimer::setPaused(bool pause)
{
	if(!isActive()) {
		return;
	}

	if(isPaused_ == pause) {
		return;
	}

	isPaused_ = pause;
	if(isPaused_) {
		// We've just moved to paused
		timeLastPaused_ = QTime::currentTime();
		intervalRemaining_ -= timeSinceTimeout();
		timer_.stop();
	} else {
		// We've just moved to unpaused
		timer_.setInterval(intervalRemaining_);
		setTimerIntervalNextTimeout_ = true;
		timeWhilePaused_ += (timeLastPaused_.msecsTo(QTime::currentTime()));
	}
}

void AMTimer::onTimerTimeout()
{
	intervalStartTime_ = QTime::currentTime();
	if(setTimerIntervalNextTimeout_) {
		timer_.setInterval(timerInterval_);
		setTimerIntervalNextTimeout_ = false;
	}
	intervalRemaining_ = timer_.interval();
	emit timeout();
}
