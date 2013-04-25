#include "SGMFastScanActionController.h"
#include "dataman/AMFastScan.h"

SGMFastScanActionController::SGMFastScanActionController(SGMFastScanConfiguration2013 *configuration, QObject *parent) :
	AMScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMFastScan();
	scan_->rawData()->addScanAxis(AMAxisInfo("ev", 0, "Incident Energy", "eV"));
	scan_->setName("SGM Fast Scan");
	scan_->setFileFormat("sgm2010Fast");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(configuration_);
	configuration_->setEnergyParameters(SGMBeamlineInfo::sgmInfo()->energyParametersForGrating(SGMBeamline::sgm()->currentGrating()));
	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");

	insertionIndex_ = AMnDIndex(0);

	/*
	if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->newTEYDetector()))))
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));

	if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->newI0Detector()))))
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
	*/

	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++){
		qDebug() << "New Fast Scan told to use " << configuration_->detectorConfigurations().detectorInfoAt(x).name();
		if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().detectorInfoAt(x))))))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
	}
}

void SGMFastScanActionController::onHackedActionsSucceeded(){
	setFinished();
}

bool SGMFastScanActionController::initializeImplementation(){
	QTimer::singleShot(0, this, SLOT(setInitialized()));
	return true;
}

#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMTimedWaitAction3.h"
#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
bool SGMFastScanActionController::startImplementation(){
	AMAgnosticDataMessageHandler *dataMessager = AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions");
	AMAgnosticDataMessageQEventHandler *scanActionMessager = qobject_cast<AMAgnosticDataMessageQEventHandler*>(dataMessager);
	if(scanActionMessager)
		scanActionMessager->addReceiver(this);

	SGMFastScanParameters *settings = configuration_->currentParameters();

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *hackedFastActions = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions", "SGM Hacked Fast Actions"));

	// Tracking off:
	// Undulator Tracking to 0
	// Exit Slit Tracking to 0
	AMListAction3 *hackedFastActionsTrackingOff = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Tracking Off", "SGM Hacked Fast Actions Tracking Off"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->undulatorTracking();
	AMControlInfo undulatorTrackingSetpoint = tmpControl->toInfo();
	undulatorTrackingSetpoint.setValue(0);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsTrackingOff->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->exitSlitTracking();
	AMControlInfo exitSlitTrackingSetpoint = tmpControl->toInfo();
	exitSlitTrackingSetpoint.setValue(0);
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsTrackingOff->addSubAction(moveAction);
	hackedFastActions->addSubAction(hackedFastActionsTrackingOff);
	// End Tracking Off

	// Initialization:
	// Energy to start energy
	// Undulator Trigger to 0
	// Undulator Relative Step Storage to 12 (non value)
	// Undulator Relative Step to 12 (non value)
	// Undulator Velocity to undulator velocity
	// Grating Velocity, Velocity Base, Acceleration to motor settings
	AMListAction3 *hackedFastActionsEnergyStartAndInit = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Energy Start and Init", "SGM Hacked Fast Actions Energy Start and Init"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->energy();
	AMControlInfo energyStartSetpoint = tmpControl->toInfo();
	//energyStartSetpoint.setValue(270);
	energyStartSetpoint.setValue(settings->energyStart());
	moveActionInfo = new AMControlMoveActionInfo3(energyStartSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorFastTracking();
	AMControlInfo undulatorFastTrackingInitSetpoint = tmpControl->toInfo();
	undulatorFastTrackingInitSetpoint.setValue(0);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorRelativeStepStorage();
	AMControlInfo undulatorRelativeStepStorageInitSetpoint = tmpControl->toInfo();
	undulatorRelativeStepStorageInitSetpoint.setValue(12);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepStorageInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorRelativeStep();
	AMControlInfo undulatorRelativeStepInitSetpoint = tmpControl->toInfo();
	undulatorRelativeStepInitSetpoint.setValue(12);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorVelocity();
	AMControlInfo undulatorVelocityInitSetpoint = tmpControl->toInfo();
	//undulatorVelocityInitSetpoint.setValue(6000);
	undulatorVelocityInitSetpoint.setValue(settings->undulatorVelocity());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorVelocityInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->gratingVelocity();
	AMControlInfo gratingVelocityInitSetpoint = tmpControl->toInfo();
	//gratingVelocityInitSetpoint.setValue(40000);
	gratingVelocityInitSetpoint.setValue(settings->velocity());
	moveActionInfo = new AMControlMoveActionInfo3(gratingVelocityInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->gratingBaseVelocity();
	AMControlInfo gratingBaseVelocityInitSetpoint = tmpControl->toInfo();
	//gratingBaseVelocityInitSetpoint.setValue(40000);
	gratingBaseVelocityInitSetpoint.setValue(settings->velocityBase());
	moveActionInfo = new AMControlMoveActionInfo3(gratingBaseVelocityInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->gratingAcceleration();
	AMControlInfo gratingAccelerationInitSetpoint = tmpControl->toInfo();
	//gratingAccelerationInitSetpoint.setValue(40000);
	gratingAccelerationInitSetpoint.setValue(settings->acceleration());
	moveActionInfo = new AMControlMoveActionInfo3(gratingAccelerationInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyStartAndInit->addSubAction(moveAction);

	hackedFastActions->addSubAction(hackedFastActionsEnergyStartAndInit);
	// End Initialization

	// Initial Positions:
	// Undulator Step to undulator start step
	// Exit Slit to median position
	// Continuous Scaler to Off
	AMListAction3 *hackedFastActionsInitialPositions = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Initial Positions", "SGM Hacked Fast Actions Initial Positions"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->undulatorStep();
	AMControlInfo undulatorStepSetpoint = tmpControl->toInfo();
	//undulatorStepSetpoint.setValue(-149991);
	undulatorStepSetpoint.setValue(settings->undulatorStartStep());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorStepSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsInitialPositions->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->exitSlit();
	AMControlInfo exitSlitSetpoint = tmpControl->toInfo();
	//exitSlitSetpoint.setValue(200.46);
	exitSlitSetpoint.setValue(settings->exitSlitDistance());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsInitialPositions->addSubAction(moveAction);
	hackedFastActionsInitialPositions->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));
	hackedFastActions->addSubAction(hackedFastActionsInitialPositions);
	// End Initial Positions

	// Scaler Settings:
	// Dwell time to milliseconds (seconds/1000)
	// Scans per Buffer to 1000
	// Total Scans to 1000
	AMListAction3 *hackedFastActionsScalerSettings = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Scaler Settings", "SGM Hacked Fast Actions Scaler Settings"), AMListAction3::Parallel);
	//hackedFastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(0.005));
	hackedFastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(settings->scalerTime()/1000));
	hackedFastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1000));
	hackedFastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1000));
	hackedFastActions->addSubAction(hackedFastActionsScalerSettings);
	// End Scaler Settings

	// Undulator Trigger to 1
	tmpControl = SGMBeamline::sgm()->undulatorFastTracking();
	AMControlInfo undulatorFastTrackingSetpoint = tmpControl->toInfo();
	undulatorFastTrackingSetpoint.setValue(1);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActions->addSubAction(moveAction);

	// Undulator Relative Step Storage to relative step
	tmpControl = SGMBeamline::sgm()->undulatorRelativeStepStorage();
	AMControlInfo undulatorRelativeStepStorageSetpoint = tmpControl->toInfo();
	//undulatorRelativeStepStorageSetpoint.setValue(16930);
	undulatorRelativeStepStorageSetpoint.setValue(settings->undulatorRelativeStep());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepStorageSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActions->addSubAction(moveAction);

	// Undulator Trigger Propogate to 1
	tmpControl = SGMBeamline::sgm()->undulatorFastTrackingTrigger();
	AMControlInfo undulatorFastTrackingTriggerSetpoint = tmpControl->toInfo();
	undulatorFastTrackingTriggerSetpoint.setValue(1);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingTriggerSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActions->addSubAction(moveAction);

	// Start Axis
	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("SGM Fast Axis"), AMScanAxis::ContinuousMoveAxis));
	hackedFastActions->addSubAction(axisStartAction);

	// Read grating encoder start point
	AMAction3 *readAction;
	readAction = SGMBeamline::sgm()->gratingEncoderDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActions->addSubAction(readAction);

	// Wait 1.0 seconds
	hackedFastActions->addSubAction(new AMTimedWaitAction3(new AMTimedWaitActionInfo3(1.0)));

	// Energy and Scaler:
	// Energy to end energy
	// Trigger Action for Scaler
	AMListAction3 *hackedFastActionsEnergyAndScaler = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Energy and Scaler", "SGM Hacked Fast Actions Energy and Scaler"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->energy();
	AMControlInfo energySetpoint = tmpControl->toInfo();
	//energySetpoint.setValue(320);
	energySetpoint.setValue(settings->energyEnd());
	moveActionInfo = new AMControlMoveActionInfo3(energySetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyAndScaler->addSubAction(moveAction);
	//hackedFastActionsEnergyAndScaler->addSubAction(SGMBeamline::sgm()->scaler()->createStartAction3(true));
	hackedFastActionsEnergyAndScaler->addSubAction(SGMBeamline::sgm()->newTEYDetector()->createTriggerAction(AMDetectorDefinitions::ContinuousRead));
	hackedFastActions->addSubAction(hackedFastActionsEnergyAndScaler);
	// End Energy and Scaler

	// Read Detectors:
	// Loop over configured detectors
	// Read EncUp
	// Read EncDown
	AMListAction3 *hackedFastActionsReadDetectors = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Read Detectors", "SGM Hacked Fast Actions Read Detectors"), AMListAction3::Parallel);
	/*
	readAction = SGMBeamline::sgm()->newTEYDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	readAction = SGMBeamline::sgm()->newI0Detector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	*/
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++){
		readAction = SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().detectorInfoAt(x))->createReadAction();
		readAction->setGenerateScanActionMessage(true);
		hackedFastActionsReadDetectors->addSubAction(readAction);
	}
	readAction = SGMBeamline::sgm()->newEncoderUpDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	readAction = SGMBeamline::sgm()->newEncoderDownDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	hackedFastActions->addSubAction(hackedFastActionsReadDetectors);
	// End Read Detectors

	// Finish Axis
	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("SGM Fast Axis")));
	hackedFastActions->addSubAction(axisFinishAction);

	connect(hackedFastActions, SIGNAL(succeeded()), this, SLOT(onHackedActionsSucceeded()));

	AMActionRunner3::scanActionRunner()->addActionToQueue(hackedFastActions);

	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	setStarted();
	return true;
}

