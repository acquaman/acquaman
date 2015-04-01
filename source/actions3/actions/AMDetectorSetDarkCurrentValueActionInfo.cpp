#include "AMDetectorSetDarkCurrentValueActionInfo.h"

AMDetectorSetDarkCurrentValueActionInfo::AMDetectorSetDarkCurrentValueActionInfo(double newValue, const AMDetectorInfo &detectorInfo, QObject *parent) :
    AMActionInfo3(QString(), QString(), QString(), parent)
{
	darkCurrentValue_ = newValue;
	detectorInfo_.setValuesFrom(detectorInfo);

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

AMDetectorSetDarkCurrentValueActionInfo::AMDetectorSetDarkCurrentValueActionInfo(const AMDetectorSetDarkCurrentValueActionInfo &other):
	AMActionInfo3(other)
{
	darkCurrentValue_ = other.darkCurrentValue();
	detectorInfo_.setValuesFrom( *(other.detectorInfo()) );
}

AMDetectorSetDarkCurrentValueActionInfo::~AMDetectorSetDarkCurrentValueActionInfo()
{

}

AMActionInfo3* AMDetectorSetDarkCurrentValueActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorSetDarkCurrentValueActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}


