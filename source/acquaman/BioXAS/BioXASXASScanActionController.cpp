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
	// Identify and setup the zebra trigger source.

	AMZebraDetectorTriggerSource *zebraTriggerSource = BioXASBeamline::bioXAS()->zebraTriggerSource();

	if (zebraTriggerSource) {
		zebraTriggerSource->removeAllDetectors();
		zebraTriggerSource->removeAllDetectorManagers();

		CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

		if (BioXASBeamlineSupport::usingScaler(scan_)) {
			qDebug() << "BioXASXASScanActionController: the scan is using the scaler. Adding scaler to Zebra.";
			zebraTriggerSource->addDetectorManager(scaler);
		}

		AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();

		for (int i = 0, count = geDetectors->count(); i < count; i++) {
			AMDetector *detector = geDetectors->at(i);

			if (BioXASBeamlineSupport::usingDetector(scan_, detector)) {
				zebraTriggerSource->addDetector(geDetectors->at(i));
				zebraTriggerSource->addDetector(geDetectors->at(i));
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

	// Create analyzed data source for the absorbance.

	AM1DExpressionAB *absorbanceSource = 0;

	if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
		qDebug() << "BioXASXASScanActionController: the scan is using I0, I1, and I2. Creating absorbance source.";
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

	if (dwellTimeSource && i0DetectorSource) {
		i0CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDataName(i0DetectorSource->name());
		i0CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i0CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i0DetectorSource->name())->darkCurrentValue());
		i0CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << dwellTimeSource);
		i0CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASBeamline::bioXAS()->i0Detector(), SIGNAL(darkCurrentValueChanged(double)), i0CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);
	}

	AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;

	if (dwellTimeSource && i1DetectorSource) {
		i1CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDataName(i1DetectorSource->name());
		i1CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i1CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i1DetectorSource->name())->darkCurrentValue());
		i1CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i1DetectorSource << dwellTimeSource);
		i1CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASBeamline::bioXAS()->i1Detector(), SIGNAL(darkCurrentValueChanged(double)), i1CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);
	}

	AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

	if (dwellTimeSource && i2DetectorSource) {
		i2CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDataName(i2DetectorSource->name());
		i2CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i2CorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(i2DetectorSource->name())->darkCurrentValue());
		i2CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i2DetectorSource << dwellTimeSource);
		i2CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASBeamline::bioXAS()->i2Detector(), SIGNAL(darkCurrentValueChanged(double)), i2CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);
	}

	AM1DDarkCurrentCorrectionAB *diodeCorrectedDetectorSource = 0;

	if (dwellTimeSource && diodeDetectorSource) {
		diodeCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(diodeDetectorSource->name()));
		diodeCorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(diodeDetectorSource->name()));
		diodeCorrectedDetectorSource->setDataName(diodeDetectorSource->name());
		diodeCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		diodeCorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(diodeDetectorSource->name())->darkCurrentValue());
		diodeCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << diodeDetectorSource << dwellTimeSource);
		diodeCorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASBeamline::bioXAS()->diodeDetector(), SIGNAL(darkCurrentValueChanged(double)), diodeCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(diodeCorrectedDetectorSource, true, false);
	}

	AM1DDarkCurrentCorrectionAB *pipsCorrectedDetectorSource = 0;

	if (dwellTimeSource && pipsDetectorSource) {
		pipsCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(pipsDetectorSource->name()));
		pipsCorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(pipsDetectorSource->name()));
		pipsCorrectedDetectorSource->setDataName(pipsDetectorSource->name());
		pipsCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		pipsCorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(pipsDetectorSource->name())->darkCurrentValue());
		pipsCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << pipsDetectorSource << dwellTimeSource);
		pipsCorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASBeamline::bioXAS()->pipsDetector(), SIGNAL(darkCurrentValueChanged(double)), pipsCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(pipsCorrectedDetectorSource, true, false);
	}

	AM1DDarkCurrentCorrectionAB *lytleCorrectedDetectorSource = 0;

	if (dwellTimeSource && lytleDetectorSource) {
		lytleCorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(lytleDetectorSource->name()));
		lytleCorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(lytleDetectorSource->name()));
		lytleCorrectedDetectorSource->setDataName(lytleDetectorSource->name());
		lytleCorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		lytleCorrectedDetectorSource->setDarkCurrent(BioXASBeamline::bioXAS()->exposedDetectorByName(lytleDetectorSource->name())->darkCurrentValue());
		lytleCorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << lytleDetectorSource << dwellTimeSource);
		lytleCorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASBeamline::bioXAS()->lytleDetector(), SIGNAL(darkCurrentValueChanged(double)), lytleCorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(lytleCorrectedDetectorSource, true, false);
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
