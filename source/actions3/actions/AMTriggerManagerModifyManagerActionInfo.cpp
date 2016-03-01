#include "AMTriggerManagerModifyManagerActionInfo.h"

AMTriggerManagerModifyManagerActionInfo::AMTriggerManagerModifyManagerActionInfo(const AMControlInfo &triggerManagerInfo, AMTriggerManagerModifyManagerActionInfo::Option option, const AMDetectorInfo &detectorInfo, const AMControlInfo &managerInfo, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	triggerManagerInfo_.setValuesFrom(triggerManagerInfo);
	option_ = option;
	detectorInfo_.setValuesFrom(detectorInfo);
	managerInfo_.setValuesFrom(managerInfo);
}

AMTriggerManagerModifyManagerActionInfo::AMTriggerManagerModifyManagerActionInfo(const AMTriggerManagerModifyManagerActionInfo &original) :
	AMActionInfo3(original)
{
	triggerManagerInfo_.setValuesFrom(original.triggerManagerInfo());
	option_ = original.option();
	detectorInfo_.setValuesFrom(original.detectorInfo());
	managerInfo_.setValuesFrom(original.managerInfo());
}

AMTriggerManagerModifyManagerActionInfo::~AMTriggerManagerModifyManagerActionInfo()
{

}

AMActionInfo3* AMTriggerManagerModifyManagerActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMTriggerManagerModifyManagerActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMTriggerManagerModifyManagerActionInfo::updateDescriptionText()
{
	QString newText = "";

	if (option_ == AddDetector)
		newText = QString("Adding %1 to the list of %2 detectors.").arg(detectorInfo_.name()).arg(triggerManagerInfo_.name());
	else if (option_ == RemoveDetector)
		newText = QString("Removing %1 from the list of %2 detectors.").arg(detectorInfo_.name()).arg(triggerManagerInfo_.name());
	else if (option_ == ClearDetectors)
		newText = QString("Clearing all %1 detectors").arg(triggerManagerInfo_.name());
	else if (option_ == AddManager)
		newText = QString("Adding %1 to the list of %2 managers.").arg(managerInfo_.name()).arg(triggerManagerInfo_.name());
	else if (option_ == RemoveManager)
		newText = QString("Removing %1 from the list of %2 managers.").arg(managerInfo_.name()).arg(triggerManagerInfo_.name());
	else if (option_ == ClearManagers)
		newText = QString("Clearing all %1 managers.").arg(triggerManagerInfo_.name());
	else
		newText = QString("Unknown option. Not sure what we're doing with the %1 detectors/managers.").arg(triggerManagerInfo_.name());

	setShortDescription(newText);
	setLongDescription(newText);
}
