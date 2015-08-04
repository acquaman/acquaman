/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMFastScanController.h"
#include "dataman/AMSamplePre2013.h"

SGMFastScanController::SGMFastScanController(SGMFastScanConfiguration *cfg){
	config_ = cfg;
	initializationActions_ = 0; //NULL
	cleanUpActions_ = 0; //NULL
	beamlineInitialized_ = false;

	specificScan_ = new AMFastScan();
	specificScan_->rawData()->addScanAxis(AMAxisInfo("ev", 0, "Incident Energy", "eV"));
	specificScan_->setName("SGM Fast Scan");
	specificScan_->setFileFormat("sgm2010Fast");
	specificScan_->setRunId(AMUser::user()->currentRunId());
	specificScan_->setScanConfiguration(config_);
	config_->setEnergyParameters(SGMBeamlineInfo::sgmInfo()->energyParametersForGrating(SGMBeamline::sgm()->currentGrating()));
	specificScan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	specificScan_->setIndexType("fileSystem");
	QString scanName;
	QString sampleName;
	if(specificScan_->sampleId() == -1)
		sampleName = "Unknown Sample";
	else
		sampleName = AMSamplePre2013(specificScan_->sampleId(), AMUser::user()->database()).name();
	if(config_->userScanName() == ""){
		scanName = config_->autoScanName();
		specificScan_->setName(QString("%1 - %2").arg(sampleName).arg(scanName));
	}
	else{
		scanName = config_->userScanName();
		specificScan_->setName(QString("%1 - %2").arg(scanName).arg(sampleName));
	}

	// Create space in raw data store, and create raw data channels, for each detector.

	/*
	//for(int i=0; i<scanDetectors.count(); i++) {
	for(int i = 0; i < config_->allDetectors()->count(); i++) {
		AMDetectorInfo* detectorInfo = config_->allDetectors()->detectorAt(i)->toInfo();
		qdebug() << "Detector named " << config_->allDetectors()->detectorAt(i)->detectorName();
		if(config_->allDetectors()->isDefaultAt(i)){
			if(specificScan_->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo)))
				specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), i));
			else{
				/// \todo send out and errorMon?
				qdebug() << "BIG PROBLEM!!!!!! WHAT JUST HAPPENED?!?!?" << detectorInfo->name();
			}
		}
	}
	*/

	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("teyScaler")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 0));
	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("I0Scaler")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 1));
	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("tfyScaler")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 2));
	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("photodiodeScaler")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 3));

	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("filterPD1Current")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 4));
	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("filterPD2Current")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 5));
	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("filterPD3Current")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 6));
	specificScan_->rawData()->addMeasurement(AMMeasurementInfo(* (config_->allDetectors()->detectorNamed("filterPD4Current")->toInfo()) ));
	specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), 7));

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<specificScan_->rawDataSources()->count(); i++)
		if(specificScan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << specificScan_->rawDataSources()->at(i);

	int rawTeyIndex = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->teyDetector()->description());
	int rawTfyIndex = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->tfyDetector()->description());
	int rawI0Index = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->i0Detector()->description());

	/*
	int rawFPD1Index = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->filterPD1ScalarDetector()->description().remove(' '));
	int rawFPD2Index = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->filterPD2ScalarDetector()->description().remove(' '));
	int rawFPD3Index = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->filterPD3ScalarDetector()->description().remove(' '));
	int rawFPD4Index = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->filterPD4ScalarDetector()->description().remove(' '));
	*/

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* teyChannel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->teyDetector()->description()));
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		teyChannel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->teyDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* tfyChannel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->tfyDetector()->description()));
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		tfyChannel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->tfyDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(tfyChannel);
	}

	/*
	//Adding normalized channels for new diodes
	if(rawFPD1Index != -1 && rawI0Index != -1) {
		AM1DExpressionAB* fpd1Channel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->filterPD1ScalarDetector()->description()));
		fpd1Channel->setDescription("Normalized Aux1");
		fpd1Channel->setInputDataSources(raw1DDataSources);
		fpd1Channel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->filterPD1ScalarDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(fpd1Channel);
	}
	if(rawFPD2Index != -1 && rawI0Index != -1) {
		AM1DExpressionAB* fpd2Channel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->filterPD2ScalarDetector()->description()));
		fpd2Channel->setDescription("Normalized Aux2");
		fpd2Channel->setInputDataSources(raw1DDataSources);
		fpd2Channel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->filterPD2ScalarDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(fpd2Channel);
	}
	if(rawFPD3Index != -1 && rawI0Index != -1) {
		AM1DExpressionAB* fpd3Channel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->filterPD3ScalarDetector()->description()));
		fpd3Channel->setDescription("Normalized Aux3");
		fpd3Channel->setInputDataSources(raw1DDataSources);
		fpd3Channel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->filterPD3ScalarDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(fpd3Channel);
	}
	if(rawFPD4Index != -1 && rawI0Index != -1) {
		AM1DExpressionAB* fpd4Channel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->filterPD4ScalarDetector()->description()));
		fpd4Channel->setDescription("Normalized Aux4");
		fpd4Channel->setInputDataSources(raw1DDataSources);
		fpd4Channel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->filterPD4ScalarDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(fpd4Channel);
	}
	*/
}

