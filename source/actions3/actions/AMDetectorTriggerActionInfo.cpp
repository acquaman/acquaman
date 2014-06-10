#include "AMDetectorTriggerActionInfo.h"

 AMDetectorTriggerActionInfo::~AMDetectorTriggerActionInfo(){}
AMDetectorTriggerActionInfo::AMDetectorTriggerActionInfo(const AMDetectorInfo &detectorInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3("Detector Trigger", "Detector Trigger", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	readMode_ = readMode;

	QString description = QString("Trigger %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorTriggerActionInfo::AMDetectorTriggerActionInfo(const AMDetectorTriggerActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	readMode_ = other.readMode();
}
