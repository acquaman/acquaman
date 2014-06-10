#include "AMTimedWaitActionInfo3.h"

 AMTimedWaitActionInfo3::~AMTimedWaitActionInfo3(){}
AMTimedWaitActionInfo3::AMTimedWaitActionInfo3(double waitTime, QObject *parent) :
	AMActionInfo3("Timed Wait", "Timed Wait", ":/system-run.png", parent)
{
	waitTime_ = waitTime;

	updateDescriptionText();
}

AMTimedWaitActionInfo3::AMTimedWaitActionInfo3(const AMTimedWaitActionInfo3 &other) :
	AMActionInfo3(other)
{
	waitTime_ = other.waitTime();
}

void AMTimedWaitActionInfo3::setWaitTime(double waitTime){
	waitTime_ = waitTime;

	updateDescriptionText();
}

void AMTimedWaitActionInfo3::updateDescriptionText()
{
	QString d = QString("Waiting for %1 seconds").arg(waitTime_);

	setShortDescription(d);
	setLongDescription(d);
}