void SGMFastScanActionController::cancelImplementation(){
	AMScanActionController::cancelImplementation();
}

bool SGMFastScanActionController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){
		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){
		case AMAgnosticDataAPIDefinitions::AxisStarted:{

			//writeHeaderToFile();

			break;}
		case AMAgnosticDataAPIDefinitions::AxisFinished:{

			qDebug() << "Roll out, let's start adding data to the fast scan";

			encoderStartValue_ = allDataMap_.value("GratingEncoderFeedback").at(0);
			spacingParam_ = SGMBeamline::sgm()->energySpacingParam()->value();
			c1Param_ = SGMBeamline::sgm()->energyC1Param()->value();
			c2Param_ = SGMBeamline::sgm()->energyC2Param()->value();
			sParam_ = SGMBeamline::sgm()->energySParam()->value();
			thetaParam_ = SGMBeamline::sgm()->energyThetaParam()->value();

			int currentEncoderValue = encoderStartValue_;
			double energyFeedback;

			for(int x = 0; x < allDataMap_.value("NEWEncoderUp").count(); x++){
				currentEncoderValue += allDataMap_.value("NEWEncoderUp").at(x);
				currentEncoderValue -= allDataMap_.value("NEWEncoderDown").at(x);
				energyFeedback = (1.0e-9*1239.842*sParam_)/(2*spacingParam_*c1Param_*c2Param_*(double)currentEncoderValue*cos(thetaParam_/2));

				scan_->rawData()->beginInsertRows(1, -1);
				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), energyFeedback);
				for(int y = 0; y < configuration_->detectorConfigurations().count(); y++)
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(configuration_->detectorConfigurations().detectorInfoAt(y).name()), AMnDIndex(), allDataMap_.value(configuration_->detectorConfigurations().detectorInfoAt(y).name()).at(x));
				/*
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("NEWTEY"), AMnDIndex(), allDataMap_.value("NEWTEY").at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("NEWI0"), AMnDIndex(), allDataMap_.value("NEWI0").at(x));
				*/
				scan_->rawData()->endInsertRows();
				insertionIndex_[0] = insertionIndex_.i()+1;
			}
			setFinished();

			//scan_->rawData()->endInsertRows();
			//writeDataToFiles();
			//emit finishWritingToFile();
			//setFinished();

			break;}
		case AMAgnosticDataAPIDefinitions::LoopIncremented:

			//scan_->rawData()->endInsertRows();
			//writeDataToFiles();
			//insertionIndex_[0] = insertionIndex_.i()+1;

			break;
		case AMAgnosticDataAPIDefinitions::DataAvailable:{

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

			qDebug() << "Data for " << message.uniqueID() << " is:\n" << localDetectorData;
			allDataMap_.insert(message.uniqueID(), localDetectorData);
			/*
			if(insertionIndex_.i() >= scan_->rawData()->scanSize(0)){
				scan_->rawData()->beginInsertRows(insertionIndex_.i()-scan_->rawData()->scanSize(0)+1, -1);
				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), currentAxisValue_);
			}

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());
			*/

			break;}
		case AMAgnosticDataAPIDefinitions::ControlMoved:

			//if(message.value("ControlMovementType") == "Absolute")
			//	currentAxisValue_ = message.value("ControlMovementValue").toDouble();
			//else if(message.value("ControlMovementType") == "Relative")
			//	currentAxisValue_ += message.value("ControlMovementValue").toDouble();

			break;
		case AMAgnosticDataAPIDefinitions::InvalidMessage:
			break;
		default:
			break;
		}

		e->accept();
		return true;
	}
	else
		return AMScanActionController::event(e);
}
