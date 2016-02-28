#include "AMDetectorManagerTriggerActionInfo.h"

AMDetectorManagerTriggerActionInfo::AMDetectorManagerTriggerActionInfo(const AMControlInfo &detectorManagerInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	readMode_ = readMode;
	detectorManagerInfo_.setValuesFrom(detectorManagerInfo);
}

AMDetectorManagerTriggerActionInfo::AMDetectorManagerTriggerActionInfo(const AMDetectorManagerTriggerActionInfo &original) :
	AMActionInfo3(original)
{
	readMode_ = original.readMode();
	detectorManagerInfo_.setValuesFrom(original.detectorManagerInfo());
}

AMDetectorManagerTriggerActionInfo::~AMDetectorManagerTriggerActionInfo()
{

}

AMActionInfo3* AMDetectorManagerTriggerActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorManagerTriggerActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorManagerTriggerActionInfo::setDetectorManagerInfo(const AMControlInfo &newInfo)
{
	detectorManagerInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void AMDetectorManagerTriggerActionInfo::setReadMode(AMDetectorDefinitions::ReadMode newMode)
{
	if (readMode_ != newMode) {
		readMode_ = newMode;
		setModified(true);

		updateDescriptionText();
	}
}

QString AMDetectorManagerTriggerActionInfo::readModeToString(int mode) const
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

void AMDetectorManagerTriggerActionInfo::updateDescriptionText()
{
	QString managerName = (detectorManagerInfo_.description().isEmpty() ? detectorManagerInfo_.name() : detectorManagerInfo_.description());

	QString newText = QString("Triggering detector manager %1 for a %2 acquisition.").arg(managerName).arg(readModeToString(readMode_));

	setShortDescription(newText);
	setLongDescription(newText);
}
