#include "BioXASXASScanActionController.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMDetectorWaitForAcquisitionStateAction.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AMNormalizationAB.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/BioXAS/BioXASSIS3820Scaler.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "util/AMErrorMonitor.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent)
{
	bioXASConfiguration_ = configuration;

	useFeedback_ = true;

	scan_->setNotes(BioXASBeamline::bioXAS()->scanNotes());

	// Load the exporter option from database.

	AMExporterOptionXDIFormat *exporterOptionXDI = 0;

	if (bioXASConfiguration_) {

		exporterOptionXDI = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), bioXASConfiguration_->canExportSpectra() && bioXASConfiguration_->exportSpectraPreference());

		if (exporterOptionXDI->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(exporterOptionXDI->id());
	}

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

BioXASXASScanActionController::~BioXASXASScanActionController()
{

}

void BioXASXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void BioXASXASScanActionController::buildScanControllerImplementation()
{
	// Load the exporter option from database.

	AMExporterOptionXDIFormat *exporterOptionXDI = 0;

	if (bioXASConfiguration_) {

		exporterOptionXDI = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), bioXASConfiguration_->canExportSpectra() && bioXASConfiguration_->exportSpectraPreference());

		if (exporterOptionXDI->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(exporterOptionXDI->id());
	}

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

			if (BioXASBeamlineSupport::usingDetector(scan_, detector)) {
				zebraTriggerSource->addDetector(geDetectors->at(i));
				zebraTriggerSource->addDetectorManager(geDetectors->at(i));
			}
		}
	}

	// Identify data sources for the scaler channels and the scaler dwell time.

	AMDataSource *i0DetectorSource = BioXASBeamlineSupport::i0DetectorSource(scan_);
	AMDataSource *i1DetectorSource = BioXASBeamlineSupport::i1DetectorSource(scan_);
	AMDataSource *i2DetectorSource = BioXASBeamlineSupport::i2DetectorSource(scan_);
	AMDataSource *diodeDetectorSource = BioXASBeamlineSupport::diodeDetectorSource(scan_);
	AMDataSource *pipsDetectorSource = BioXASBeamlineSupport::pipsDetectorSource(scan_);
	AMDataSource *lytleDetectorSource = BioXASBeamlineSupport::lytleDetectorSource(scan_);

	AMDataSource *dwellTimeSource = BioXASBeamlineSupport::scalerDwellTimeDetectorSource(scan_);

	// Create analyzed data source for the raw absorbance.

	AM1DExpressionAB *absorbanceSource = 0;

	if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
		absorbanceSource = new AM1DExpressionAB("Absorbance_Raw");
		absorbanceSource->setDescription("Raw Absorbance");
		absorbanceSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << i1DetectorSource << i2DetectorSource);
		absorbanceSource->setExpression(QString("ln(%1/%2)").arg(i1DetectorSource->name(), i2DetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceSource, true, false);
	}

	// Create analyzed data source for the derivative of the raw absorbance.

	AM1DDerivativeAB *derivAbsorbanceSource = 0;

	if (absorbanceSource) {
		derivAbsorbanceSource = new AM1DDerivativeAB("DerivAbsorbance_Raw");
		derivAbsorbanceSource->setDescription("Derivative of Raw Absorbance");
		derivAbsorbanceSource->setInputDataSources(QList<AMDataSource*>() << absorbanceSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceSource, true, false);
	}

	// Create analyzed data sources for the dark current corrected I0.

	AM1DDarkCurrentCorrectionAB *i0CorrectedDetectorSource = 0;

	if (dwellTimeSource && i0DetectorSource) {
		i0CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDataName(i0DetectorSource->name());
		i0CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i0CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i0DetectorSource->name())->darkCurrentValue());
		i0CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << dwellTimeSource);
		i0CorrectedDetectorSource->setTimeUnitMultiplier(0.001);
		i0CorrectedDetectorSource->setDescription(QString("%1").arg(i0DetectorSource->name()));

		connect( BioXASBeamline::bioXAS()->i0Detector(), SIGNAL(darkCurrentValueChanged(double)), i0CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);

		// We have a corrected I0 data source, so no need to export the raw I0 data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(i0DetectorSource));
	}

	// Create analyzed data sources for the dark current corrected I1.

	AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;

	if (dwellTimeSource && i1DetectorSource) {
		i1CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDataName(i1DetectorSource->name());
		i1CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i1CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i1DetectorSource->name())->darkCurrentValue());
		i1CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i1DetectorSource << dwellTimeSource);
		i1CorrectedDetectorSource->setTimeUnitMultiplier(0.001);
		i1CorrectedDetectorSource->setDescription(QString("%1").arg(i1DetectorSource->name()));

		connect( BioXASBeamline::bioXAS()->i1Detector(), SIGNAL(darkCurrentValueChanged(double)), i1CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);

		// We have a corrected I1 data source, so no need to export the raw I1 data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(i1DetectorSource));
	}

	// Create analyzed data sources for the dark current corrected I2.

	AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

	if (dwellTimeSource && i2DetectorSource) {
		i2CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDataName(i2DetectorSource->name());
		i2CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i2CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i2DetectorSource->name())->darkCurrentValue());
		i2CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i2DetectorSource << dwellTimeSource);
		i2CorrectedDetectorSource->setTimeUnitMultiplier(0.001);
		i2CorrectedDetectorSource->setDescription(QString("%1").arg(i2DetectorSource->name()));

		connect( BioXASBeamline::bioXAS()->i2Detector(), SIGNAL(darkCurrentValueChanged(double)), i2CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);

		// We have a corrected I2 data source, so no need to export the raw I2 data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(i2DetectorSource));
	}

	// Create analyzed data sources for the dark current corrected diode.

	AM1DDarkCurrentCorrectionAB *diodeCorrectedDetectorSource = 0;

	if (dwellTimeSource && diodeDetectorSource) {
		diodeCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1").arg(diodeDetectorSource->name()));
		diodeCorrectedDetectorSource->setDataName(diodeDetectorSource->name());
		diodeCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		diodeCorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(diodeDetectorSource->name())->darkCurrentValue());
		diodeCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << diodeDetectorSource << dwellTimeSource);
		diodeCorrectedDetectorSource->setTimeUnitMultiplier(0.001);
		diodeCorrectedDetectorSource->setDescription(QString("%1").arg(diodeDetectorSource->name()));

		connect( BioXASBeamline::bioXAS()->diodeDetector(), SIGNAL(darkCurrentValueChanged(double)), diodeCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(diodeCorrectedDetectorSource, true, false);

		// We have a corrected diode data source, so no need to export the raw diode data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(diodeDetectorSource));
	}

	// Create analyzed data sources for the dark current corrected PIPS.

	AM1DDarkCurrentCorrectionAB *pipsCorrectedDetectorSource = 0;

	if (dwellTimeSource && pipsDetectorSource) {
		pipsCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1").arg(pipsDetectorSource->name()));
		pipsCorrectedDetectorSource->setDataName(pipsDetectorSource->name());
		pipsCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		pipsCorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(pipsDetectorSource->name())->darkCurrentValue());
		pipsCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << pipsDetectorSource << dwellTimeSource);
		pipsCorrectedDetectorSource->setTimeUnitMultiplier(0.001);
		pipsCorrectedDetectorSource->setDescription(QString("%1").arg(pipsDetectorSource->name()));

		connect( BioXASBeamline::bioXAS()->pipsDetector(), SIGNAL(darkCurrentValueChanged(double)), pipsCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(pipsCorrectedDetectorSource, true, false);

		// We have a corrected PIPS data source, so no need to export the raw PIPS data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(pipsDetectorSource));
	}

	// Create analyzed data sources for the dark current corrected Lytle.

	AM1DDarkCurrentCorrectionAB *lytleCorrectedDetectorSource = 0;

	if (dwellTimeSource && lytleDetectorSource) {
		lytleCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1").arg(lytleDetectorSource->name()));
		lytleCorrectedDetectorSource->setDataName(lytleDetectorSource->name());
		lytleCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		lytleCorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(lytleDetectorSource->name())->darkCurrentValue());
		lytleCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << lytleDetectorSource << dwellTimeSource);
		lytleCorrectedDetectorSource->setTimeUnitMultiplier(0.001);
		lytleCorrectedDetectorSource->setDescription(QString("%1").arg(lytleDetectorSource->name()));

		connect( BioXASBeamline::bioXAS()->lytleDetector(), SIGNAL(darkCurrentValueChanged(double)), lytleCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(lytleCorrectedDetectorSource, true, false);

		// We have a corrected Lytle data source, so no need to export the raw Lytle data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(lytleDetectorSource));
	}

	// Create analyzed data source for the dark current corrected absorbance.

	AM1DExpressionAB *absorbanceCorrectedSource = 0;

	if (i0CorrectedDetectorSource && i1CorrectedDetectorSource && i2CorrectedDetectorSource) {
		absorbanceCorrectedSource = new AM1DExpressionAB("Absorbance");
		absorbanceCorrectedSource->setDescription("Absorbance");
		absorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << i0CorrectedDetectorSource << i1CorrectedDetectorSource << i2CorrectedDetectorSource);
		absorbanceCorrectedSource->setExpression(QString("ln(%1/%2)").arg(i1CorrectedDetectorSource->name(), i2CorrectedDetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceCorrectedSource, true, false);

		// We have a corrected absorbance data source, so no need to export the raw absorbance data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(absorbanceSource));
	}

	// Create analyzed data source for the derivative of the dark current corrected absorbance.

	AM1DDerivativeAB *derivAbsorbanceCorrectedSource = 0;

	if (absorbanceCorrectedSource) {
		derivAbsorbanceCorrectedSource = new AM1DDerivativeAB("DerivAbsorbance");
		derivAbsorbanceCorrectedSource->setDescription("Derivative of Absorbance");
		derivAbsorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << absorbanceCorrectedSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceCorrectedSource, true, false);

		// We have a corrected deriv absorbance data source, so no need to export the raw deriv absorbance data source by default.

