#include "CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h"
#include "beamline/CLS/CLSBeamline.h"

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(double dwellTime, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	dwellTime_ = dwellTime;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo &other) :
	AMListActionInfo3(other)
{
	dwellTime_ = other.dwellTime();
}

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::~CLSSIS3820ScalerDarkCurrentMeasurementActionInfo()
{

}

AMActionInfo3* CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::createCopy() const
{
	AMActionInfo3 *info = new CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::setDwellTime(double dwellTime)
{
	dwellTime_ = dwellTime;
	setModified(true);
}
