#include "CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h"

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(CLSSIS3820Scaler *scaler, double secondsDwell, QObject *parent) :
	AMListActionInfo3("Make dark current measurement.", "Make dark current measurement.", ":/system-run.png", parent)
{
	scaler_ = scaler;
	secondsDwell_ = secondsDwell;
}

CLSSIS3820ScalerDarkCurrentMeasurementActionInfo::CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo &other) :
	AMListActionInfo3(other)
{
	scaler_ = other.scaler_;
	secondsDwell_ = other.secondsDwell_;
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
