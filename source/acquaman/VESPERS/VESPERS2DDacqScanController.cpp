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

}

void VESPERS2DDacqScanController::addExtraDatasources()
{

}

bool VESPERS2DDacqScanController::initializeImplementation()
{
	// To initialize the 2D scan, there are two stages.
	/*
		First: Enable/Disable all the pertinent detectors.  The scalar is ALWAYS enabled.
		Second: Set the mode to single shot,set the time on the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *initializationActionsList = new AMBeamlineParallelActionsList;

	if (!initializationActions_)
		onInitializationActionFinished();

	initializationActions_ = new AMBeamlineListAction(initializationActionsList);

	// First stage.
	initializationActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Scalar
	initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(0)->createEnableAction(true));
	// Single element vortex
	if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement)
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(true));
	else
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(1)->createEnableAction(false));
	// CCD
	if (config_->usingCCD())
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(true));
	else
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(2)->createEnableAction(false));
	// Picoammeters
	initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(3)->createEnableAction(false));
	// Four element vortex
	if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement)
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(true));
	else
		initializationActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->elementAt(4)->createEnableAction(false));

	// Second stage.
	initializationActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createScansPerBufferAction(1));
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->scaler()->createTotalScansAction(1));
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::SingleShot));
	initializationActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(config_->timeStep()));

	// Integrity check.  Make sure no actions are null.
	for (int i = 0; i < initializationActionsList->stageCount(); i++){

		for (int j = 0; j < initializationActionsList->stage(i)->size(); j++){

			if (initializationActionsList->action(i, j) == 0){

				onInitializationActionsFailed(0);
				return false;
			}
		}
	}

	connect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
	connect(initializationActions_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
	connect(initializationActions_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
	initializationActions_->start();

	return true;
}

bool VESPERS2DDacqScanController::startImplementation()
{
	bool configSuccess = false;

	if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::SingleElement)
		configSuccess = setupSingleElementMap();

	else if (config_->fluorescenceDetectorChoice() == VESPERS2DScanConfiguration::FourElement)
		configSuccess = setupFourElementMap();

	if (!configSuccess){

		AMErrorMon::alert(this, VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE, "Could not recognize the configuration file.");
		return false;
	}

	return AM2DDacqScanController::startImplementation();
}

void VESPERS2DDacqScanController::cleanup()
{
	// To cleanup the 2D scan, there are two stages.
	/*
		First: Set the dwell time to 1 second.  Disables the variable integration time.  Make sure the CCD is no longer armed if it was used.
		Second: Set the scan mode to continuous.  This starts the synchronized dwell time.
	 */
	AMBeamlineParallelActionsList *cleanupActionsList = new AMBeamlineParallelActionsList;

	if (!cleanupActions_)
		onCleanupActionFinished();

	cleanupActions_ = new AMBeamlineListAction(cleanupActionsList);

	// First stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	// Synchronized dwell time.
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->synchronizedDwellTime()->createMasterTimeAction(1.0));
	// Variable integration time.
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->variableIntegrationTime()->createModeAction(CLSVariableIntegrationTime::Disabled));
	// Energy.
	cleanupActionsList->appendAction(0, VESPERSBeamline::vespers()->roperCCD()->createStopAction());

	// Second stage.
	cleanupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	// Start the synchronized dwell time.
	cleanupActionsList->appendAction(1, VESPERSBeamline::vespers()->synchronizedDwellTime()->createModeAction(CLSSynchronizedDwellTime::Continuous));

	connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
	connect(cleanupActions_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
	cleanupActions_->start();
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

bool VESPERS2DDacqScanController::setupSingleElementMap()
{
	bool loadSuccess = false;

	if (!config_->usingCCD())
		loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/2D-1Elem.cfg"));
	else
		loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/2D-1Elem-CCD.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(2)){}
}

bool VESPERS2DDacqScanController::setupFourElementMap()
{
	bool loadSuccess = false;

	if (!config_->usingCCD())
		loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/2D-4Elem.cfg"));
	else
		loadSuccess = advAcq_->setConfigFile(getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/2D-4Elem-CCD.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(2)){}
}
