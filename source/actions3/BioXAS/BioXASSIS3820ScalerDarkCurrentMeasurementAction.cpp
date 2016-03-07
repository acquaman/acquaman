#include "BioXASSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "dataman/info/AMDetectorInfo.h"
#include "beamline/CLS/CLSBeamline.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "util/AMErrorMonitor.h"

BioXASSIS3820ScalerDarkCurrentMeasurementAction::BioXASSIS3820ScalerDarkCurrentMeasurementAction(CLSSIS3820ScalerDarkCurrentMeasurementActionInfo *info, QObject *parent) :
	CLSSIS3820ScalerDarkCurrentMeasurementAction(info, parent)
{

}

BioXASSIS3820ScalerDarkCurrentMeasurementAction::BioXASSIS3820ScalerDarkCurrentMeasurementAction(const BioXASSIS3820ScalerDarkCurrentMeasurementAction &other) :
	CLSSIS3820ScalerDarkCurrentMeasurementAction(other)
{

}

BioXASSIS3820ScalerDarkCurrentMeasurementAction::~BioXASSIS3820ScalerDarkCurrentMeasurementAction()
{

}

void BioXASSIS3820ScalerDarkCurrentMeasurementAction::measurementInitialization()
{
	// General initialization.

	CLSSIS3820ScalerDarkCurrentMeasurementAction::measurementInitialization();

	// BioXAS-specific initialization.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		AMZebraDetectorTriggerSource *triggerSource = qobject_cast<AMZebraDetectorTriggerSource*>(scaler->triggerSource());

		if (triggerSource) {
			originalDetectors_ = triggerSource->detectors();
			originalDetectorManagers_ = triggerSource->detectorManagers();

			// Update the detectors and managers.

			triggerSource->removeAllDetectors();
			triggerSource->removeAllDetectorManagers();

			AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
			if (i0Detector)
				triggerSource->addDetector(i0Detector);

			AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();
			if (i1Detector)
				triggerSource->addDetector(i1Detector);

			AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();
			if (i2Detector)
				triggerSource->addDetector(i2Detector);

			triggerSource->addDetectorManager(scaler);
		}
	}
}

void BioXASSIS3820ScalerDarkCurrentMeasurementAction::measurementCleanup()
{
	// General cleanup.

	CLSSIS3820ScalerDarkCurrentMeasurementAction::measurementCleanup();

	// BioXAS-specific cleanup.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		AMZebraDetectorTriggerSource *triggerSource = qobject_cast<AMZebraDetectorTriggerSource*>(scaler->triggerSource());

		if (triggerSource) {

			// Update the detectors and managers.

			triggerSource->removeAllDetectors();
			triggerSource->removeAllDetectorManagers();

			foreach (AMDetector* detector, originalDetectors_)
				triggerSource->addDetector(detector);

			foreach (QObject* manager, originalDetectorManagers_)
				triggerSource->addDetectorManager(manager);
		}
	}
}
