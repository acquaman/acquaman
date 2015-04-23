#include "AMDetectorSetLastMeasurementAsDarkCurrentActionInfo.h"

AMDetectorSetLastMeasurementAsDarkCurrentActionInfo::AMDetectorSetLastMeasurementAsDarkCurrentActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
    AMActionInfo3(QString(), QString(), QString(), parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

AMDetectorSetLastMeasurementAsDarkCurrentActionInfo::AMDetectorSetLastMeasurementAsDarkCurrentActionInfo(const AMDetectorSetLastMeasurementAsDarkCurrentActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom( *(other.detectorInfo()) );
}

AMDetectorSetLastMeasurementAsDarkCurrentActionInfo::~AMDetectorSetLastMeasurementAsDarkCurrentActionInfo()
{

}

AMActionInfo3* AMDetectorSetLastMeasurementAsDarkCurrentActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorSetLastMeasurementAsDarkCurrentActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
