#include "BioXASGenericStepScanController.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "application/BioXAS/BioXAS.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASGenericStepScanController::BioXASGenericStepScanController(BioXASGenericStepScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent), BioXASScanController()
{
	useFeedback_ = true;
	setGeneralScanControllerScan(scan_);

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

	// Identify exporter option.

	AMExporterOptionXDIFormat *genericExporterOption = 0;

	if (configuration_) {

		genericExporterOption = BioXAS::buildStandardXDIFormatExporterOption("BioXAS Generic Step Scan (XDI Format)", "", "", true);

		if (genericExporterOption->id() > 0)
			AMAppControllerSupport::registerClass<BioXASGenericStepScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(genericExporterOption->id());

		// Clear the option of any previous data sources.

		genericExporterOption->clearDataSources();
	}

	// Identify and setup the zebra trigger source.

	AMZebraDetectorTriggerSource *zebraTriggerSource = BioXASBeamline::bioXAS()->zebraTriggerSource();

	if (zebraTriggerSource) {
		zebraTriggerSource->removeAllDetectors();
		zebraTriggerSource->removeAllDetectorManagers();

		if (usingDetector(BioXASBeamline::bioXAS()->i0Detector()))
			zebraTriggerSource->addDetector(BioXASBeamline::bioXAS()->i0Detector());

		if (usingDetector(BioXASBeamline::bioXAS()->i1Detector()))
			zebraTriggerSource->addDetector(BioXASBeamline::bioXAS()->i1Detector());

		if (usingDetector(BioXASBeamline::bioXAS()->i2Detector()))
			zebraTriggerSource->addDetector(BioXASBeamline::bioXAS()->i2Detector());

		if (usingScaler())
			zebraTriggerSource->addDetectorManager(BioXASBeamline::bioXAS()->scaler());

		AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

		for (int i = 0, count = geDetectors->count(); i < count; i++) {
			AMDetector *detector = geDetectors->at(i);

			if (usingDetector(detector)) {
				zebraTriggerSource->addDetector(detector);
				zebraTriggerSource->addDetectorManager(detector);
			}
		}
	}

	// Identify data sources for the scaler channels and add them to the exporter option.
	AMDataSource *i0DetectorSource = detectorDataSource(BioXASBeamline::bioXAS()->i0Detector());
	if (i0DetectorSource && genericExporterOption)
		genericExporterOption->addDataSource(i0DetectorSource->name(), false);

	AMDataSource *i1DetectorSource = detectorDataSource(BioXASBeamline::bioXAS()->i1Detector());
	if (i1DetectorSource && genericExporterOption)
		genericExporterOption->addDataSource(i1DetectorSource->name(), true);

	AMDataSource *i2DetectorSource = detectorDataSource(BioXASBeamline::bioXAS()->i2Detector());
	if (i2DetectorSource && genericExporterOption)
		genericExporterOption->addDataSource(i2DetectorSource->name(), true);

	// Save changes to the exporter option.
	if (genericExporterOption)
		genericExporterOption->storeToDb(AMDatabase::database("user"));
}
