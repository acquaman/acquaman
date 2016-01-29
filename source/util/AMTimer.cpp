#include "AMTimer.h"

AMTimer::AMTimer(QObject *parent)
	:QObject(parent)
{
	timerInterval_ = 0;
	isPaused_ = false;

	connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
}

int AMTimer::interval() const
{
	return timerInterval_;
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
	return ( !timer_.isActive() && isPaused_ );
}

int AMTimer::timeSinceStarted() const
{
	if (isActive()) {
		return startTime_.msecsTo(QTime::currentTime());
	} else {
		return 0;
	}
}

int AMTimer::timeSinceTimeout() const
{
	if(isActive()) {
		return intervalStartTime_.msecsTo(QTime::currentTime());
	} else {
		return 0;
	}
}

int AMTimer::timeRunning() const
{
	if (isActive()) {
		return timeSinceStarted() - timePaused();
	} else {
		return 0;
	}
}

int AMTimer::timePaused() const
{
	if (isActive()) {
		int totalTimePaused = timeWhilePaused_;
		if (isPaused())
			totalTimePaused += timeLastPaused_.msecsTo(QTime::currentTime());
		return totalTimePaused;
	} else {
		return 0;
	}
}

void AMTimer::setInterval(int msec)
{
	if (msec > 0) {
		timerInterval_ = msec;
		timer_.setInterval(msec);
	}
}

void AMTimer::setSingleShot(bool singleShot)
{
	timer_.setSingleShot(singleShot);
}

void AMTimer::start(int msec)
{
	setInterval(msec);

	isPaused_ = false;
	timeLastPaused_ = QTime();
	timeWhilePaused_ = 0;

	startTime_ = QTime::currentTime();
	intervalStartTime_ = startTime_;
	timeLastResumed_ = startTime_;

	intervalRemaining_ = timerInterval_;
	timer_.start(timerInterval_);
}

void AMTimer::start()
{
	start(timerInterval_);
}

void AMTimer::stop()
{
	timer_.stop();

	isPaused_ = false;
}

void AMTimer::pause()
{
	if(!timer_.isActive() || isPaused_) {
		return;
	}

	isPaused_ = true;

	// We've just moved to paused
	timeLastPaused_ = QTime::currentTime();
	timer_.stop();

	// the remaining running time for this interval: the original remaining intervalTime - the last running time (time from start/last resume)
	intervalRemaining_ -= timeLastResumed_.msecsTo(QTime::currentTime());
}

void AMTimer::resume()
{
	if(timer_.isActive() || !isPaused_) {
		return;
	}

	isPaused_ = false;
	// We've just moved to unpaused
	timeLastResumed_ = QTime::currentTime();
	timeWhilePaused_ += (timeLastPaused_.msecsTo(QTime::currentTime()));

	resetTimerIntervalNextTimeout_ = true;
	timer_.start(intervalRemaining_);
}

void AMTimer::onTimerTimeout()
{
	if (resetTimerIntervalNextTimeout_) {
		timer_.setInterval(timerInterval_);
		resetTimerIntervalNextTimeout_ = false;
	}

	intervalStartTime_ = QTime::currentTime();
	timeLastResumed_ = intervalStartTime_;

	intervalRemaining_ = timerInterval_;
	emit timeout();
}
