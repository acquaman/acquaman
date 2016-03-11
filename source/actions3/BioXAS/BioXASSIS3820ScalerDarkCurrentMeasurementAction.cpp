#include "BioXASSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "dataman/info/AMDetectorInfo.h"
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

void BioXASSIS3820ScalerDarkCurrentMeasurementAction::measurementCleanup()
{
	// General cleanup.

	CLSSIS3820ScalerDarkCurrentMeasurementAction::measurementCleanup();

	// BioXAS-specific cleanup.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra) {

		zebra->clearDetectors();

		foreach (AMDetector *detector, originalDetectors_)
			zebra->addDetector(detector);
	}
}
