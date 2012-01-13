#include "VESPERSRoperCCDDetectorInfo.h"

VESPERSRoperCCDDetectorInfo::VESPERSRoperCCDDetectorInfo(const QString &name, const QString &description, QObject *parent)
	: AMDetectorInfo(name, description, parent)
{
	acquireTime_ = 0;
	temperature_ = 0;
	QList<AMAxisInfo> axes_;
	axes_ << AMAxisInfo("Width", 2084, "Pixels along x-direction");
	axes_ << AMAxisInfo("Height", 2084, "Pixels along the y-direction");
}

VESPERSRoperCCDDetectorInfo::VESPERSRoperCCDDetectorInfo(const VESPERSRoperCCDDetectorInfo &original)
	: AMDetectorInfo(original)
{
	retreiveAndSetProperties(original);
}

VESPERSRoperCCDDetectorInfo &VESPERSRoperCCDDetectorInfo::operator =(const VESPERSRoperCCDDetectorInfo &other)
{
	if (this != &other)
		retreiveAndSetProperties(other);

	return *this;
}
