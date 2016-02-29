#include "SGMContinuousScanControllerAssembler.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"

SGMContinuousScanControllerAssembler::SGMContinuousScanControllerAssembler(bool automaticDirectionAssessment, AMScanConfiguration::Direction direction, QObject *parent) :
    AMGenericScanActionControllerAssembler(automaticDirectionAssessment, direction, parent)
{
}

QList<AMDetector *> SGMContinuousScanControllerAssembler::generateListOfDetectorsToConfigure() const
{
	QList<AMDetector*> detectorsToConfigure;

	bool foundOneScaler = false;
	for(int x = 0, size = detectors_->count(); x < size; x++){
		CLSAMDSScalerChannelDetector *asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(detectors_->at(x));
		if(asScalerChannelDetector && !foundOneScaler){
			foundOneScaler = true;
			detectorsToConfigure.append(detectors_->at(x));
		}
		else if(!asScalerChannelDetector) {
			detectorsToConfigure.append(detectors_->at(x));
		}
	}

	return detectorsToConfigure;
}
