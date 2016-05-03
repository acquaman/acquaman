#include "IDEAS2DScanActionController.h"

#include "application/IDEAS/IDEAS.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMWaitAction.h"
#include "application/AMAppControllerSupport.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "dataman/AM2DScan.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "analysis/AM2DNormalizationAB.h"
#include "dataman/export/AMExporter2DAscii.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporterOptionSMAK.h"



IDEAS2DScanActionController::IDEAS2DScanActionController(IDEAS2DScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AM2DScan();
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");

	// SMAK format requires a specific spectra file format.
	AMExporterOptionSMAK *exportOptions = IDEAS::buildSMAKExporterOption("IDEAS2DDefault", true, true);

    if(exportOptions->id() > 0)
		AMAppControllerSupport::registerClass<IDEAS2DScanConfiguration, AMSMAKExporter, AMExporterOptionSMAK>(exportOptions->id());

    exportOptions->deleteLater();

	QString scanName;

	if(configuration_->userScanName() == ""){

		scanName = configuration_->autoScanName();
		scan_->setName(QString("%1").arg(scanName));
	}

	else{

		scanName = configuration_->userScanName();
		scan_->setName(QString("%1").arg(scanName));
	}

	int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();

	AMControlInfoList list;
	list.append(AMBeamline::bl()->exposedControlByName("Sample Platform Horizontal")->toInfo());
	list.append(AMBeamline::bl()->exposedControlByName("Sample Platform Vertical")->toInfo());
	configuration_->setAxisControlInfos(list);

	scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
	scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("I_0")->toInfo());
	detectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("Sample")->toInfo());

	if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ketek)) {

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByName("KETEK");
		detector->setIsVisible(false);
		detector->setHiddenFromUsers(true);
		detectors.addDetectorInfo(detector->toInfo());
	}

	else if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ge13Element) && IDEASBeamline::ideas()->ge13Element()->isConnected()){

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByName("13-el Ge");
		detector->setIsVisible(false);
		detector->setHiddenFromUsers(true);
		detectors.addDetectorInfo(detector->toInfo());
	}

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

IDEAS2DScanActionController::~IDEAS2DScanActionController()
{

}

void IDEAS2DScanActionController::buildScanControllerImplementation()
{
    QList<AMDataSource *> i0Sources = QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("I_0"));

    AM2DNormalizationAB *transmissionAB = new AM2DNormalizationAB("transmission");
    transmissionAB->setInputDataSources(i0Sources << scan_->rawDataSources()->at(scan_->rawDataSources()->indexOfKey("Sample")));
    transmissionAB->setDataName("I_0");
    transmissionAB->setNormalizationName("Sample");
    scan_->addAnalyzedDataSource(transmissionAB);

	AMXRFDetector *detector = IDEASBeamline::ideas()->xrfDetector(configuration_->fluorescenceDetector());
	if (detector){

		detector->removeAllRegionsOfInterest();

		AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			detector->addRegionOfInterest(region->createCopy());

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, true, false);

			AM2DNormalizationAB *normalizedRegion = new AM2DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0Sources.first()->name());
			scan_->addAnalyzedDataSource(normalizedRegion, true, false);
		}
	}
}

void IDEAS2DScanActionController::createAxisOrderMap()
{
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
}

AMAction3 * IDEAS2DScanActionController::createInitializationActions()
{
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("IDEAS 2D Initialization", "IDEAS 2D Initialization"), AMListAction3::Parallel);

	initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(IDEASBeamline::ideas()->monoEnergyControl(), configuration_->energy()));
	initializationActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(false));

	return initializationActions;
}

AMAction3 * IDEAS2DScanActionController::createCleanupActions()
{
    AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("IDEAS 2D Cleanup Actions", "IDEAS 2D Cleanup Actions"));

	cleanupActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(IDEASBeamline::ideas()->scaler()->dwellTime())));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createDwellTimeAction3(0.25));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(true));

	return cleanupActions;
}

void IDEAS2DScanActionController::onInitializationActionsListSucceeded()
{
	AMControlInfoList positions(IDEASBeamline::ideas()->exposedControls()->toInfoList());
	positions.remove(positions.indexOf("masterDwell"));
	positions.remove(positions.indexOf("DirectEnergy"));
	positions.remove(positions.indexOf("Energy"));
	positions.remove(positions.indexOf("XRF1E Real Time"));

	if(!configuration_->fluorescenceDetector().testFlag(IDEAS::NoXRF)){

		positions.remove(positions.indexOf("XRF1E Peaking Time"));
		positions.remove(positions.indexOf("XRF1E Trigger Level"));
		positions.remove(positions.indexOf("XRF1E Baseline Threshold"));
		positions.remove(positions.indexOf("XRF1E Preamp Gain"));
	}

	scan_->setScanInitialConditions(positions);
	AMScanActionController::onInitializationActionsListSucceeded();
}

void IDEAS2DScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
