#include "BioXASZebraModifyDetectorsActionInfo.h"

BioXASZebraModifyDetectorsActionInfo::BioXASZebraModifyDetectorsActionInfo(BioXASZebraModifyDetectorsActionInfo::Option option, const AMDetectorInfo &info, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	option_ = option;
	detectorInfo_.setValuesFrom(info);

	updateDescriptionText();
}

BioXASZebraModifyDetectorsActionInfo::BioXASZebraModifyDetectorsActionInfo(const BioXASZebraModifyDetectorsActionInfo &original):
	AMActionInfo3(original)
{
	option_ = original.option();
	detectorInfo_.setValuesFrom(original.detectorInfo());

	updateDescriptionText();
}

BioXASZebraModifyDetectorsActionInfo::~BioXASZebraModifyDetectorsActionInfo()
{

}

AMActionInfo3* BioXASZebraModifyDetectorsActionInfo::createCopy() const
{
	AMActionInfo3 *info = new BioXASZebraModifyDetectorsActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void BioXASZebraModifyDetectorsActionInfo::updateDescriptionText()
{
	QString newText = "";

	if (option_ == AddDetector)
		newText = QString("Adding %1 to list of Zebra detectors.").arg(detectorInfo_.name().isEmpty() ? "???" : detectorInfo_.name());
	else if (option_ == RemoveDetector)
		newText = QString("Removing %1 from list of Zebra detectors.").arg(detectorInfo_.name().isEmpty() ? "???" : detectorInfo_.name());
	else if (option_ == ClearDetectors)
		newText = QString("Clearing all Zebra detectors.");
	else
		newText = QString("Unknown action. Not sure what we're doing with the Zebra detectors.");

	setShortDescription(newText);
	setLongDescription(newText);
}
