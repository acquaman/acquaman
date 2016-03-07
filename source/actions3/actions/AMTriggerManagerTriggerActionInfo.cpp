#include "AMTriggerManagerTriggerActionInfo.h"

AMTriggerManagerTriggerActionInfo::AMTriggerManagerTriggerActionInfo(const AMControlInfo &triggerManagerInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	readMode_ = readMode;
	triggerManagerInfo_.setValuesFrom(triggerManagerInfo);

	updateDescriptionText();
}

AMTriggerManagerTriggerActionInfo::AMTriggerManagerTriggerActionInfo(const AMTriggerManagerTriggerActionInfo &original) :
	AMActionInfo3(original)
{
	readMode_ = original.readMode();
	triggerManagerInfo_.setValuesFrom(original.triggerManagerInfo());

	updateDescriptionText();
}

AMTriggerManagerTriggerActionInfo::~AMTriggerManagerTriggerActionInfo()
{

}

AMActionInfo3* AMTriggerManagerTriggerActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMTriggerManagerTriggerActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

QString AMTriggerManagerTriggerActionInfo::readModeToString(int mode) const
{
	QString result = "";

	if (mode == AMDetectorDefinitions::SingleRead)
		result = "Single Read";
	else if (mode == AMDetectorDefinitions::ContinuousRead)
		result = "Continuous Read";
	else
		result = "Unknown";

	return result;
}

void AMTriggerManagerTriggerActionInfo::updateDescriptionText()
{
	QString managerName = (triggerManagerInfo_.description().isEmpty() ? triggerManagerInfo_.name() : triggerManagerInfo_.description());

	QString newText = QString("Triggering trigger manager %1 for a %2 acquisition.").arg(managerName).arg(readModeToString(readMode_));

	setShortDescription(newText);
	setLongDescription(newText);
}
