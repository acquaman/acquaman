#include "CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h"
#include "beamline/CLS/CLSBeamline.h"

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(double secondsDwell, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	secondsDwell_ = secondsDwell;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo &other) :
	AMListActionInfo3(other)
{
	secondsDwell_ = other.dwellTime();
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
