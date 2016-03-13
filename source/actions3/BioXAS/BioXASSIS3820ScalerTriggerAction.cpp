#include "BioXASSIS3820ScalerTriggerAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASSIS3820ScalerTriggerAction::BioXASSIS3820ScalerTriggerAction(CLSSIS3820ScalerTriggerActionInfo *info, QObject *parent) :
	CLSSIS3820ScalerTriggerAction(info, parent)
{

}

BioXASSIS3820ScalerTriggerAction::BioXASSIS3820ScalerTriggerAction(const BioXASSIS3820ScalerTriggerAction &original) :
	CLSSIS3820ScalerTriggerAction(original)
{

}

BioXASSIS3820ScalerTriggerAction::~BioXASSIS3820ScalerTriggerAction()
{

}

void BioXASSIS3820ScalerTriggerAction::triggerInitialization()
{
	// General initialization.

	CLSSIS3820ScalerTriggerAction::triggerInitialization();

	// BioXAS-specific initialization.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra) {

		originalDetectors_ = zebra->detectors();

		zebra->clearDetectors();

		if (BioXASBeamline::bioXAS()->i0Detector())
			zebra->addDetector(BioXASBeamline::bioXAS()->i0Detector());

		if (BioXASBeamline::bioXAS()->i1Detector())
			zebra->addDetector(BioXASBeamline::bioXAS()->i1Detector());

		if (BioXASBeamline::bioXAS()->i2Detector())
			zebra->addDetector(BioXASBeamline::bioXAS()->i2Detector());

		if (BioXASBeamline::bioXAS()->diodeDetector())
			zebra->addDetector(BioXASBeamline::bioXAS()->diodeDetector());

		if (BioXASBeamline::bioXAS()->pipsDetector())
			zebra->addDetector(BioXASBeamline::bioXAS()->pipsDetector());

		if (BioXASBeamline::bioXAS()->lytleDetector())
			zebra->addDetector(BioXASBeamline::bioXAS()->lytleDetector());
	}
}

void BioXASSIS3820ScalerTriggerAction::triggerCleanup()
{
	// General cleanup.

	CLSSIS3820ScalerTriggerAction::triggerCleanup();

	// BioXAS-specific cleanup.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra) {

		zebra->clearDetectors();

		foreach (AMDetector *detector, originalDetectors_)
			zebra->addDetector(detector);
	}
}
