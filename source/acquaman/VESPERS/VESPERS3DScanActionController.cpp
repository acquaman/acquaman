#include "VESPERS3DScanActionController.h"

#include "actions3/AMListAction3.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AM3DScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/VESPERS/VESPERSExporter2DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporterSMAK.h"
#include "analysis/AM3DNormalizationAB.h"
#include "analysis/AM3DAdditionAB.h"
#include "analysis/AMOrderReductionAB.h"

VESPERS3DScanActionController::VESPERS3DScanActionController(VESPERS3DScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AM3DScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();
	int zPoints = configuration_->scanAxisAt(2)->numberOfPoints();

	// 3D is very limited in motor selection choices.
	AMControlInfoList list;
	list.append(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->toInfo());
	list.append(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
	list.append(VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
	scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
	scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
	scan_->rawData()->addScanAxis(AMAxisInfo("Wire", zPoints, "Wire Position", "mm"));
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleHFeedback")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleVFeedback")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("WireVFeedback")->toInfo());

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MasterDwellTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RingCurrent")->toInfo());

	// We only use the single element for 3D mapping.
	AMDetector *detector = VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex");
	detector->setHiddenFromUsers(true);
	detector->setIsVisible(false);
	detectors.addDetectorInfo(detector->toInfo());

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexDeadTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRealTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexLiveTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexFastPeaks")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexSlowPeaks")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRawSpectrum")->toInfo());

	VESPERS::CCDDetectors ccdDetector = configuration_->ccdDetector();

	if (ccdDetector.testFlag(VESPERS::Roper))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RoperFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Mar))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MarFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Pilatus))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PilatusFileNumber")->toInfo());

	configuration_->setDetectorConfigurations(detectors);

	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));
}

void VESPERS3DScanActionController::buildScanControllerImplementation()
{
	// Due to dimensionality, all 3D/4D sources need to be hidden.
	for (int i = 0, size = scan_->dataSourceCount(); i < size; i++){

		AMDataSource *source = scan_->dataSourceAt(i);
		source->setVisibleInPlots(false);
		source->setHiddenFromUsers(true);
	}

	AMXRFDetector *detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex"));
	detector->removeAllRegionsOfInterest();

	QList<AMDataSource *> i0Sources = QList<AMDataSource *>()
			<< scan_->dataSourceAt(scan_->indexOfDataSource("SplitIonChamber"))
			   << scan_->dataSourceAt(scan_->indexOfDataSource("PreKBIonChamber"))
				  << scan_->dataSourceAt(scan_->indexOfDataSource("MiniIonChamber"));

	AMDataSource *spectraSource	 = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));
	QList<AMDataSource *> normalizedRegions;

	foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

		AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
		AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
		newRegion->setBinningRange(regionAB->binningRange());
		newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
		scan_->addAnalyzedDataSource(newRegion, false, true);
		detector->addRegionOfInterest(region->createCopy());

		AM3DNormalizationAB *normalizedRegion = new AM3DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
		normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
		normalizedRegion->setDataName(newRegion->name());
		normalizedRegion->setNormalizationName(i0Sources.at(int(configuration_->incomingChoice()))->name());
		normalizedRegions << normalizedRegion;
		scan_->addAnalyzedDataSource(normalizedRegion, false, true);
	}

	for (int i = 0, size = i0Sources.size(); i < size; i++){

		AMDataSource *tempInput = i0Sources.at(i);
		AMOrderReductionAB *temp = new AMOrderReductionAB(QString("reduced_%1").arg(tempInput->name()));
		temp->setDescription("Reduced " % tempInput->description());
		temp->setSelectedName(tempInput->name());
		temp->setReducedAxis(2);
		temp->setInputDataSources(QList<AMDataSource *>() << tempInput);
		scan_->addAnalyzedDataSource(temp, true, false);
	}

	for (int i = 0, size = normalizedRegions.size(); i < size; i++){

		AMDataSource *normalizedSource = normalizedRegions.at(i);
		AMOrderReductionAB *reducedRegion = new AMOrderReductionAB(normalizedSource->name().replace("norm_", "reduced_"));
		reducedRegion->setSelectedName(normalizedSource->name());
		reducedRegion->setReducedAxis(2);
		reducedRegion->setInputDataSources(QList<AMDataSource *>() << normalizedSource);
		scan_->addAnalyzedDataSource(reducedRegion, true, false);
	}
}

void VESPERS3DScanActionController::createAxisOrderMap()
{
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(2).name, 2);

}

AMAction3* VESPERS3DScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *initializationActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Initialization actions", "Initialization actions"));
	initializationActions->addSubAction(buildBaseInitializationAction(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));
	initializationActions->addSubAction(buildCCDInitializationAction(configuration_->ccdDetector(),
									 configuration_->ccdFileName(),
									 scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString(" name = '%1'").arg(scan_->name()))+1));

	return initializationActions;
}

AMAction3* VESPERS3DScanActionController::createCleanupActions()
{
	AMDetector *detector = VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex");
	detector->setHiddenFromUsers(false);
	detector->setIsVisible(true);

	return buildCleanupAction();
}

void VESPERS3DScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