//		if (exporterOptionXDI)
//			exporterOptionXDI->removeDataSourceAt(scan_->indexOfDataSource(derivAbsorbanceSource));
	}

	// Create analyzed data source for each Ge 32-el detector.

	AMDetectorSet *ge32Detectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	for (int i = 0, count = ge32Detectors->count(); i < count; i++) {

		BioXAS32ElementGeDetector *ge32Detector = qobject_cast<BioXAS32ElementGeDetector*>(ge32Detectors->at(i));

		if (BioXASBeamlineSupport::usingGeDetector(scan_, ge32Detector)) {

			// Clear any previous regions.

			ge32Detector->removeAllRegionsOfInterest();

			// Iterate through each region in the configuration.
			// Create analysis block for each region, add ge32Detector spectra source as input source for each.
			// Add analysis block to the scan and to the ge32Detector.
			// Create normalized analysis block for each region, add to scan.

			AMDataSource *spectraSource = BioXASBeamlineSupport::geDetectorSource(scan_, ge32Detector);
			AMDetectorSet *elements = BioXASBeamline::bioXAS()->elementsForDetector(ge32Detector);
			QString edgeSymbol = bioXASConfiguration_->edge().split(" ").first();
			bool canNormalize = (i0DetectorSource || i0CorrectedDetectorSource);
			AMDataSource *normalizationSource = 0;

			if (canNormalize)
				normalizationSource = (i0CorrectedDetectorSource != 0) ? i0CorrectedDetectorSource : i0DetectorSource;

			foreach (AMRegionOfInterest *region, bioXASConfiguration_->regionsOfInterest()){

				ge32Detector->addRegionOfInterest(region);

				AMAnalysisBlock *newRegion = createRegionOfInterestAB(region->name().remove(' '), region, spectraSource);

				if (newRegion){

					scan_->addAnalyzedDataSource(newRegion, false, true);

					if (canNormalize) {

						AMAnalysisBlock *normalizedRegion = createNormalizationAB(QString("norm_%1").arg(newRegion->name()), newRegion, normalizationSource);

						if (normalizedRegion)
							scan_->addAnalyzedDataSource(normalizedRegion, false, false);
					}
				}

				if (elements){

					for (int i = 0, size = elements->count(); i < size; i++){

						newRegion = createRegionOfInterestAB(QString("%1_%2").arg(region->name().remove(' ')).arg(elements->at(i)->description()),
										     region,
										     scan_->dataSourceAt(scan_->indexOfDataSource(elements->at(i)->name())));

						if (newRegion){

							scan_->addAnalyzedDataSource(newRegion, false, true);

							if (canNormalize) {

								AMAnalysisBlock *normalizedRegion = createNormalizationAB(QString("norm_%1_%2").arg(newRegion->name()).arg(elements->at(i)->description()),
															  newRegion,
															  normalizationSource);

								if (normalizedRegion)
									scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
							}
						}
					}
				}
			}
		}
	}
}

AMAnalysisBlock *BioXASXASScanActionController::createRegionOfInterestAB(const QString &name, AMRegionOfInterest *region, AMDataSource *spectrumSource) const
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

AMAnalysisBlock *BioXASXASScanActionController::createNormalizationAB(const QString &name, AMDataSource *source, AMDataSource *normalizer) const
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
