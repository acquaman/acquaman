#include "AMDetectorSetAsDarkCurrentTimeActionInfo.h"

AMDetectorSetAsDarkCurrentTimeActionInfo::AMDetectorSetAsDarkCurrentTimeActionInfo(const AMDetectorInfo &detectorInfo, double secondsDwell, QObject *parent) :
    AMActionInfo3("Detector set as dark current time", "Detector set as dark current time", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	darkCurrentTime_ = secondsDwell;
}

AMDetectorSetAsDarkCurrentTimeActionInfo::AMDetectorSetAsDarkCurrentTimeActionInfo(const AMDetectorSetAsDarkCurrentTimeActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom( *(other.detectorInfo()) );
	darkCurrentTime_ = other.darkCurrentTime_;
}

AMDetectorSetAsDarkCurrentTimeActionInfo::~AMDetectorSetAsDarkCurrentTimeActionInfo()
{

}

AMActionInfo3* AMDetectorSetAsDarkCurrentTimeActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorSetAsDarkCurrentTimeActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
