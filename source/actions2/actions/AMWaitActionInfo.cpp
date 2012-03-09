#include "AMWaitActionInfo.h"

AMWaitActionInfo::AMWaitActionInfo(double seconds, QObject *parent) :
	AMActionInfo(QString("Waiting for %1 seconds").arg(seconds),
				 QString("This action waits for a defined period of time (%1 seconds) before completing").arg(seconds),
				 ":/user-away.png",
				 parent)
{
	seconds_ = seconds;
	setExpectedDuration(seconds_);
}

void AMWaitActionInfo::setSecondsToWait(double seconds)
{
	if(seconds == seconds_) return;

	seconds_ = seconds;
	setExpectedDuration(seconds);
	setShortDescription(QString("Waiting for %1 seconds").arg(seconds));
	setLongDescription(QString("This action waits for a defined period of time (%1 seconds) before completing").arg(seconds));
	setModified(true);
}
