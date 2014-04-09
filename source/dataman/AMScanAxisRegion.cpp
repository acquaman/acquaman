#include "AMScanAxisRegion.h"

AMScanAxisRegion::~AMScanAxisRegion(){}

AMScanAxisRegion::AMScanAxisRegion(const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, QObject *parent)
	: AMDbObject(parent)
{
	regionStart_ = start;
	regionStep_ = step;
	regionEnd_ = end;
	regionTime_ = time;
}

AMScanAxisRegion::AMScanAxisRegion(const AMScanAxisRegion &original)
	: AMDbObject(original)
{
	regionStart_ = original.regionStart();
	regionStep_ = original.regionStep();
	regionEnd_ = original.regionEnd();
	regionTime_ = original.regionTime();
}

AMNumber AMScanAxisRegion::regionStart() const
{
	return regionStart_;
}

AMNumber AMScanAxisRegion::regionStep() const
{
	return regionStep_;
}

AMNumber AMScanAxisRegion::regionEnd() const
{
	return regionEnd_;
}

AMNumber AMScanAxisRegion::regionTime() const
{
	return regionTime_;
}

void AMScanAxisRegion::setRegionStart(const AMNumber &regionStart)
{
	emit regionStartChanged(regionStart_ = regionStart);
	setModified(true);
}

void AMScanAxisRegion::setRegionStep(const AMNumber &regionStep)
{
	emit regionStepChanged(regionStep_ = regionStep);
	setModified(true);
}

void AMScanAxisRegion::setRegionEnd(const AMNumber &regionEnd)
{
	emit regionEndChanged(regionEnd_ = regionEnd);
	setModified(true);
}

void AMScanAxisRegion::setRegionTime(const AMNumber &regionTime)
{
	emit regionTimeChanged(regionTime_ = regionTime);
	setModified(true);
}
