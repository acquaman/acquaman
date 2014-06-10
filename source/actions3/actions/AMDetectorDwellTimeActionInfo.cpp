#include "AMDetectorDwellTimeActionInfo.h"

 AMDetectorDwellTimeActionInfo::~AMDetectorDwellTimeActionInfo(){}
AMDetectorDwellTimeActionInfo::AMDetectorDwellTimeActionInfo(const AMDetectorInfo &detectorInfo, double dwellSeconds, QObject *parent) :
	AMActionInfo3("Detector Dwell", "Detector Dwell", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	dwellSeconds_ = dwellSeconds;

	QString description = QString("Dwell Time %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorDwellTimeActionInfo::AMDetectorDwellTimeActionInfo(const AMDetectorDwellTimeActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	dwellSeconds_ = other.dwellSeconds();
}
