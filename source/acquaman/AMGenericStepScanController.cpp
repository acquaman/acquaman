#include "AMGenericStepScanController.h"

#include "analysis/AMAdditionAB.h"
#include "analysis/AMNormalizationAB.h"
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
	return AMBeamline::bl()->createScanInitializationAction(configuration_);
}

AMAction3 * AMGenericStepScanController::createCleanupActions()
{
	return AMBeamline::bl()->createScanCleanupAction(configuration_);
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

		QList <AMRegionOfInterestAB *> newRegions;

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();

			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectrumSource);

			scan_->addAnalyzedDataSource(newRegion, true, false);
			newRegions << newRegion;
			// This is sufficient to add a region of interest on all detectors as they should by synchronized via AMBeamline::synchronizeXRFDetectors.
			detector->addRegionOfInterest(region);
		}

		if (configuration_->hasI0()){

			int index = scan_->indexOfDataSource(configuration_->i0().name());

			if (index != -1){

				AMDataSource *i0Source = scan_->dataSourceAt(index);
				QVector<int> sourceIndices = scan_->nonHiddenDataSourceIndexes();

				for (int i = 0, size = sourceIndices.size(); i < size; i++){

					AMDataSource *source = scan_->dataSourceAt(sourceIndices.at(i));

					if (source->name() != i0Source->name() && source->rank() == i0Source->rank()){

						AMNormalizationAB *normalizedSource = new AMNormalizationAB(QString("norm_%1").arg(source->name()));
						normalizedSource->setInputDataSources(QList<AMDataSource *>() << source << i0Source);
						normalizedSource->setDataName(source->name());
						normalizedSource->setNormalizationName(i0Source->name());
						scan_->addAnalyzedDataSource(normalizedSource, true, false);
					}
				}
			}
		}
	}
}
