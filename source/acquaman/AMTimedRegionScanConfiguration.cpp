#include "AMTimedRegionScanConfiguration.h"

#include "util/AMErrorMonitor.h"

AMTimedRegionScanConfiguration::AMTimedRegionScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	userScanName_ = "$sample - $technique";
	time_ = 0.0;
	totalTime_ = 0.0;
}

AMTimedRegionScanConfiguration::AMTimedRegionScanConfiguration(const AMTimedRegionScanConfiguration &original)
	: AMScanConfiguration()
{
	userScanName_ = original.userScanName();
	time_ = original.time();
	totalTime_ = original.totalTime();
}

const QMetaObject *AMTimedRegionScanConfiguration::getMetaObject()
{
	return metaObject();
}

double AMTimedRegionScanConfiguration::time() const
{
	return time_;
}

double AMTimedRegionScanConfiguration::totalTime() const
{
	return totalTime_;
}

void AMTimedRegionScanConfiguration::setTime(double newTime)
{
	if (time_ != newTime){

		time_ = newTime;
		emit timeChanged(time_);
		setModified(true);
	}
}

void AMTimedRegionScanConfiguration::setTotalTime(double newTotalTime)
{
	if (totalTime_ != newTotalTime){

		totalTime_ = newTotalTime;
		emit timeChanged(totalTime_);
		setModified(true);
	}
}
