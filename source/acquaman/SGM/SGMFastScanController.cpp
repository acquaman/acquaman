#include "SGMFastScanController.h"

SGMFastScanController::SGMFastScanController(SGMFastScanConfiguration *cfg){
	specificCfg_ = cfg;
	initializationActions_ = 0; //NULL
	cleanUpActions_ = 0; //NULL
	beamlineInitialized_ = false;

	QList<AMDetectorInfo*> scanDetectors = pCfg()->usingDetectors();

	specificScan_ = new AMFastScan();
	pScan()->setName("SGM Fast Scan");
	pScan()->setFilePath(pCfg()->filePath()+pCfg()->fileName());
	pScan()->setFileFormat("sgm2010Fast");
	pScan()->setRunId(AMUser::user()->currentRunId());

	// The energy axis will be slightly miscalibrated, but can be approximated as uniform with 1000 points between the
	//  start energy and the final energy
	/*
	pScan()->rawData()->scanAxisAt(0).isUniform = true;
	pScan()->rawData()->scanAxisAt(0).start = pCfg()->start();
	pScan()->rawData()->scanAxisAt(0).increment = (double) ((pCfg()->end() - pCfg()->start())/1000.0);

	qDebug() << pScan()->rawData()->scanAxisAt(0).isUniform << (double)pScan()->rawData()->scanAxisAt(0).start << (double)pScan()->rawData()->scanAxisAt(0).increment;
	*/

	// Create space in raw data store, and create raw data channels, for each detector.

	qDebug() << "Count is " << scanDetectors.count();
	for(int i=0; i<scanDetectors.count(); i++) {
		qDebug() << "Detector at " << i << " is " << scanDetectors.at(i)->name() ;
		AMDetectorInfo* detectorInfo = scanDetectors.at(i);
		pScan()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
		pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), i));
	}
	//pScan()->rawData()->addMeasurement(AMMeasurementInfo("energyFbk", "Energy Feedback", "eV"));
	//pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), scanDetectors.count()));


	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<pScan()->rawDataSources()->count(); i++)
		if(pScan()->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << pScan()->rawDataSources()->at(i);

	int rawTeyIndex = pScan()->rawDataSources()->indexOfKey("tey");
	int rawTfyIndex = pScan()->rawDataSources()->indexOfKey("tfy");
	int rawI0Index = pScan()->rawDataSources()->indexOfKey("I0");

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* teyChannel = new AM1DExpressionAB("tey_n");
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		teyChannel->setExpression("tey/I0");

		pScan()->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* tfyChannel = new AM1DExpressionAB("tfy_n");
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		tfyChannel->setExpression("tfy/I0");

		pScan()->addAnalyzedDataSource(tfyChannel);
	}


	/// \bug CRITICAL Removed creating default channels. They were never set anyway (nothing called the old AMXASScan::setDefaultChannels(); )

}

bool SGMFastScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMFastScanController::beamlineInitialize(){
	/*
	SGMBeamline::sgm()->exitSlitGap()->move( pCfg()->exitSlitGap() );
	SGMBeamline::sgm()->grating()->move( pCfg()->grating() );
	SGMBeamline::sgm()->harmonic()->move( pCfg()->harmonic() );
	SGMBeamline::sgm()->undulatorTracking()->move( pCfg()->undulatorTracking() );
	SGMBeamline::sgm()->monoTracking()->move( pCfg()->monoTracking() );
	SGMBeamline::sgm()->exitSlitTracking()->move( pCfg()->exitSlitTracking() );
	*/

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

	/*
	AMPVControl *tnos = qobject_cast<AMPVControl*>(SGMBeamline::scalerTotalNumberOfScans());
	if(!tnos)
		qDebug() << "SUPER FAIL!";
	else{
		qDebug() << tnos->readPV()
	}
	*/

	//tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->);
	//tmpAction->setSetpoint(settings->velocity());
	//initializationActions_->appendAction(3, tmpAction);

	/**/

	AMDetectorInfo* tmpDI;
#warning "YEAH DAVE, CHECK THIS OUT! I THINK DETECTOR SET IS PROGRAMMED WRONG!!!"
	//for(int x = 0; x < pCfg()->detectorSet()->count(); x++){
	for(int x = 0; x < pCfg()->usingDetectors().count(); x++){
		//tmpDI = pCfg()->detectorSet()->detectorAt(x);
		tmpDI = pCfg()->usingDetectors().at(x);
		#warning "David please review... Had to change because of removed AMDbObject::typeDescription"
		/* previously: typeDescription()s were never the safest way to tell what class something was anyway.
		if(tmpDI->typeDescription() == "PGT SDD Spectrum-Output Detector")
			((PGTDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (PGTDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else if(tmpDI->typeDescription() == "MCP Detector")
			((MCPDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (MCPDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else
			((AMSingleControlDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (AMDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
			*/
		// replaced with: use qobject_cast<toType*>(genericType*).  Returns 0 if genericType* is not of the toType type.
		PGTDetector* pgtDetector;
		MCPDetector* mcpDetector;
		AMSingleControlDetector* scDetector;

		#warning "David: I'm concerned about the cfgDetectorInfoSet and the usingDetectors not being aligned in x so you can fix it and uncomment it"
		/*
		if( (pgtDetector = qobject_cast<PGTDetector*>(tmpDI)) )
			pgtDetector->setControls( (PGTDetectorInfo*)pCfg()->cfgDetectorInfoSet()->detectorAt(x) );
		else if( (mcpDetector = qobject_cast<MCPDetector*>(tmpDI)) )
			mcpDetector->setControls( (MCPDetectorInfo*)pCfg()->cfgDetectorInfoSet()->detectorAt(x) );
		else if( (scDetector = qobject_cast<AMSingleControlDetector*>(tmpDI)) )
			scDetector->setControls( (AMDetectorInfo*)pCfg()->cfgDetectorInfoSet()->detectorAt(x) );
		*/

	}

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}

void SGMFastScanController::reinitialize(){
	/// \bug CRITICAL this was commented out. Why?
	delete specificScan_;

	QList<AMDetectorInfo*> scanDetectors;
	scanDetectors = pCfg()->usingDetectors();
	//scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	//scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());


	specificScan_ = new AMFastScan();
	//_pScan_ = &specificScan_;
	pScan()->setName("SGM Fast Scan");
	pScan()->setFilePath(pCfg()->filePath()+pCfg()->fileName());
	pScan()->setFileFormat("sgm2010Fast");

	/*
	// The energy axis will be slightly miscalibrated, but can be approximated as uniform with 1000 points between the
	//  start energy and the final energy
	pScan()->rawData()->scanAxisAt(0).isUniform = true;
	pScan()->rawData()->scanAxisAt(0).start = pCfg()->start();
	pScan()->rawData()->scanAxisAt(0).increment = (double) ((pCfg()->end() - pCfg()->start())/1000.0);
	*/

	// Create space in raw data store, and create raw data channels, for each detector.

	for(int i=0; i<scanDetectors.count(); i++) {
		AMDetectorInfo* detectorInfo = scanDetectors.at(i);

		pScan()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
		pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), i));
	}

	/// \bug CRITICAL removed creating default channels. Was never used anyway.
}
