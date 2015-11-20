#include "SGMSampleChamberVacuumMoveActionInfo.h"

SGMSampleChamberVacuumMoveActionInfo::SGMSampleChamberVacuumMoveActionInfo(const AMNumber &setpoint, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	setpoint_ = setpoint;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

SGMSampleChamberVacuumMoveActionInfo::SGMSampleChamberVacuumMoveActionInfo(const SGMSampleChamberVacuumMoveActionInfo &original) :
	AMListActionInfo3(original)
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