bool SGMFastScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMFastScanController::beamlineInitialize(){
	bool undulatorTrackingOn = (fabs(SGMBeamline::sgm()->undulatorTracking()->value()-1.0) < SGMBeamline::sgm()->undulatorTracking()->tolerance());
	bool exitSlitTrackingOn = (fabs(SGMBeamline::sgm()->exitSlitTracking()->value()-1.0) < SGMBeamline::sgm()->exitSlitTracking()->tolerance());

	AMBeamlineControlMoveAction *tmpAction = 0; //NULL;
	AMBeamlineActionItem *tmpBAction = 0; //NULL

	bool cleanupFailed = false;
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

	tmpBAction = SGMBeamline::sgm()->scaler()->createDwellTimeAction(SGMBeamline::sgm()->scaler()->dwellTime());
	tmpBAction ? cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpBAction) : cleanupFailed = true;
	tmpBAction = SGMBeamline::sgm()->scaler()->createContinuousEnableAction(SGMBeamline::sgm()->scaler()->isContinuous());
	tmpBAction ? cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpBAction) : cleanupFailed = true;
	tmpBAction = SGMBeamline::sgm()->scaler()->createScansPerBufferAction(SGMBeamline::sgm()->scaler()->isContinuous() ? 1 : SGMBeamline::sgm()->scaler()->scansPerBuffer());
	tmpBAction ? cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpBAction) : cleanupFailed = true;
	tmpBAction = SGMBeamline::sgm()->scaler()->createTotalScansAction(SGMBeamline::sgm()->scaler()->isContinuous() ? 1 : SGMBeamline::sgm()->scaler()->totalScans());
	tmpBAction ? cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpBAction) : cleanupFailed = true;

	for(int x = 0; x < 32; x++){
		tmpBAction = SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction(SGMBeamline::sgm()->scaler()->channelAt(x)->isEnabled());
		tmpBAction ? cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpBAction) : cleanupFailed = true;
	}

	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorFastTracking());
	tmpAction->setSetpoint(0);
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorVelocity());
	tmpAction->setSetpoint(11811);
	cleanUpActions_->appendAction(cleanUpActions_->stageCount()-1, tmpAction);

	SGMFastScanParameters *settings = config_->currentParameters();
	/* NTBA - August 25th, 2011 (David Chevrier)
		Who's going to delete the list and the actions?"
	*/
	bool initializationFailed = false;
	initializationActions_ = new AMBeamlineParallelActionsList();

	if( !SGMBeamline::sgm()->grating()->withinTolerance(settings->scanInfo().start().sgmGrating())){
		initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->grating());
		tmpAction->setSetpoint(settings->scanInfo().start().sgmGrating());
		initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	}

	if( SGMBeamline::sgm()->energy()->withinTolerance(settings->energyStart()) ){
		initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
		tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
		tmpAction->setSetpoint(settings->energyStart()+1.0);
		initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	}

	// Only need to do this if tracking is currently on
	if( undulatorTrackingOn || exitSlitTrackingOn ){
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
	for(int x = 0; x < config_->allDetectors()->count(); x++){
		if(config_->allDetectorConfigurations().isActiveAt(x)){
			config_->allDetectors()->detectorAt(x)->activate();
			if(config_->allDetectors()->detectorAt(x)->turnOnAction()){
//				qdebug() << "Fast scan wants to turn on HV";
//				initializationActions_->appendAction(initializationActions_->stageCount()-1, config_->allDetectors()->detectorAt(x)->turnOnAction());
			}
		}
	}
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
	tmpAction->setSetpoint(settings->energyStart());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpBAction = SGMBeamline::sgm()->scaler()->createContinuousEnableAction(false);
	tmpBAction ? initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpBAction) : cleanupFailed = true;

	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorStep());
	tmpAction->setSetpoint(settings->undulatorStartStep());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->exitSlit());
	tmpAction->setSetpoint(settings->exitSlitDistance());
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

	tmpBAction = SGMBeamline::sgm()->scaler()->createDwellTimeAction(settings->scalerTime()/1000);
	tmpBAction ? initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpBAction) : initializationFailed = true;
	tmpBAction = SGMBeamline::sgm()->scaler()->createScansPerBufferAction(1000);
	tmpBAction ? initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpBAction) : initializationFailed = true;
	tmpBAction = SGMBeamline::sgm()->scaler()->createTotalScansAction(1000);
	tmpBAction ? initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpBAction) : initializationFailed = true;

	bool enableIt = false;
	for(int x = 0; x < 32; x++){
		//(x < 6) ? enableIt = true : enableIt = false;
		// Adding the new diodes
		(x < 10) ? enableIt = true : enableIt = false;
		tmpBAction = SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction(enableIt);
		tmpBAction ? initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpBAction) : initializationFailed = true;
	}

	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorRelativeStepStorage());
	tmpAction->setSetpoint(settings->undulatorRelativeStep());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorVelocity());
	tmpAction->setSetpoint(settings->undulatorVelocity());
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->undulatorFastTracking());
	tmpAction->setSetpoint(1);
	initializationActions_->appendAction(initializationActions_->stageCount()-1, tmpAction);

	beamlineInitialized_ = !initializationFailed;
	return beamlineInitialized_;
}
