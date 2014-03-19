#include "AMScanAxisRegion.h"

AMScanAxisRegion::~AMScanAxisRegion(){}

AMScanAxisRegion::AMScanAxisRegion(AMNumber start, AMNumber step, AMNumber end, AMNumber time, QObject *parent) :
	AMDbObject(parent)
{
	setRegionStart(start);
	setRegionStep(step);
	setRegionEnd(end);
	setRegionTime(time);
}

AMScanAxisRegion::AMScanAxisRegion(const AMScanAxisRegion &original)
	: AMDbObject(original)
{
	setRegionStart(original.regionStart());
	setRegionStep(original.regionStep());
	setRegionEnd(original.regionEnd());
	setRegionTime(original.regionTime());
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

void AMScanAxisRegion::setRegionStart(AMNumber regionStart)
{
	regionStart_ = regionStart;
}

void AMScanAxisRegion::setRegionStep(AMNumber regionStep)
{
	regionStep_ = regionStep;
}

void AMScanAxisRegion::setRegionEnd(AMNumber regionEnd)
{
	regionEnd_ = regionEnd;
}

void AMScanAxisRegion::setRegionTime(AMNumber regionTime)
{
	regionTime_ = regionTime;
}
