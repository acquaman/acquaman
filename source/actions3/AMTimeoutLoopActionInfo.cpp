#include "AMTimeoutLoopActionInfo.h"

AMTimeoutLoopActionInfo::AMTimeoutLoopActionInfo(int iterations, double timeout, const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMLoopActionInfo3(iterations, shortDescription, longDescription, iconFileName, parent)
{
	timeoutTime_ = timeout;
}

AMTimeoutLoopActionInfo::~AMTimeoutLoopActionInfo()
{

}

AMActionInfo3 *AMTimeoutLoopActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMTimeoutLoopActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMTimeoutLoopActionInfo::setTimeoutTime(double newTime)
{
	if (newTime != timeoutTime_){

		timeoutTime_ = newTime;
		setModified(true);
		emit timeoutTimeChanged(timeoutTime_);
	}
}
