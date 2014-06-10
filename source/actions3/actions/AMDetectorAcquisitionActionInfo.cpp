#include "AMDetectorAcquisitionActionInfo.h"

 AMDetectorAcquisitionActionInfo::~AMDetectorAcquisitionActionInfo(){}
AMDetectorAcquisitionActionInfo::AMDetectorAcquisitionActionInfo(const AMDetectorInfo &detectorInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3("Detector Acquisition", "Detector Acquisition", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	readMode_ = readMode;

	QString description = QString("Acquire %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorAcquisitionActionInfo::AMDetectorAcquisitionActionInfo(const AMDetectorAcquisitionActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	readMode_ = other.readMode();
}
