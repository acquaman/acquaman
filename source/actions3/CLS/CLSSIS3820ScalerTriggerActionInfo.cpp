#include "CLSSIS3820ScalerTriggerActionInfo.h"

CLSSIS3820ScalerTriggerActionInfo::CLSSIS3820ScalerTriggerActionInfo(int readMode, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	readMode_ = readMode;
}

CLSSIS3820ScalerTriggerActionInfo::CLSSIS3820ScalerTriggerActionInfo(const CLSSIS3820ScalerTriggerActionInfo &original) :
	AMActionInfo3(original)
{
	readMode_ = original.readMode();
}

CLSSIS3820ScalerTriggerActionInfo::~CLSSIS3820ScalerTriggerActionInfo()
{

}

AMActionInfo3* CLSSIS3820ScalerTriggerActionInfo::createCopy() const
{
	AMActionInfo3 *info = new CLSSIS3820ScalerTriggerActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void CLSSIS3820ScalerTriggerActionInfo::setReadMode(int newMode)
{
	readMode_ = newMode;
	setModified(true);
}
