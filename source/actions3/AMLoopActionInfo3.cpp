#include "AMLoopActionInfo3.h"

AMLoopActionInfo3::AMLoopActionInfo3(int iterations, QObject *parent) :
	AMListActionInfo3(parent)
{
	setShortDescription(QString("Looping %1 times").arg(iterations));
	setLongDescription(QString("This action repeats all of its sub-actions in order, %1 times").arg(iterations));
	setIconFileName(":/32x32/media-playlist-repeat.png");
	loopCount_ = iterations;
}

void AMLoopActionInfo3::setLoopCount(int loopCount)
{
	if(loopCount == loopCount_)
		return;

	loopCount_ = loopCount;
	setShortDescription(QString("Looping %1 times").arg(loopCount));
	setLongDescription(QString("This action repeats all of its sub-actions in order, %1 times").arg(loopCount));

	setModified(true);
	emit loopCountChanged(loopCount_);
}
