#include "SXRMBEXAFSScanActionController.h"

#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "beamline/SXRMB/SXRMBBeamline.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

SXRMBEXAFSScanActionController::SXRMBEXAFSScanActionController(SXRMBEXAFSScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(configuration);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	AMControlInfoList list;
	list.append(SXRMBBeamline::sxrmb()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet sxrmbDetectors;
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->i0Detector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->teyDetector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->energyFeedbackDetector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->brukerDetector()->toInfo());
	configuration_->setDetectorConfigurations(sxrmbDetectors);
}

SXRMBEXAFSScanActionController::~SXRMBEXAFSScanActionController()
{
}

AMAction3* SXRMBEXAFSScanActionController::createInitializationActions()
{
	return 0;
}

AMAction3* SXRMBEXAFSScanActionController::createCleanupActions()
{
	return 0;
}

void SXRMBEXAFSScanActionController::cancelImplementation()
{

}

void SXRMBEXAFSScanActionController::onInitializationActionsListSucceeded()
{

}

void SXRMBEXAFSScanActionController::buildScanControllerImplementation()
{

}

void SXRMBEXAFSScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}
