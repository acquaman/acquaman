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

#include "util/AMErrorMonitor.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent), BioXASScanController()
{
	useFeedback_ = true;
	setScan(scan_);

	bioXASConfiguration_ = configuration;

	// Setup exporter option.
	if (bioXASConfiguration_) {

		AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), true);

		if (bioXASDefaultXAS->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
	}

	// Add the Ge detectors spectra, if a Ge detector is being used.

	AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	if (geDetectors) {

		for (int i = 0, detectorsCount = geDetectors->count(); i < detectorsCount; i++) {
			BioXAS32ElementGeDetector *geDetector = qobject_cast<BioXAS32ElementGeDetector*>(geDetectors->at(i));

			if (geDetector && bioXASConfiguration_->usingDetector(geDetector->name())) {

				// Add spectra.

				AMDetectorSet *elements = BioXASBeamline::bioXAS()->elementsForDetector(geDetector);

				if (elements) {
					for (int j = 0, elementsCount = elements->count(); j < elementsCount; j++) {
						AMDetector *element = elements->at(j);

						if (element && element->isConnected())
							bioXASConfiguration_->addDetector(element->toInfo());
					}
				}

				// Add ICRs, according to the preference set in the scan configuration.

				AMDetectorSet *icrDetectors = BioXASBeamline::bioXAS()->icrsForDetector(geDetector);

				if (icrDetectors && bioXASConfiguration_->canCollectICRs() && bioXASConfiguration_->collectICRsPreference()) {

					for (int j = 0, icrsCount = icrDetectors->count(); j < icrsCount; j++) {
						AMDetector *icrDetector = icrDetectors->at(j);

						if (icrDetector && icrDetector->isConnected())
							bioXASConfiguration_->addDetector(icrDetector->toInfo());
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
	BioXASBeamline *bioxas = BioXASBeamline::bioXAS();

	// Identify current beamline settings.
	scan_->setScanInitialConditions(bioxas->defaultXASScanControlInfos());

	// Identify exporter option.
	AMExporterOptionXDIFormat *exportXDI = 0;
	if (bioXASConfiguration_) {

		exportXDI = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), true);

		if (exportXDI->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(exportXDI->id());

		// Clear the option of any previous data sources.

		exportXDI->clearDataSources();
	}

	// Identify and setup the zebra trigger source.
	AMZebraDetectorTriggerSource *zebraTriggerSource = bioxas->zebraTriggerSource();
	if (zebraTriggerSource) {
		zebraTriggerSource->removeAllDetectors();
		zebraTriggerSource->removeAllDetectorManagers();

		if (usingDetector(bioxas->i0Detector()))
			zebraTriggerSource->addDetector(bioxas->i0Detector());

		if (usingDetector(bioxas->i1Detector()))
			zebraTriggerSource->addDetector(bioxas->i1Detector());

		if (usingDetector(bioxas->i2Detector()))
			zebraTriggerSource->addDetector(bioxas->i2Detector());

		if (usingScaler())
			zebraTriggerSource->addDetectorManager(bioxas->scaler());

		AMDetectorSet *geDetectors = bioxas->ge32ElementDetectors();

		for (int i = 0, count = geDetectors->count(); i < count; i++) {
			AMDetector *detector = geDetectors->at(i);

			if (usingDetector(detector)) {
				zebraTriggerSource->addDetector(geDetectors->at(i));
				zebraTriggerSource->addDetectorManager(geDetectors->at(i));
			}
		}
	}

	// Identify data sources for the scaler channels and the scaler dwell time.
	AMDataSource *i0DetectorSource = detectorDataSource(bioxas->i0Detector());
	AMDataSource *i1DetectorSource = detectorDataSource(bioxas->i1Detector());
	AMDataSource *i2DetectorSource = detectorDataSource(bioxas->i2Detector());
	AMDataSource *diodeDetectorSource = detectorDataSource(bioxas->diodeDetector());
	AMDataSource *pipsDetectorSource = detectorDataSource(bioxas->pipsDetector());
	AMDataSource *lytleDetectorSource = detectorDataSource(bioxas->lytleDetector());
	AMDataSource *dwellTimeSource = detectorDataSource(bioxas->scalerDwellTimeDetector());

	// Create analyzed data source for the absorbance.

	AM1DExpressionAB *absorbanceSource = 0;

	if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
		absorbanceSource = new AM1DExpressionAB("Absorbance");
		absorbanceSource->setDescription("Absorbance");
		absorbanceSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << i1DetectorSource << i2DetectorSource);
		absorbanceSource->setExpression(QString("ln(%1/%2)").arg(i1DetectorSource->name(), i2DetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance.

	AM1DDerivativeAB *derivAbsorbanceSource = 0;

	if (absorbanceSource) {
		derivAbsorbanceSource = new AM1DDerivativeAB("DerivAbsorbance");
		derivAbsorbanceSource->setInputDataSources(QList<AMDataSource*>() << absorbanceSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceSource, true, false);
	}

	// Create analyzed data sources for the dark current corrected scaler channel detectors.

	AM1DDarkCurrentCorrectionAB *i0CorrectedDetectorSource = 0;

	if (dwellTimeSource && i0DetectorSource && exportXDI) {
		i0CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDataName(i0DetectorSource->name());
		i0CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i0CorrectedDetectorSource->setDarkCurrent(bioxas->exposedDetectorByName(i0DetectorSource->name())->darkCurrentValue());
		i0CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << dwellTimeSource);
		i0CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( bioxas->i0Detector(), SIGNAL(darkCurrentValueChanged(double)), i0CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);

		exportXDI->addDataSource(i0CorrectedDetectorSource->name(), false);
	}

	AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;

	if (dwellTimeSource && i1DetectorSource && exportXDI) {
		i1CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDataName(i1DetectorSource->name());
		i1CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i1CorrectedDetectorSource->setDarkCurrent(bioxas->exposedDetectorByName(i1DetectorSource->name())->darkCurrentValue());
		i1CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i1DetectorSource << dwellTimeSource);
		i1CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( bioxas->i1Detector(), SIGNAL(darkCurrentValueChanged(double)), i1CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);

		exportXDI->addDataSource(i1CorrectedDetectorSource->name(), true);
	}

	AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

	if (dwellTimeSource && i2DetectorSource && exportXDI) {
		i2CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDataName(i2DetectorSource->name());
		i2CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i2CorrectedDetectorSource->setDarkCurrent(bioxas->exposedDetectorByName(i2DetectorSource->name())->darkCurrentValue());
		i2CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i2DetectorSource << dwellTimeSource);
		i2CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( bioxas->i2Detector(), SIGNAL(darkCurrentValueChanged(double)), i2CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);

		exportXDI->addDataSource(i2CorrectedDetectorSource->name(), true);
	}

	AM1DDarkCurrentCorrectionAB *diodeCorrectedDetectorSource = 0;

	if (dwellTimeSource && diodeDetectorSource && exportXDI) {
		diodeCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(diodeDetectorSource->name()));
		diodeCorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(diodeDetectorSource->name()));
		diodeCorrectedDetectorSource->setDataName(diodeDetectorSource->name());
		diodeCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		diodeCorrectedDetectorSource->setDarkCurrent(bioxas->exposedDetectorByName(diodeDetectorSource->name())->darkCurrentValue());
		diodeCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << diodeDetectorSource << dwellTimeSource);
		diodeCorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( bioxas->diodeDetector(), SIGNAL(darkCurrentValueChanged(double)), diodeCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(diodeCorrectedDetectorSource, true, false);

		exportXDI->addDataSource(diodeCorrectedDetectorSource->name(), true);
	}

	AM1DDarkCurrentCorrectionAB *pipsCorrectedDetectorSource = 0;

	if (dwellTimeSource && pipsDetectorSource && exportXDI) {
		pipsCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(pipsDetectorSource->name()));
		pipsCorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(pipsDetectorSource->name()));
		pipsCorrectedDetectorSource->setDataName(pipsDetectorSource->name());
		pipsCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		pipsCorrectedDetectorSource->setDarkCurrent(bioxas->exposedDetectorByName(pipsDetectorSource->name())->darkCurrentValue());
		pipsCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << pipsDetectorSource << dwellTimeSource);
		pipsCorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( bioxas->pipsDetector(), SIGNAL(darkCurrentValueChanged(double)), pipsCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(pipsCorrectedDetectorSource, true, false);

		exportXDI->addDataSource(pipsCorrectedDetectorSource->name(), true);
	}

	AM1DDarkCurrentCorrectionAB *lytleCorrectedDetectorSource = 0;

	if (dwellTimeSource && lytleDetectorSource && exportXDI) {
		lytleCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(lytleDetectorSource->name()));
		lytleCorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(lytleDetectorSource->name()));
		lytleCorrectedDetectorSource->setDataName(lytleDetectorSource->name());
		lytleCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		lytleCorrectedDetectorSource->setDarkCurrent(bioxas->exposedDetectorByName(lytleDetectorSource->name())->darkCurrentValue());
		lytleCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << lytleDetectorSource << dwellTimeSource);
		lytleCorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( bioxas->lytleDetector(), SIGNAL(darkCurrentValueChanged(double)), lytleCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(lytleCorrectedDetectorSource, true, false);

		exportXDI->addDataSource(lytleCorrectedDetectorSource->name(), true);
	}

	// Create analyzed data source for the absorbance, dark current corrected values.

	AM1DExpressionAB *absorbanceCorrectedSource = 0;

	if (i0CorrectedDetectorSource && i1CorrectedDetectorSource && i2CorrectedDetectorSource && exportXDI) {
		absorbanceCorrectedSource = new AM1DExpressionAB("Absorbance_DarkCorrect");
		absorbanceCorrectedSource->setDescription("Absorbance Dark Current Corrected");
		absorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << i0CorrectedDetectorSource << i1CorrectedDetectorSource << i2CorrectedDetectorSource);
		absorbanceCorrectedSource->setExpression(QString("ln(%1/%2)").arg(i1CorrectedDetectorSource->name(), i2CorrectedDetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceCorrectedSource, true, false);

		exportXDI->addDataSource(absorbanceCorrectedSource->name(), true);
	}

	// Create analyzed data source for the derivative of the absorbance, dark current corrected values.

	AM1DDerivativeAB *derivAbsorbanceCorrectedSource = 0;

	if (absorbanceCorrectedSource && exportXDI) {
		derivAbsorbanceCorrectedSource = new AM1DDerivativeAB("DerivAbsorbance_DarkCorrect");
		derivAbsorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << absorbanceCorrectedSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceCorrectedSource, true, false);

		exportXDI->addDataSource(derivAbsorbanceCorrectedSource->name(), true);
	}

	// Create analyzed data source for each Ge 32-el detector.

	AMDetectorSet *ge32Detectors = bioxas->ge32ElementDetectors();

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
			AMDetectorSet *elements = bioxas->elementsForDetector(ge32Detector);
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

						if (normalizedRegion && exportXDI) {
							scan_->addAnalyzedDataSource(normalizedRegion, false, false);
							exportXDI->addDataSource(normalizedRegion->name(), true);

							exportXDI->setExportPrecision(normalizedRegion->name(), 19);
							exportXDI->setExportPrecision(normalizationSource->name(), 19);
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

							if (canNormalize) {

								AMAnalysisBlock *normalizedRegion = createNormalizationAB(QString("norm_%1_%2").arg(newRegion->name()).arg(elements->at(i)->description()),
															  newRegion,
															  normalizationSource);

								if (normalizedRegion && exportXDI) {
									scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
									exportXDI->addDataSource(normalizedRegion->name(), true);

									exportXDI->setExportPrecision(normalizedRegion->name(), 19);
									exportXDI->setExportPrecision(normalizationSource->name(), 19);
								}
							}
						}
					}
				}
			}
		}
	}

	// Save changes to the exporter option.

	if (exportXDI)
		exportXDI->storeToDb(AMDatabase::database("user"));
}

AMAction3* BioXASXASScanActionController::createInitializationActions()
{
	AMAction3 *initializationAction = AMGenericStepScanController::createInitializationActions();

	AMListAction3 *initializationListAction = qobject_cast<AMListAction3 *> (initializationAction);
	if (initializationListAction) {
		initializationListAction->addSubAction(AMActionSupport::buildControlMoveAction(BioXASBeamline::bioXAS()->mono()->energy(), bioXASConfiguration_->energy()));
	}

	return initializationAction;
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
