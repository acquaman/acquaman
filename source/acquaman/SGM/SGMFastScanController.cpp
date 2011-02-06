#include "SGMFastScanController.h"

SGMFastScanController::SGMFastScanController(SGMFastScanConfiguration *cfg){
	specificCfg_ = cfg;
	initializationActions_ = NULL;
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
		tfyChannel->setExpression("-tfy/I0");

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
	AMBeamlineControlMoveAction *tmpAction = NULL;
	SGMFastScanParameters *settings = pCfg()->currentParameters();
	#warning "Hey David, who's going to delete the list and the actions?"
	initializationActions_ = new AMBeamlineParallelActionsList();
	/**/
	//Go to midpoint of energy range
	qDebug() << initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
	qDebug() << tmpAction->setSetpoint(settings->energyMidpoint());
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	//Turn off undulator and exit slit tracking
	qDebug() << initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorTracking());
	qDebug() << tmpAction->setSetpoint(0);
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->exitSlitTracking());
	qDebug() << tmpAction->setSetpoint(0);
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	//Go to start of energy range
	qDebug() << initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
	qDebug() << tmpAction->setSetpoint(settings->energyStart());
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	//Set the grating motor velocity, base velocity, and acceleration
	qDebug() << initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingVelocity());
	qDebug() << tmpAction->setSetpoint(settings->velocity());
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingBaseVelocity());
	qDebug() << tmpAction->setSetpoint(settings->velocityBase());
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->gratingAcceleration());
	qDebug() << tmpAction->setSetpoint(settings->acceleration());
	qDebug() << initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	qDebug() << initializationActions_->stageCount() << initializationActions_->countAt(0) << initializationActions_->countAt(1) << initializationActions_->countAt(2) << initializationActions_->countAt(3);

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
