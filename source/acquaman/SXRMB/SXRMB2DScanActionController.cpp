#include "SXRMB2DScanActionController.h"

#include "actions3/AMListAction3.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/SXRMB/SXRMBBeamline.h"
#include "dataman/AM2DScan.h"
#include "analysis/AM2DNormalizationAB.h"

SXRMB2DScanActionController::SXRMB2DScanActionController(SXRMB2DMapScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AM2DScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	int yPoints = int(round((double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()))/double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()))) + 1;

	AMControlInfoList list;
	list.append(SXRMBBeamline::sxrmb()->microprobeSampleStageX()->toInfo());
	list.append(SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->toInfo());
	scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
	scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));

	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet detectors;

	detectors.addDetectorInfo(SXRMBBeamline::sxrmb()->exposedDetectorByName("Bruker")->toInfo());

	configuration_->setDetectorConfigurations(detectors);
}

void SXRMB2DScanActionController::buildScanControllerImplementation()
{
	AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

	detector->removeAllRegionsOfInterest();

//	QList<AMDataSource *> i0Sources = QList<AMDataSource *>()
//			<< scan_->dataSourceAt(scan_->indexOfDataSource("SplitIonChamber"))
//			   << scan_->dataSourceAt(scan_->indexOfDataSource("PreKBIonChamber"))
//				  << scan_->dataSourceAt(scan_->indexOfDataSource("MiniIonChamber"));

//	AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

//	foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

//		AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
//		AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
//		newRegion->setBinningRange(regionAB->binningRange());
//		newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
//		scan_->addAnalyzedDataSource(newRegion, false, true);
//		detector->addRegionOfInterest(region);

//		AM2DNormalizationAB *normalizedRegion = new AM2DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
//		normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
//		normalizedRegion->setDataName(newRegion->name());
//		normalizedRegion->setNormalizationName(i0Sources.at(int(configuration_->incomingChoice()))->name());
//		scan_->addAnalyzedDataSource(normalizedRegion, true, false);
//	}
}

void SXRMB2DScanActionController::createAxisOrderMap()
{
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
}

AMAction3* SXRMB2DScanActionController::createInitializationActions()
{
	return 0;
}

AMAction3* SXRMB2DScanActionController::createCleanupActions()
{
	AMDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();
	detector->setHiddenFromUsers(false);
	detector->setIsVisible(true);

	return 0;
}
