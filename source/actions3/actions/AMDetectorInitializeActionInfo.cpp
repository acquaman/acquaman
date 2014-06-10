#include "AMDetectorInitializeActionInfo.h"

 AMDetectorInitializeActionInfo::~AMDetectorInitializeActionInfo(){}
AMDetectorInitializeActionInfo::AMDetectorInitializeActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
	AMActionInfo3("Detector Initialization", "Detector Initialization", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);

	QString description = QString("Initialize %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorInitializeActionInfo::AMDetectorInitializeActionInfo(const AMDetectorInitializeActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}
