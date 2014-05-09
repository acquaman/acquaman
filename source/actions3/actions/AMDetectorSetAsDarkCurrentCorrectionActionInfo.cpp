#include "AMDetectorSetAsDarkCurrentCorrectionActionInfo.h"


AMDetectorSetAsDarkCurrentCorrectionActionInfo::AMDetectorSetAsDarkCurrentCorrectionActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
   AMActionInfo3("Detector Set As Dark Current Correction", "Detector Set As Dark Current Correction", ":/system-run.png", parent)
{
   detectorInfo_.setValuesFrom(detectorInfo);

   QString description = QString("Set As Dark Current Correction %1").arg(detectorInfo_.description());
   setShortDescription(description);
   setLongDescription(description);
}

AMDetectorSetAsDarkCurrentCorrectionActionInfo::~AMDetectorSetAsDarkCurrentCorrectionActionInfo(){}

AMDetectorSetAsDarkCurrentCorrectionActionInfo::AMDetectorSetAsDarkCurrentCorrectionActionInfo(const AMDetectorSetAsDarkCurrentCorrectionActionInfo &other) :
   AMActionInfo3(other)
{
   detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}
