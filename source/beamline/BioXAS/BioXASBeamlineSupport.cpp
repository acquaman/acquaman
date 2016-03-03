#include "BioXASBeamlineSupport.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/AMDetector.h"
#include "dataman/AMScan.h"
#include "dataman/datasource/AMDataSource.h"

#include <QDebug>
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

	if (result)
		qDebug() << "Configurating using scaler.";
	else
		qDebug() << "Configuration NOT using scaler.";

	return result;
}

bool usingZebra(AMGenericStepScanConfiguration *configuration)
{
	bool result = false;

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra)
		result = ( usingAnyGeDetector(configuration) || usingScaler(configuration) );

	if (result)
		qDebug() << "Configuration using zebra.";
	else
		qDebug() << "Configuration NOT using zebra.";

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

	if (result)
		qDebug() << "Configuration using detector" << detector->name();

	return result;
}

bool usingMono(AMGenericStepScanConfiguration *configuration)
{
	bool result = false;

	BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();

	if (mono)
		result = ( usingControl(configuration, mono->energy()) || usingControl(configuration, mono->energy()->braggControl()) );

	if (result)
		qDebug() << "Configuration using mono.";
	else
	return result;
}

bool usingControl(AMGenericStepScanConfiguration *configuration, AMControl *control)
{
	bool result = false;

	if (configuration && control)
		result = (configuration->axisControlInfos().indexOf(control->name()));

	if (result)
		qDebug() << "Configuration uses control" << control->name();
	else if (control)
		qDebug() << "Configuration does NOT use control" << control->name();

	return result;
}

bool usingStandardsWheel(AMGenericStepScanConfiguration *configuration)
{
	bool result = false;

	BioXASXASScanConfiguration *bioxasConfiguration = qobject_cast<BioXASXASScanConfiguration*>(configuration);

	CLSStandardsWheel *standardsWheel = BioXASBeamline::bioXAS()->standardsWheel();

	if (bioxasConfiguration && standardsWheel)
		result = (bioxasConfiguration && standardsWheel->indexFromName(bioxasConfiguration->edge().split(" ").first()) != -1);

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

	if (result)
		qDebug() << "Scan using detector" << detector->name();
	else if (detector)
		qDebug() << "Scan NOT using detector" << detector->name();

	return result;
}

AMDataSource* i0DetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->i0Detector());
}

AMDataSource* i1DetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->i1Detector());
}

AMDataSource* i2DetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->i2Detector());
}

AMDataSource* diodeDetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->diodeDetector());
}

AMDataSource* pipsDetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->pipsDetector());
}

AMDataSource* lytleDetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->lytleDetector());
}

AMDataSource* geDetectorSource(AMScan *scan, BioXAS32ElementGeDetector *detector)
{
	return detectorSource(scan, detector);
}

AMDataSource* scalerDwellTimeDetectorSource(AMScan *scan)
{
	return detectorSource(scan, BioXASBeamline::bioXAS()->scalerDwellTimeDetector());
}

AMDataSource* detectorSource(AMScan *scan, AMDetector *detector)
{
	AMDataSource *result = 0;

	if (scan && detector)
		result = scan->dataSourceAt(scan->indexOfDataSource(detector->name()));

	if (result)
		qDebug() << "Scan detector source found for detector" << detector->name();

	return result;
}

}
