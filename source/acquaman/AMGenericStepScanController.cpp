#include "AMGenericStepScanController.h"

#include "analysis/AMAdditionAB.h"
#include "beamline/AMBeamline.h"

AMGenericStepScanController::AMGenericStepScanController(AMGenericStepScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	AMControlInfo axisControlInfo1 = configuration_->axisControlInfos().at(0);
	scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo1.name(), 0, axisControlInfo1.description()));

	if (configuration_->scanAxes().size() == 2){

		int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();
		AMControlInfo axisControlInfo2 = configuration_->axisControlInfos().at(1);
		scan_->rawData()->addScanAxis(AMAxisInfo(axisControlInfo2.name(), yPoints, axisControlInfo2.description()));
	}

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

AMGenericStepScanController::~AMGenericStepScanController()
{

}

void AMGenericStepScanController::createAxisOrderMap()
{
	if (scan_->scanRank() == 1){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 0);
	}

	else if (scan_->scanRank() == 2){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
	}
}

void AMGenericStepScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

AMAction3 * AMGenericStepScanController::createInitializationActions()
{
	return 0;
}

AMAction3 * AMGenericStepScanController::createCleanupActions()
{
	return 0;
}

void AMGenericStepScanController::buildScanControllerImplementation()
{
	QList<AMXRFDetector *> xrfDetectors;
	QList<AMDataSource *> spectrumSources;

	for (int i = 0, size = configuration_->detectorConfigurations().count(); i < size; i++){

		AMXRFDetector *xrfDetector = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(i)));

		if (xrfDetector){

			xrfDetectors << xrfDetector;
			spectrumSources << scan_->dataSourceAt(scan_->indexOfDataSource(xrfDetector->name()));
		}
	}

	if (!xrfDetectors.isEmpty()){

		// This is sufficient to clear all regions of interest on all detectors as they should by synchronized via AMBeamline::synchronizeXRFDetectors.
		AMXRFDetector *detector = xrfDetectors.first();
		detector->removeAllRegionsOfInterest();

		AMDataSource *spectrumSource = 0;

		if (xrfDetectors.size() > 1){

			AMAdditionAB *spectrumSum = new AMAdditionAB("SpectrumSum");
			spectrumSum->setInputDataSources(spectrumSources);
			scan_->addAnalyzedDataSource(spectrumSum, false, true);
			spectrumSource = spectrumSum;
		}

		else
			spectrumSource = spectrumSources.first();

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();

			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectrumSource);

			scan_->addAnalyzedDataSource(newRegion, true, false);
			// This is sufficient to add a region of interest on all detectors as they should by synchronized via AMBeamline::synchronizeXRFDetectors.
			detector->addRegionOfInterest(region);
		}

		if (configuration_->hasI0()){


		}
	}
}
