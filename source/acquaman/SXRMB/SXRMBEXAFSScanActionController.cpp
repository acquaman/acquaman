#include "SXRMBEXAFSScanActionController.h"

#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "beamline/SXRMB/SXRMBBeamline.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "analysis/AM1DNormalizationAB.h"

SXRMBEXAFSScanActionController::SXRMBEXAFSScanActionController(SXRMBEXAFSScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(configuration);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	AMControlInfoList list;
	list.append(SXRMBBeamline::sxrmb()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet sxrmbDetectors;
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->i0Detector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->teyDetector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->energyFeedbackDetector()->toInfo());
	sxrmbDetectors.addDetectorInfo(SXRMBBeamline::sxrmb()->brukerDetector()->toInfo());
	configuration_->setDetectorConfigurations(sxrmbDetectors);
}

SXRMBEXAFSScanActionController::~SXRMBEXAFSScanActionController()
{
}

AMAction3* SXRMBEXAFSScanActionController::createInitializationActions()
{
	return 0;
}

AMAction3* SXRMBEXAFSScanActionController::createCleanupActions()
{
	return 0;
}

void SXRMBEXAFSScanActionController::cancelImplementation()
{

}

void SXRMBEXAFSScanActionController::onInitializationActionsListSucceeded()
{

}

void SXRMBEXAFSScanActionController::buildScanControllerImplementation()
{
	AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

	detector->removeAllRegionsOfInterest();

	QList<AMDataSource *> i0Sources = QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("I0Detector"));

	AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

	QString edgeSymbol = configuration_->edge().split(" ").first();

	foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

		AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
		AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
		newRegion->setBinningRange(regionAB->binningRange());
		newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
		scan_->addAnalyzedDataSource(newRegion, false, true);
		detector->addRegionOfInterest(region);

		AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
		normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
		normalizedRegion->setDataName(newRegion->name());
		normalizedRegion->setNormalizationName(i0Sources.first()->name());
		scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
	}
}

void SXRMBEXAFSScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}
