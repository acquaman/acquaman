#include "AMDetectorSetDarkCurrentValidStateActionInfo.h"

AMDetectorSetDarkCurrentValidStateActionInfo::AMDetectorSetDarkCurrentValidStateActionInfo(bool newState, const AMDetectorInfo &detectorInfo, QObject *parent) :
    AMActionInfo3(QString(), QString(), QString(), parent)
{
	darkCurrentState_ = newState;
	detectorInfo_.setValuesFrom(detectorInfo);

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

AMDetectorSetDarkCurrentValidStateActionInfo::AMDetectorSetDarkCurrentValidStateActionInfo(const AMDetectorSetDarkCurrentValidStateActionInfo &other) :
	AMActionInfo3(other)
{
	darkCurrentState_ = other.darkCurrentState();
	detectorInfo_.setValuesFrom( *(other.detectorInfo()) );
}

AMDetectorSetDarkCurrentValidStateActionInfo::~AMDetectorSetDarkCurrentValidStateActionInfo()
{

}

AMActionInfo3* AMDetectorSetDarkCurrentValidStateActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorSetDarkCurrentValidStateActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorSetDarkCurrentValidStateActionInfo::setDarkCurrentState(bool darkCurrentState)
{
	 darkCurrentState_ = darkCurrentState;
	 setModified(true);
}
