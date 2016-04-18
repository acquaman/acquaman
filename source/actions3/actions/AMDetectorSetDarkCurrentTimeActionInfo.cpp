#include "AMDetectorSetDarkCurrentTimeActionInfo.h"

AMDetectorSetDarkCurrentTimeActionInfo::AMDetectorSetDarkCurrentTimeActionInfo(double secondsDwell, const AMDetectorInfo &detectorInfo, QObject *parent) :
	AMActionInfo3(QString(), QString(), QString(), parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	darkCurrentTime_ = secondsDwell;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

AMDetectorSetDarkCurrentTimeActionInfo::AMDetectorSetDarkCurrentTimeActionInfo(const AMDetectorSetDarkCurrentTimeActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom( *(other.detectorInfo()) );
	darkCurrentTime_ = other.darkCurrentTime();
}

AMDetectorSetDarkCurrentTimeActionInfo::~AMDetectorSetDarkCurrentTimeActionInfo()
{

}

AMActionInfo3* AMDetectorSetDarkCurrentTimeActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorSetDarkCurrentTimeActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorSetDarkCurrentTimeActionInfo::setDarkCurrentTime(double darkCurrentTime)
{
	 darkCurrentTime_ = darkCurrentTime;
	 setModified(true);
}
