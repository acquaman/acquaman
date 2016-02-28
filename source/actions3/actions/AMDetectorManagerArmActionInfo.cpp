#include "AMDetectorManagerArmActionInfo.h"

AMDetectorManagerArmActionInfo::AMDetectorManagerArmActionInfo(const AMControlInfo &detectorManagerInfo, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	detectorManagerInfo_.setValuesFrom(detectorManagerInfo);
}

AMDetectorManagerArmActionInfo::AMDetectorManagerArmActionInfo(const AMDetectorManagerArmActionInfo &original) :
	AMActionInfo3(original)
{
	detectorManagerInfo_.setValuesFrom(original.detectorManagerInfo());
}

AMDetectorManagerArmActionInfo::~AMDetectorManagerArmActionInfo()
{

}

AMActionInfo3* AMDetectorManagerArmActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorManagerArmActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorManagerArmActionInfo::setDetectorManagerInfo(const AMControlInfo &newInfo)
{
	detectorManagerInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void AMDetectorManagerArmActionInfo::updateDescriptionText()
{
	QString managerName = (detectorManagerInfo_.description().isEmpty() ? detectorManagerInfo_.name() : detectorManagerInfo_.description());

	QString newText = QString("Arming detector manager %1").arg(managerName);

	setShortDescription(newText);
	setLongDescription(newText);
}
