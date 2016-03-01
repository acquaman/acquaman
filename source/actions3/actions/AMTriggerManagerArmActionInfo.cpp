#include "AMTriggerManagerArmActionInfo.h"

AMTriggerManagerArmActionInfo::AMTriggerManagerArmActionInfo(const AMControlInfo &triggerManagerInfo, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	triggerManagerInfo_.setValuesFrom(triggerManagerInfo);

	updateDescriptionText();
}

AMTriggerManagerArmActionInfo::AMTriggerManagerArmActionInfo(const AMTriggerManagerArmActionInfo &original) :
	AMActionInfo3(original)
{
	triggerManagerInfo_.setValuesFrom(original.triggerManagerInfo());
}

AMTriggerManagerArmActionInfo::~AMTriggerManagerArmActionInfo()
{

}

AMActionInfo3* AMTriggerManagerArmActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMTriggerManagerArmActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMTriggerManagerArmActionInfo::updateDescriptionText()
{
	QString managerName = (triggerManagerInfo_.description().isEmpty() ? triggerManagerInfo_.name() : triggerManagerInfo_.description());

	QString newText = QString("Arming trigger manager %1").arg(managerName);

	setShortDescription(newText);
	setLongDescription(newText);
}
