#include "BioXASZebraModifyDetectorsActionInfo.h"

BioXASZebraModifyDetectorsActionInfo::BioXASZebraModifyDetectorsActionInfo(BioXASZebraModifyDetectorsActionInfo::Action action, const AMDetectorInfo &info, QObject *parent) :
	AMActionInfo3(typeDescription(), typeDescription(), QString(), parent)
{
	action_ = action;
}

BioXASZebraModifyDetectorsActionInfo::BioXASZebraModifyDetectorsActionInfo(const BioXASZebraModifyDetectorsActionInfo &original):
	AMActionInfo3(original)
{
	action_ = original.action();
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

void BioXASZebraModifyDetectorsActionInfo::setAction(BioXASZebraModifyDetectorsActionInfo::Action newAction)
{
	action_ = newAction;
	setModified(true);

	updateDescriptionText();
}

void BioXASZebraModifyDetectorsActionInfo::setDetectorInfo(const AMDetectorInfo &newInfo)
{
	detectorInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void BioXASZebraModifyDetectorsActionInfo::updateDescriptionText()
{
	QString newText = "";

	if (action_ == AddDetector)
		newText = QString("Adding %1 to list of Zebra detectors.").arg(detectorInfo_.name().isEmpty() ? "???" : detectorInfo_.name());
	else if (action_ == RemoveDetector)
		newText = QString("Removing %1 from list of Zebra detectors.").arg(detectorInfo_.name().isEmpty() ? "???" : detectorInfo_.name());
	else if (action_ == ClearDetectors)
		newText = QString("Clearing all Zebra detectors.");
	else
		newText = QString("Unknown action. Not sure what we're doing with the Zebra detectors.");

	setShortDescription(newText);
	setLongDescription(newText);
}
