#include "SGMFastScanController.h"
#include "dataman/AMSample.h"

SGMFastScanController::SGMFastScanController(SGMFastScanConfiguration *cfg){
	specificCfg_ = cfg;
	initializationActions_ = 0; //NULL
	cleanUpActions_ = 0; //NULL
	beamlineInitialized_ = false;

	specificScan_ = new AMFastScan();
	pScan()->setName("SGM Fast Scan");
	pScan()->setFileFormat("sgm2010Fast");
	pScan()->setRunId(AMUser::user()->currentRunId());
	pScan()->setScanConfiguration(pCfg());
	pScan()->setSampleId(SGMBeamline::sgm()->currentSampleId());
	QString scanName;
	QString sampleName;
	if(pCfg()->userScanName() == "")
		scanName = pCfg()->autoScanName();
	else
		scanName = pCfg()->userScanName();
	if(pScan()->sampleId() == -1)
		sampleName = "Unknown Sample";
	else
		sampleName = AMSample(pScan()->sampleId(), AMUser::user()->database()).name();
	pScan()->setName(QString("%1 on %2").arg(scanName).arg(sampleName));

	// Create space in raw data store, and create raw data channels, for each detector.

	/*
	//for(int i=0; i<scanDetectors.count(); i++) {
	for(int i = 0; i < pCfg()->allDetectors()->count(); i++) {
		AMDetectorInfo* detectorInfo = pCfg()->allDetectors()->detectorAt(i)->toInfo();
		qDebug() << "Detector named " << pCfg()->allDetectors()->detectorAt(i)->detectorName();
		if(pCfg()->allDetectors()->isDefaultAt(i)){
			if(pScan()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo)))
				pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), i));
			else{
				/// \todo send out and errorMon?
				qDebug() << "BIG PROBLEM!!!!!! WHAT JUST HAPPENED?!?!?" << detectorInfo->name();
			}
		}
	}
	*/
	pScan()->rawData()->addMeasurement(AMMeasurementInfo(* (pCfg()->allDetectors()->detectorNamed("teyScaler")->toInfo()) ));
	pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), 0));
	pScan()->rawData()->addMeasurement(AMMeasurementInfo(* (pCfg()->allDetectors()->detectorNamed("I0Scaler")->toInfo()) ));
	pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), 1));
	pScan()->rawData()->addMeasurement(AMMeasurementInfo(* (pCfg()->allDetectors()->detectorNamed("tfyScaler")->toInfo()) ));
	pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), 2));
	pScan()->rawData()->addMeasurement(AMMeasurementInfo(* (pCfg()->allDetectors()->detectorNamed("photodiodeScaler")->toInfo()) ));
	pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), 3));

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<pScan()->rawDataSources()->count(); i++)
		if(pScan()->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << pScan()->rawDataSources()->at(i);

	int rawTeyIndex = pScan()->rawDataSources()->indexOfKey(SGMBeamline::sgm()->teyDetector()->description());
	int rawTfyIndex = pScan()->rawDataSources()->indexOfKey(SGMBeamline::sgm()->tfyDetector()->description());
	int rawI0Index = pScan()->rawDataSources()->indexOfKey(SGMBeamline::sgm()->i0Detector()->description());

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		//AM1DExpressionAB* teyChannel = new AM1DExpressionAB("tey_n");
		AM1DExpressionAB* teyChannel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->teyDetector()->description()));
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		//teyChannel->setExpression("tey/I0");
		teyChannel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->teyDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		pScan()->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		//AM1DExpressionAB* tfyChannel = new AM1DExpressionAB("tfy_n");
		AM1DExpressionAB* tfyChannel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->tfyDetector()->description()));
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		//tfyChannel->setExpression("tfy/I0");
		tfyChannel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->tfyDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		pScan()->addAnalyzedDataSource(tfyChannel);
	}


	/// \bug CRITICAL Removed creating default channels. They were never set anyway (nothing called the old AMXASScan::setDefaultChannels(); )

}

bool SGMFastScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMFastScanController::beamlineInitialize(){
	bool undulatorTrackingOn = (fabs(SGMBeamline::sgm()->undulatorTracking()->value()-1.0) < SGMBeamline::sgm()->undulatorTracking()->tolerance());
	bool exitSlitTrackingOn = (fabs(SGMBeamline::sgm()->exitSlitTracking()->value()-1.0) < SGMBeamline::sgm()->exitSlitTracking()->tolerance());

	AMBeamlineControlMoveAction *tmpAction = NULL;

	cleanUpActions_ = new AMBeamlineParallelActionsList();

	//Prepare the actions for clean up by making a one stage list with the current values for:
	// Undulator and Exit Slit Tracking
	// Mono Velocity, Base Velocity, and Acceleration
	// Scaler intergration time, scans per buffer, total # of scans, and mode
	cleanUpActions_->appendStage(new QList<AMBeamlineActionItem*>());
	if( undulatorTrackingOn || exitSlitTrackingOn ){
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorTracking());
		tmpAction->setSetpoint(SGMBeamline::sgm()->undulatorTracking()->value());
		cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->exitSlitTracking());
		tmpAction->setSetpoint(SGMBeamline::sgm()->exitSlitTracking()->value());
		cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	}
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingVelocity());
	tmpAction->setSetpoint(SGMBeamline::sgm()->gratingVelocity()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingBaseVelocity());
	tmpAction->setSetpoint(SGMBeamline::sgm()->gratingBaseVelocity()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingAcceleration());
	tmpAction->setSetpoint(SGMBeamline::sgm()->gratingAcceleration()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerIntegrationTime());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerIntegrationTime()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerMode());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerMode()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerScansPerBuffer());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerScansPerBuffer()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerTotalNumberOfScans());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerTotalNumberOfScans()->value());
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);



	SGMFastScanParameters *settings = pCfg()->currentParameters();
	#warning "Hey David, who's going to delete the list and the actions?"
	initializationActions_ = new AMBeamlineParallelActionsList();
	// Only need to do this if tracking is currently on
	if( undulatorTrackingOn || exitSlitTrackingOn ){
		qDebug() << "Need to optimize for middle of energy range";
		//Go to midpoint of energy range
		initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
		tmpAction->setSetpoint(settings->energyMidpoint());
		initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

		//Turn off undulator and exit slit tracking
		initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorTracking());
		tmpAction->setSetpoint(0);
		initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->exitSlitTracking());
		tmpAction->setSetpoint(0);
		initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	}

	//Go to start of energy range
	//Mode needs to change before time, buffer, and total
	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
	tmpAction->setSetpoint(settings->energyStart());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerMode());
	tmpAction->setSetpoint(0);
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	//Set the grating motor velocity, base velocity, and acceleration as well as scaler mode, scans per buffer, total # of scans, and intergration time
	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingVelocity());
	tmpAction->setSetpoint(settings->velocity());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingBaseVelocity());
	tmpAction->setSetpoint(settings->velocityBase());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingAcceleration());
	tmpAction->setSetpoint(settings->acceleration());
//	tmpAction->setSetpoint(5000);
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerIntegrationTime());
	tmpAction->setSetpoint(settings->scalerTime());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerScansPerBuffer());
	tmpAction->setSetpoint(1000);
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerTotalNumberOfScans());
	tmpAction->setSetpoint(1000);
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	/* NTBA March 14, 2011 David Chevrier
	AMDetector* tmpD;
	What detectors, if any, do we need to take of for a fast scan?
	*/

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}
