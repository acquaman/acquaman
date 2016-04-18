#include "AMDetectorWaitForAcquisitionStateActionInfo.h"

AMDetectorWaitForAcquisitionStateActionInfo::AMDetectorWaitForAcquisitionStateActionInfo(const AMDetectorInfo &detectorInfo, int acquisitionState, QObject *parent)
	: AMActionInfo3("Detector Wait For Acquisition State", "Detector Wait For Acquisition State", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	acquisitionState_ = acquisitionState;

	QString description = QString("Wait For Acquisition State for %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorWaitForAcquisitionStateActionInfo::AMDetectorWaitForAcquisitionStateActionInfo(const AMDetectorWaitForAcquisitionStateActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	acquisitionState_ = other.acquisitionState();
}

AMDetectorWaitForAcquisitionStateActionInfo::~AMDetectorWaitForAcquisitionStateActionInfo()
{

}

AMActionInfo3 *AMDetectorWaitForAcquisitionStateActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMDetectorWaitForAcquisitionStateActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMDetectorWaitForAcquisitionStateActionInfo::setAcquisitionState(int newState)
{
	acquisitionState_ = newState;
	setModified(true);
}



