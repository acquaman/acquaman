#include "BioXASGenericStepScanController.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "application/BioXAS/BioXAS.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "analysis/AMNormalizationAB.h"

BioXASGenericStepScanController::BioXASGenericStepScanController(BioXASGenericStepScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent), BioXASScanController()
{
	useFeedback_ = true;
	setScan(scan_);

	// Setup exporter option.

	if (configuration_) {

		AMExporterOptionXDIFormat *bioXASDefaultXDI = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", "", "", true);

		if (bioXASDefaultXDI->id() > 0)
			AMAppControllerSupport::registerClass<BioXASGenericStepScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXDI->id());
	}

	// Add the Ge detectors spectra, if a Ge detector is being used.

	AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	if (geDetectors) {

		for (int i = 0, detectorsCount = geDetectors->count(); i < detectorsCount; i++) {
			BioXAS32ElementGeDetector *geDetector = qobject_cast<BioXAS32ElementGeDetector*>(geDetectors->at(i));

			if (geDetector && configuration_->usingDetector(geDetector->name())) {

				// Add spectra.

				AMDetectorSet *elements = BioXASBeamline::bioXAS()->elementsForDetector(geDetector);

				if (elements) {
					for (int j = 0, elementsCount = elements->count(); j < elementsCount; j++) {
						AMDetector *element = elements->at(j);

						if (element && element->isConnected())
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

	// Create analyzed data source for each Ge 32-el detector.

	AMDetectorSet *ge32Detectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	for (int i = 0, count = ge32Detectors->count(); i < count; i++) {

		BioXAS32ElementGeDetector *ge32Detector = qobject_cast<BioXAS32ElementGeDetector*>(ge32Detectors->at(i));

		if (usingDetector(ge32Detector)) {

			// Clear any previous regions.

			ge32Detector->removeAllRegionsOfInterest();

			// Iterate through each region in the configuration.
			// Create analysis block for each region, add ge32Detector spectra source as input source for each.
			// Add analysis block to the scan and to the ge32Detector.
			// Create normalized analysis block for each region, add to scan.

			AMDataSource *spectraSource = detectorDataSource(ge32Detector);
			AMDetectorSet *elements = BioXASBeamline::bioXAS()->elementsForDetector(ge32Detector);
			AMDataSource *normalizationSource = i0DetectorSource;

			foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

				ge32Detector->addRegionOfInterest(region);

				AMAnalysisBlock *newRegion = createRegionOfInterestAB(region->name().remove(' '), region, spectraSource);

				if (newRegion){

					scan_->addAnalyzedDataSource(newRegion, false, true);

					if (normalizationSource) {

						AMAnalysisBlock *normalizedRegion = createNormalizationAB(QString("norm_%1").arg(newRegion->name()), newRegion, normalizationSource);

						if (normalizedRegion) {
							scan_->addAnalyzedDataSource(normalizedRegion, false, false);
							genericExporterOption->addDataSource(normalizedRegion->name(), true);

							genericExporterOption->setExportPrecision(normalizedRegion->name(), 19);
							genericExporterOption->setExportPrecision(normalizationSource->name(), 19);
						}
					}
				}

				if (elements){

					for (int i = 0, size = elements->count(); i < size; i++){

						newRegion = createRegionOfInterestAB(QString("%1_%2").arg(region->name().remove(' ')).arg(elements->at(i)->description()),
											 region,
											 scan_->dataSourceAt(scan_->indexOfDataSource(elements->at(i)->name())));

						if (newRegion){

							scan_->addAnalyzedDataSource(newRegion, false, true);

							if (normalizationSource) {

								AMAnalysisBlock *normalizedRegion = createNormalizationAB(QString("norm_%1_%2").arg(newRegion->name()).arg(elements->at(i)->description()),
															  newRegion,
															  normalizationSource);

								if (normalizedRegion) {
									scan_->addAnalyzedDataSource(normalizedRegion, true, false);
									genericExporterOption->addDataSource(normalizedRegion->name(), true);

									genericExporterOption->setExportPrecision(normalizedRegion->name(), 19);
									genericExporterOption->setExportPrecision(normalizationSource->name(), 19);
								}
							}
						}
					}
				}
			}
		}
	}

	// Save changes to the exporter option.
	if (genericExporterOption)
		genericExporterOption->storeToDb(AMDatabase::database("user"));
}

AMAnalysisBlock *BioXASGenericStepScanController::createRegionOfInterestAB(const QString &name, AMRegionOfInterest *region, AMDataSource *spectrumSource) const
{
	if (region && region->isValid() && spectrumSource && ((spectrumSource->rank()-scan_->scanRank()) == 1)){

		AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
		AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(name);
		newRegion->setBinningRange(regionAB->binningRange());
		newRegion->setInputDataSources(QList<AMDataSource *>() << spectrumSource);

		return newRegion;
	}

	return 0;
}

AMAnalysisBlock *BioXASGenericStepScanController::createNormalizationAB(const QString &name, AMDataSource *source, AMDataSource *normalizer) const
{
	if (source && normalizer && source->rank() == normalizer->rank()){

		AMNormalizationAB *normalizedSource = new AMNormalizationAB(name);
		normalizedSource->setInputDataSources(QList<AMDataSource *>() << source << normalizer);
		normalizedSource->setDataName(source->name());
		normalizedSource->setNormalizationName(normalizer->name());

		return normalizedSource;
	}

	return 0;
}

