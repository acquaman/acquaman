#include "AMLoopActionInfo.h"

AMLoopActionInfo::AMLoopActionInfo(int iterations, QObject *parent) :
	AMActionInfo(QString("Looping %1 times").arg(iterations),
				 QString("This action repeats all of its sub-actions in order, %1 times").arg(iterations),
				 ":/32x32/media-playlist-repeat.png",
				 parent)
{
	loopCount_ = iterations;
}

void AMLoopActionInfo::setLoopCount(int loopCount)
{
	if(loopCount == loopCount_)
		return;

	loopCount_ = loopCount;
	setShortDescription(QString("Looping %1 times").arg(loopCount));
	setLongDescription(QString("This action repeats all of its sub-actions in order, %1 times").arg(loopCount));

	setModified(true);
	emit loopCountChanged(loopCount_);
}
