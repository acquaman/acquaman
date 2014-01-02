#include "AMDetectorCleanupActionInfo.h"

 AMDetectorCleanupActionInfo::~AMDetectorCleanupActionInfo(){}
AMDetectorCleanupActionInfo::AMDetectorCleanupActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
	AMActionInfo3("Detector Cleanup", "Detector Cleanup", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);

	QString description = QString("Cleanup %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorCleanupActionInfo::AMDetectorCleanupActionInfo(const AMDetectorCleanupActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}
