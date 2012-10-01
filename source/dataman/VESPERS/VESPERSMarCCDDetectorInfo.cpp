#include "VESPERSMarCCDDetectorInfo.h"

VESPERSMarCCDDetectorInfo::VESPERSMarCCDDetectorInfo(const QString &name, const QString &description, QObject *parent)
	: AMDetectorInfo(name, description, parent)
{
	acquireTime_ = 0;
	temperature_ = 0;
	axes_ << AMAxisInfo("Width", 2084, "Pixels along x-direction");
	axes_ << AMAxisInfo("Height", 2084, "Pixels along the y-direction");
}

VESPERSMarCCDDetectorInfo::VESPERSMarCCDDetectorInfo(const VESPERSMarCCDDetectorInfo &original)
	: AMDetectorInfo(original)
{
	this->operator =(original);
}

VESPERSMarCCDDetectorInfo &VESPERSMarCCDDetectorInfo::operator =(const VESPERSMarCCDDetectorInfo &other)
{
	if (this != &other){

		AMDetectorInfo::operator =(other);
		axes_ = other.axes();
		setAcquireTime(other.acquireTime());
		setTemperature(other.temperature());
	}

	return *this;
}
