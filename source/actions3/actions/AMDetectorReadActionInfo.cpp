#include "AMDetectorReadActionInfo.h"

 AMDetectorReadActionInfo::~AMDetectorReadActionInfo(){}
AMDetectorReadActionInfo::AMDetectorReadActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
	AMActionInfo3("Detector Read", "Detector Read", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);

	QString description = QString("Read %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorReadActionInfo::AMDetectorReadActionInfo(const AMDetectorReadActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}
