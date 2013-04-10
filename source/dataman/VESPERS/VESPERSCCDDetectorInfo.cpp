#include "VESPERSCCDDetectorInfo.h"

VESPERSCCDDetectorInfo::VESPERSCCDDetectorInfo(const QString &name, const QString &description, const AMnDIndex &size, QObject *parent)
	: AMOldDetectorInfo(name, description, parent)
{
	acquireTime_ = -1;
	size_ = size;
	axes_ << AMAxisInfo("Width", size_.i(), "Pixels along x-direction");
	axes_ << AMAxisInfo("Height", size_.j(), "Pixels along the y-direction");
}

VESPERSCCDDetectorInfo::VESPERSCCDDetectorInfo(const VESPERSCCDDetectorInfo &original)
	: AMOldDetectorInfo(original)
{
	this->operator =(original);
}

VESPERSCCDDetectorInfo &VESPERSCCDDetectorInfo::operator =(const VESPERSCCDDetectorInfo &other)
{
	if (this != &other){

		AMOldDetectorInfo::operator =(other);
		axes_ = other.axes();
		setSize(other.size());
		setAcquireTime(other.acquireTime());
	}

	return *this;
}

bool VESPERSCCDDetectorInfo::setSize(const AMnDIndex &size)
{
	size_ = size;
	axes_[0].size = size_.i();
	axes_[1].size = size_.j();

	return true;
}
