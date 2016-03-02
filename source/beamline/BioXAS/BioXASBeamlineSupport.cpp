#include "BioXASBeamlineSupport.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/AMDetector.h"
#include "dataman/AMScan.h"
#include "dataman/datasource/AMDataSource.h"

namespace BioXASBeamlineSupport {

bool usingI0Detector(AMGenericStepScanConfiguration *configuration)
{
	return usingDetector(configuration, BioXASBeamline::bioXAS()->i0Detector());
}

bool usingI1Detector(AMGenericStepScanConfiguration *configuration)
{
	return usingDetector(configuration, BioXASBeamline::bioXAS()->i1Detector());
}

bool usingI2Detector(AMGenericStepScanConfiguration *configuration)
{
	return usingDetector(configuration, BioXASBeamline::bioXAS()->i2Detector());
}

bool usingDiodeDetector(AMGenericStepScanConfiguration *configuration)
{
	return usingDetector(configuration, BioXASBeamline::bioXAS()->diodeDetector());
}

bool usingPIPSDetector(AMGenericStepScanConfiguration *configuration)
{
	return usingDetector(configuration, BioXASBeamline::bioXAS()->pipsDetector());
}

bool usingLytleDetector(AMGenericStepScanConfiguration *configuration)
{
	return usingDetector(configuration, BioXASBeamline::bioXAS()->lytleDetector());
}

bool usingScaler(AMGenericStepScanConfiguration *configuration)
{
	bool result = false;

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (configuration && scaler)
		result = ( usingI0Detector(configuration) || usingI1Detector(configuration) || usingI2Detector(configuration) );

	return result;
}

bool usingZebra(AMGenericStepScanConfiguration *configuration)
{
	bool result = false;

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra)
		result = ( usingAnyGeDetector(configuration) || usingScaler(configuration) );

	return result;
}

bool usingGeDetector(AMGenericStepScanConfiguration *configuration, BioXAS32ElementGeDetector *detector)
{
	return usingDetector(configuration, detector);
}

bool usingAnyGeDetector(AMGenericStepScanConfiguration *configuration)
{
	bool usingGeDetector = false;

	AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	if (geDetectors && !geDetectors->isEmpty()) {

		for (int i = 0, count = geDetectors->count(); i < count && !usingGeDetector; i++)
			usingGeDetector = usingDetector(configuration, geDetectors->at(i));
	}

	return usingGeDetector;
}

bool usingDetector(AMGenericStepScanConfiguration *configuration, AMDetector *detector)
{
	bool result = false;

	if (configuration && detector)
		result = (configuration->detectorConfigurations().indexOf(detector->name()) != -1);

	return result;
}

bool usingMono(AMGenericStepScanConfiguration *configuration)
{
	bool result = false;

	BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();

	if (mono)
		result = usingControl(configuration, mono->energy());

	return result;
}

bool usingControl(AMGenericStepScanConfiguration *configuration, AMControl *control)
{
	bool result = false;

	if (configuration && control)
		result = (configuration->axisControlInfos().indexOf(control->name()));

	return result;
}

bool usingI0Detector(AMScan *scan)
{
	return usingDetector(scan, BioXASBeamline::bioXAS()->i0Detector());
}

bool usingI1Detector(AMScan *scan)
{
	return usingDetector(scan, BioXASBeamline::bioXAS()->i1Detector());
}

bool usingI2Detector(AMScan *scan)
{
	return usingDetector(scan, BioXASBeamline::bioXAS()->i2Detector());
}

bool usingDiodeDetector(AMScan *scan)
{
	return usingDetector(scan, BioXASBeamline::bioXAS()->diodeDetector());
}

bool usingPIPSDetector(AMScan *scan)
{
	return usingDetector(scan, BioXASBeamline::bioXAS()->pipsDetector());
}

bool usingLytleDetector(AMScan *scan)
{
	return usingDetector(scan, BioXASBeamline::bioXAS()->lytleDetector());
}

bool usingScaler(AMScan *scan)
{
	bool result = false;

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (scan && scaler)
		result = ( usingI0Detector(scan) || usingI1Detector(scan) || usingI2Detector(scan) );

	return result;
}

bool usingZebra(AMScan *scan)
{
	bool result = false;

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra)
		result = ( usingAnyGeDetector(scan) || usingScaler(scan) );

	return result;
}

bool usingGeDetector(AMScan *scan, BioXAS32ElementGeDetector *detector)
{
	return usingDetector(scan, detector);
}

bool usingAnyGeDetector(AMScan *scan)
{
	bool usingGeDetector = false;

	AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	if (geDetectors && !geDetectors->isEmpty()) {

		for (int i = 0, count = geDetectors->count(); i < count && !usingGeDetector; i++)
			usingGeDetector = usingDetector(scan, geDetectors->at(i));
	}

	return usingGeDetector;
}

bool usingDetector(AMScan *scan, AMDetector *detector)
{
	bool result = false;

	if (scan && detector)
		result = (scan->indexOfDataSource(detector->name()) != -1);

	return result;
}


}
