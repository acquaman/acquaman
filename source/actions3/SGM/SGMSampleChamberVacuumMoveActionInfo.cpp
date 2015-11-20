#include "SGMSampleChamberVacuumMoveActionInfo.h"

SGMSampleChamberVacuumMoveActionInfo::SGMSampleChamberVacuumMoveActionInfo(const AMNumber &setpoint, QObject *parent) :
	AMActionInfo3(QString(), QString(), QString(), parent)
{
	setpoint_ = setpoint;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

SGMSampleChamberVacuumMoveActionInfo::SGMSampleChamberVacuumMoveActionInfo(const SGMSampleChamberVacuumMoveActionInfo &original) :
	AMActionInfo3(other)
{
	setpoint_ = original.setpoint();
}

SGMSampleChamberVacuumMoveActionInfo::~SGMSampleChamberVacuumMoveActionInfo()
{

}

AMActionInfo3* SGMSampleChamberVacuumMoveActionInfo::createCopy() const
{
	AMActionInfo3 *info = new SGMSampleChamberVacuumMoveActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
