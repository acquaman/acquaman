#include "VESPERS2DDacqScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "actions/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"
#include "analysis/AM1DExpressionAB.h"
#include "actions/AMBeamlineParallelActionsList.h"

#include <QDir>

VESPERS2DDacqScanController::VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent)
	: AM2DDacqScanController(cfg, parent)
{
	config_ = cfg;

	xAxisCount_ = int(fabs((config_->xEnd()-config_->xStart())/config_->xStep())) + 1;
	yAxisCount_ = int(fabs((config_->yEnd()-config_->yStart())/config_->yStep())) + 1;

	AMPVwStatusControl *control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
	xAxisPVName_ = control != 0 ? control->writePVName() : "";
	control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStage()->vert());
	yAxisPVName_ = control != 0 ? control->writePVName() : "";

	initializationActions_ = 0;
	cleanupActions_ = 0;

	secondsElapsed_ = 0;
	secondsTotal_ = config_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

	scan_ = new AM2DScan(); 	// MB: Moved from line 363 in startImplementation.
	scan_->setName(config_->name());
	scan_->setScanConfiguration(config_);
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setIndexType("fileSystem");

	if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement && !config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF1El");

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement && config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF1ElXRD");

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement && !config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF4El");

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement && config_->usingCCD())
		scan_->setFileFormat("vespers2012XRF4ElXRD");

	else
		AMErrorMon::error(this, VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE, "Could not recognize the format type of the scan.");

	// Add all the raw data sources.
	////////////////////////////////////////////////

	// Add the feedback coordinates.

	// Add the ion chambers.

	// Add the regions of interest.

	// Add the rest.

	// Add analysis blocks.

}

void VESPERS2DDacqScanController::addExtraDatasources()
{

}

bool VESPERS2DDacqScanController::initializeImplementation()
{
	/// \todo Flesh out with the appropriate actions like in VESPERSEXAFSDacqScanController.
	setInitialized();
	return true;
}

bool VESPERS2DDacqScanController::startImplementation()
{
	/// \todo Fill in the start implementation in a way that's similar too VESPERSEXAFSDacqScanController.e
	return AM2DDacqScanController::startImplementation();
}

void VESPERS2DDacqScanController::cleanup()
{

}

void VESPERS2DDacqScanController::onCleanupFinished()
{
	AM2DDacqScanController::onDacqStop();
}

void VESPERS2DDacqScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

void VESPERS2DDacqScanController::onInitializationActionsSucceeded()
{
	setInitialized();
}

void VESPERS2DDacqScanController::onInitializationActionsFailed(int explanation)
{
	Q_UNUSED(explanation)

	AMErrorMon::alert(this, VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE, "2D scan failed to initialize.");
	onInitializationActionFinished();
	setFailed();
}

void VESPERS2DDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
}

QString VESPERS2DDacqScanController::getHomeDirectory()
{
	// Find out which path we are using for acquaman (depends on whether you are on Mac or Linux or beamline OPI).
	QString homeDir = QDir::homePath();
	if(QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if(QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	return homeDir;
}

void VESPERS2DDacqScanController::onInitializationActionFinished()
{
	if (initializationActions_ == 0)
		return;

	// Disconnect all signals and return all memory.
	initializationActions_->disconnect();
	AMBeamlineParallelActionsList *actionList = initializationActions_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	initializationActions_->deleteLater();
	initializationActions_ = 0;
}

void VESPERS2DDacqScanController::onCleanupActionFinished()
{
	if (cleanupActions_ == 0)
		return;

	// Disconnect all signals and return all memory.
	cleanupActions_->disconnect();
	AMBeamlineParallelActionsList *actionList = cleanupActions_->list()	;

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	cleanupActions_->deleteLater();
	cleanupActions_ = 0;
}
