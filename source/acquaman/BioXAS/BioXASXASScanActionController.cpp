#include "BioXASXASScanActionController.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMDetectorWaitForAcquisitionStateAction.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "analysis/AM1DNormalizationAB.h"

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

	if (bioXASConfiguration_) {
		AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), true);

		if (bioXASDefaultXAS->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
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
	// Identify the zebra trigger source.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if (zebra) {
		zebra->clearDetectors();
		zebra->clearDetectorManagers();
	}

	// Identify the scaler.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	// Identify data sources for the scaler channels.

	AMDataSource *i0DetectorSource = 0;
	AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();

	if (i0Detector) {

		int i0DetectorIndex = scan_->indexOfDataSource(i0Detector->name());

		if (i0DetectorIndex != -1) {

			if (zebra)
				zebra->addDetector(i0Detector);

			i0DetectorSource = scan_->dataSourceAt(i0DetectorIndex);
		}
	}

	AMDataSource *i1DetectorSource = 0;
	AMDetector *i1Detector = BioXASBeamline::bioXAS()->i1Detector();

	if (i1Detector) {

		int i1DetectorIndex = scan_->indexOfDataSource(i1Detector->name());

		if (i1DetectorIndex != -1) {

			if (zebra)
				zebra->addDetector(i1Detector);

			i1DetectorSource = scan_->dataSourceAt(i1DetectorIndex);
		}
	}

	AMDataSource *i2DetectorSource = 0;
	AMDetector *i2Detector = BioXASBeamline::bioXAS()->i2Detector();

	if (i2Detector) {

		int i2DetectorIndex = scan_->indexOfDataSource(i2Detector->name());

		if (i2DetectorIndex != -1) {

			if (zebra)
				zebra->addDetector(i2Detector);

			i2DetectorSource = scan_->dataSourceAt(i2DetectorIndex);
		}
	}

	if (scan_->indexOfDataSource(i0Detector->name()) != -1
			|| scan_->indexOfDataSource(i1Detector->name()) != -1
			|| scan_->indexOfDataSource(i2Detector->name()) != -1){

		if (scaler && zebra)
			zebra->addDetectorManager(scaler);
	}

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

	AMDataSource *dwellTimeSource = 0;
	AM1DDarkCurrentCorrectionAB *i0CorrectedDetectorSource = 0;
	AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;
	AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

	AMDetector *scalerDwellTimeDetector = BioXASBeamline::bioXAS()->scalerDwellTimeDetector();

	if (scalerDwellTimeDetector) {
		int dwellTimeIndex = scan_->indexOfDataSource(scalerDwellTimeDetector->name());
		if (dwellTimeIndex != -1) {
			dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);
		}

		if (dwellTimeSource && i0DetectorSource) {
			i0CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i0DetectorSource->name()));
			i0CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i0DetectorSource->name()));
			i0CorrectedDetectorSource->setDataName(i0DetectorSource->name());
			i0CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
			i0CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i0DetectorSource->name())->darkCurrentValue());
			i0CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << dwellTimeSource);
			i0CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

			connect( i0Detector, SIGNAL(darkCurrentValueChanged(double)), i0CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

			scan_->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);
		}

		if (dwellTimeSource && i1DetectorSource) {
			i1CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i1DetectorSource->name()));
			i1CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i1DetectorSource->name()));
			i1CorrectedDetectorSource->setDataName(i1DetectorSource->name());
			i1CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
			i1CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i1DetectorSource->name())->darkCurrentValue());
			i1CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i1DetectorSource << dwellTimeSource);
			i1CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

			connect( i1Detector, SIGNAL(darkCurrentValueChanged(double)), i1CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

			scan_->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);
		}

		if (dwellTimeSource && i2DetectorSource) {
			i2CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i2DetectorSource->name()));
			i2CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i2DetectorSource->name()));
			i2CorrectedDetectorSource->setDataName(i2DetectorSource->name());
			i2CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
			i2CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i2DetectorSource->name())->darkCurrentValue());
			i2CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i2DetectorSource << dwellTimeSource);
			i2CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

			connect( i2Detector, SIGNAL(darkCurrentValueChanged(double)), i2CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

			scan_->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);
		}
	}

	// Create analyzed data source for the absorbance, dark current corrected values.

	AM1DExpressionAB *absorbanceCorrectedSource = 0;

	if (i0CorrectedDetectorSource && i1CorrectedDetectorSource && i2CorrectedDetectorSource) {
		absorbanceCorrectedSource = new AM1DExpressionAB("Absorbance_DarkCorrect");
		absorbanceCorrectedSource->setDescription("Absorbance Dark Current Corrected");
		absorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << i0CorrectedDetectorSource << i1CorrectedDetectorSource << i2CorrectedDetectorSource);
		absorbanceCorrectedSource->setExpression(QString("ln(%1/%2)").arg(i1CorrectedDetectorSource->name(), i2CorrectedDetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceCorrectedSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance, dark current corrected values.

	AM1DDerivativeAB *derivAbsorbanceCorrectedSource = 0;

	if (absorbanceCorrectedSource) {
		derivAbsorbanceCorrectedSource = new AM1DDerivativeAB("DerivAbsorbance_DarkCorrect");
		derivAbsorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << absorbanceCorrectedSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceCorrectedSource, true, false);
	}

	// Create analyzed data source for each Ge 32-el detector.

	AMDetectorSet *ge32Detectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

	for (int i = 0; i < ge32Detectors->count(); i++) {

		AMXRFDetector *ge32Detector = qobject_cast<AMXRFDetector*>(ge32Detectors->at(i));

		if (ge32Detector) {

			int ge32DetectorIndex = scan_->indexOfDataSource(ge32Detector->name());

			if (ge32DetectorIndex != -1) {

				if (zebra) {
					zebra->addDetector(ge32Detector);
				}

				// Clear any previous regions.

				ge32Detector->removeAllRegionsOfInterest();

				// Iterate through each region in the configuration.
				// Create analysis block for each region, add ge32Detector spectra source as input source for each.
				// Add analysis block to the scan and to the ge32Detector.
				// Create normalized analysis block for each region, add to scan.

				AMDataSource *spectraSource = scan_->dataSourceAt(ge32DetectorIndex);
				QString edgeSymbol = bioXASConfiguration_->edge().split(" ").first();
				bool canNormalize = (i0DetectorSource || i0CorrectedDetectorSource);

				foreach (AMRegionOfInterest *region, bioXASConfiguration_->regionsOfInterest()){

					AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();

					AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
					newRegion->setBinningRange(regionAB->binningRange());
					newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);

					scan_->addAnalyzedDataSource(newRegion, false, true);
					ge32Detector->addRegionOfInterest(region);

					if (canNormalize) {
						AMDataSource *normalizationSource = (i0CorrectedDetectorSource != 0) ? i0CorrectedDetectorSource : i0DetectorSource;

						AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
						normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << normalizationSource);
						normalizedRegion->setDataName(newRegion->name());
						normalizedRegion->setNormalizationName(normalizationSource->name());

						scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
					}
				}
			}
		}
	}
}
