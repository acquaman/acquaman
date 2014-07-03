#include "AMWaitActionInfo.h"

AMWaitActionInfo::AMWaitActionInfo(double waitTime, QObject *parent)
	: AMActionInfo3("Timed Wait", "Timed Wait", ":/user-away.png", parent)
{
	waitTime_ = waitTime;

	setShortDescription("Timed Wait");
	setLongDescription("Timed Wait");
}

AMWaitActionInfo::AMWaitActionInfo(const AMWaitActionInfo &other)
	: AMActionInfo3(other)
{
	waitTime_ = other.waitTime();
}

AMWaitActionInfo *AMWaitActionInfo::createCopy() const
{
	return new AMWaitActionInfo(*this);
}

void AMWaitActionInfo::setWaitTime(double newTime)
{
	if (newTime != waitTime_){

		waitTime_ = newTime;
		emit waitTimeChanged(waitTime_);
		setShortDescription(QString("Timed Wait of %1 seconds").arg(waitTime_));
		setLongDescription(QString("Timed Wait of %1 seconds").arg(waitTime_));
		setExpectedDuration(waitTime_);
		setModified(true);
	}
}
