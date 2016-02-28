#include "AMDetectorManagerModifyManagerActionInfo.h"

AMDetectorManagerModifyManagerActionInfo::AMDetectorManagerModifyManagerActionInfo(const AMControlInfo &detectorManagerInfo, AMDetectorManagerModifyManagerActionInfo::Option option, const AMDetectorInfo &detectorInfo, const AMControlInfo &managerInfo, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	detectorManagerInfo_.setValuesFrom(detectorManagerInfo);
	option_ = option;
	detectorInfo_.setValuesFrom(detectorInfo);
	managerInfo_.setValuesFrom(managerInfo);
}

AMDetectorManagerModifyManagerActionInfo::AMDetectorManagerModifyManagerActionInfo(const AMDetectorManagerModifyManagerActionInfo &original) :
	AMActionInfo3(original)
{
	detectorManagerInfo_.setValuesFrom(original.detectorManagerInfo());
	option_ = original.option();
	detectorInfo_.setValuesFrom(original.detectorInfo());
	managerInfo_.setValuesFrom(original.managerInfo());
}

AMDetectorManagerModifyManagerActionInfo::~AMDetectorManagerModifyManagerActionInfo()
{

}

AMActionInfo3* AMDetectorManagerModifyManagerActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorManagerModifyManagerActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorManagerModifyManagerActionInfo::setDetectorManagerInfo(const AMControlInfo &newInfo)
{
	detectorManagerInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void AMDetectorManagerModifyManagerActionInfo::setOption(AMDetectorManagerModifyManagerActionInfo::Option newOption)
{
	option_ = newOption;
	setModified(true);

	updateDescriptionText();
}

void AMDetectorManagerModifyManagerActionInfo::setDetectorInfo(const AMDetectorInfo &newInfo)
{
	detectorInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void AMDetectorManagerModifyManagerActionInfo::setManagerInfo(const AMControlInfo &newInfo)
{
	managerInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void AMDetectorManagerModifyManagerActionInfo::updateDescriptionText()
{
	QString newText = "";

	if (option_ == AddDetector)
		newText = QString("Adding %1 to the list of %2 detectors.").arg(detectorInfo_.name()).arg(detectorManagerInfo_.name());
	else if (option_ == RemoveDetector)
		newText = QString("Removing %1 from the list of %2 detectors.").arg(detectorInfo_.name()).arg(detectorManagerInfo_.name());
	else if (option_ == ClearDetectors)
		newText = QString("Clearing all %1 detectors").arg(detectorManagerInfo_.name());
	else if (option_ == AddManager)
		newText = QString("Adding %1 to the list of %2 managers.").arg(managerInfo_.name()).arg(detectorManagerInfo_.name());
	else if (option_ == RemoveManager)
		newText = QString("Removing %1 from the list of %2 managers.").arg(managerInfo_.name()).arg(detectorManagerInfo_.name());
	else if (option_ == ClearManagers)
		newText = QString("Clearing all %1 managers.").arg(detectorManagerInfo_.name());
	else
		newText = QString("Unknown option. Not sure what we're doing with the %1 detectors/managers.").arg(detectorManagerInfo_.name());

	setShortDescription(newText);
	setLongDescription(newText);
}
