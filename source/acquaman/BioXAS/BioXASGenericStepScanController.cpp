#include "BioXASGenericStepScanController.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASGenericStepScanController::BioXASGenericStepScanController(BioXASGenericStepScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent)
{
	useFeedback_ = true;

	// Add the Ge detectors spectra, if a Ge detector is being used.

	AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	if (geDetectors) {

		for (int i = 0, detectorsCount = geDetectors->count(); i < detectorsCount; i++) {
			BioXAS32ElementGeDetector *geDetector = qobject_cast<BioXAS32ElementGeDetector*>(geDetectors->at(i));

			if (geDetector && configuration_->detectorConfigurations().contains(geDetector->name())) {
				AMDetectorSet *elements = BioXASBeamline::bioXAS()->elementsForDetector(geDetector);

				if (elements) {
					for (int j = 0, elementsCount = elements->count(); j < elementsCount; j++) {
						AMDetector *element = elements->at(j);

						if (element)
							configuration_->addDetector(element->toInfo());
					}
				}
			}
		}
	}
}

BioXASGenericStepScanController::~BioXASGenericStepScanController()
{

}

void BioXASGenericStepScanController::buildScanControllerImplementation()
{
	AMGenericStepScanController::buildScanControllerImplementation();

	// Identify and setup the zebra trigger source.

	AMZebraDetectorTriggerSource *zebraTriggerSource = BioXASBeamline::bioXAS()->zebraTriggerSource();

	if (zebraTriggerSource) {
		zebraTriggerSource->removeAllDetectors();
		zebraTriggerSource->removeAllDetectorManagers();

		if (BioXASBeamlineSupport::usingI0Detector(scan_))
			zebraTriggerSource->addDetector(BioXASBeamline::bioXAS()->i0Detector());

		if (BioXASBeamlineSupport::usingI1Detector(scan_))
			zebraTriggerSource->addDetector(BioXASBeamline::bioXAS()->i1Detector());

		if (BioXASBeamlineSupport::usingI2Detector(scan_))
			zebraTriggerSource->addDetector(BioXASBeamline::bioXAS()->i2Detector());

		if (BioXASBeamlineSupport::usingScaler(scan_))
			zebraTriggerSource->addDetectorManager(BioXASBeamline::bioXAS()->scaler());

		AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

		for (int i = 0, count = geDetectors->count(); i < count; i++) {
			AMDetector *detector = geDetectors->at(i);

			if (detector && BioXASBeamlineSupport::usingDetector(scan_, detector)) {
				zebraTriggerSource->addDetector(detector);
				zebraTriggerSource->addDetectorManager(detector);
			}
		}
	}
}
