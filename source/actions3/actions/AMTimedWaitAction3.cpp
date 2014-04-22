#include "AMTimedWaitAction3.h"

 AMTimedWaitAction3::~AMTimedWaitAction3(){}
AMTimedWaitAction3::AMTimedWaitAction3(AMTimedWaitActionInfo3 *info, QObject *parent) :
	AMAction3(info, parent)
{
	waitTimer_ = 0; //NULL
}

AMTimedWaitAction3::AMTimedWaitAction3(const AMTimedWaitAction3 &other) :
	AMAction3(other)
{
	waitTimer_ = 0; //NULL
}

void AMTimedWaitAction3::startImplementation(){
	waitTimer_ = new QTimer();
	waitTimer_->setSingleShot(true);
	waitTimer_->setInterval(timedWaitInfo()->waitTime()*1000);
	connect(waitTimer_, SIGNAL(timeout()), this, SLOT(onWaitTimerTimeout()));

	setStarted();
	waitTimer_->start();
}

void AMTimedWaitAction3::cancelImplementation(){
	if(waitTimer_)
		waitTimer_->stop();

	setCancelled();
}

void AMTimedWaitAction3::onWaitTimerTimeout(){
	setProgress(100, 100);
	setSucceeded();
}
