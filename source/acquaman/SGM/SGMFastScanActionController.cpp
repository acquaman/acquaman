#include "SGMFastScanActionController.h"

#include <QFileInfo>

#include "dataman/AMFastScan.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMTimedWaitAction3.h"
#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "dataman/AMTextStream.h"

SGMFastScanActionController::SGMFastScanActionController(SGMFastScanConfiguration2013 *configuration, QObject *parent) :
	AMScanActionController(configuration, parent)
{
	fileWriterIsBusy_ = false;
	configuration_ = configuration;
	masterListSucceeded_ = false;

	scan_ = new AMFastScan();
	scan_->rawData()->addScanAxis(AMAxisInfo("ev", 0, "Incident Energy", "eV"));
	scan_->setName("SGM Fast Scan");
	scan_->setFileFormat("sgm2013Fast");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(configuration_);
	configuration_->setEnergyParameters(SGMBeamlineInfo::sgmInfo()->energyParametersForGrating(SGMBeamline::sgm()->currentGrating()));
	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");

	insertionIndex_ = AMnDIndex(0);

	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
		if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().detectorInfoAt(x))))))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)
	scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)

	fileWriterThread_ = new QThread();

	qRegisterMetaType<SGMXASScanActionControllerFileWriter::FileWriterError>("FileWriterError");
	SGMXASScanActionControllerFileWriter *fileWriter = new SGMXASScanActionControllerFileWriter(AMUserSettings::userDataFolder+fullPath.filePath(), false);
	connect(fileWriter, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter, SIGNAL(fileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError)), this, SLOT(onFileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter, SLOT(writeToFile(int,QString)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter, SLOT(finishWriting()));
	fileWriter->moveToThread(fileWriterThread_);
	fileWriterThread_->start();
}

void SGMFastScanActionController::onInitializationActionsListSucceeded(){
	disconnect(fastActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(fastActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	setInitialized();
}

void SGMFastScanActionController::onInitializationActionsListFailed(){
	disconnect(fastActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(fastActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	connect(fastActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(fastActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(fastActionsCleanupList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void SGMFastScanActionController::onMasterActionsListSucceeded(){
	masterListSucceeded_ = true;
	disconnect(fastActionsMasterList_, SIGNAL(succeeded()), this, SLOT(onMasterActionsListSucceeded()));
	disconnect(fastActionsMasterList_, SIGNAL(failed()), this, SLOT(onMasterActionsListFailed()));

	connect(fastActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(fastActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(fastActionsCleanupList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void SGMFastScanActionController::onMasterActionsListFailed(){
	disconnect(fastActionsMasterList_, SIGNAL(succeeded()), this, SLOT(onMasterActionsListSucceeded()));
	disconnect(fastActionsMasterList_, SIGNAL(failed()), this, SLOT(onMasterActionsListFailed()));

	connect(fastActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(fastActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(fastActionsCleanupList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void SGMFastScanActionController::onCleanupActionsListSucceeded(){
	disconnect(fastActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(fastActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	if(masterListSucceeded_)
		setFinished();
	else
		setFailed();
}

void SGMFastScanActionController::onCleanupActionsListFailed(){
	disconnect(fastActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(fastActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	setFailed();
}

#include "ui/util/AMMessageBoxWTimeout.h"
void SGMFastScanActionController::onFileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError error){
	qDebug() << "Got a file writer error in Fast Scan" << error;
	QString userErrorString;
	switch(error){
	case SGMXASScanActionControllerFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, SGMFASTSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, "Error, SGM Fast Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	case SGMXASScanActionControllerFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, SGMFASTSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, "Error, SGM Fast Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	default:
		AMErrorMon::alert(this, SGMFASTSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, "Error, SGM Fast Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	}

	setFailed();

	AMMessageBoxWTimeout box(30000);
	box.setWindowTitle("Sorry! Your scan has been cancelled because a file writing error occured.");
	box.setText("Acquaman saves files for long term storage, but some sort of error occured for your scan.");
	box.setInformativeText(userErrorString);

	QPushButton *acknowledgeButton_ = new QPushButton("Ok");

	box.addButton(acknowledgeButton_, QMessageBox::AcceptRole);
	box.setDefaultButton(acknowledgeButton_);

	box.execWTimeout();
}

void SGMFastScanActionController::onFileWriterIsBusy(bool isBusy){
	fileWriterIsBusy_ = isBusy;
	emit readyForDeletion(!fileWriterIsBusy_);
}

bool SGMFastScanActionController::initializeImplementation(){
	AMAction3 *cleanupActions = createCleanupActions();
	if(qobject_cast<AMListAction3*>(cleanupActions))
		fastActionsCleanupList_ = qobject_cast<AMListAction3*>(cleanupActions);

	AMAction3 *initializationActions = createInitializationActions();
	if(qobject_cast<AMListAction3*>(initializationActions))
		fastActionsInitializationList_ = qobject_cast<AMListAction3*>(initializationActions);


	connect(fastActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	connect(fastActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(fastActionsInitializationList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);

	return true;
}

bool SGMFastScanActionController::startImplementation(){
	AMAgnosticDataMessageHandler *dataMessager = AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions");
	AMAgnosticDataMessageQEventHandler *scanActionMessager = qobject_cast<AMAgnosticDataMessageQEventHandler*>(dataMessager);
	if(scanActionMessager)
		scanActionMessager->addReceiver(this);

	SGMFastScanParameters *settings = configuration_->currentParameters();

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	///////////////////
	// MASTER LIST ///
	/////////////////

	fastActionsMasterList_ = new AMListAction3(new AMListActionInfo3("SGM Fast Actions", "SGM Fast Actions"));

	// Undulator Trigger Propogate to 1
	tmpControl = SGMBeamline::sgm()->undulatorFastTrackingTrigger();
	AMControlInfo undulatorFastTrackingTriggerSetpoint = tmpControl->toInfo();
	undulatorFastTrackingTriggerSetpoint.setValue(1);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingTriggerSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsMasterList_->addSubAction(moveAction);

	// Read grating encoder start point
	AMAction3 *readAction;
	readAction = SGMBeamline::sgm()->gratingEncoderDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	fastActionsMasterList_->addSubAction(readAction);

	// Start Axis
	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("SGM Fast Axis"), AMScanAxis::ContinuousMoveAxis));
	fastActionsMasterList_->addSubAction(axisStartAction);

	// Wait 1.0 seconds
	fastActionsMasterList_->addSubAction(new AMTimedWaitAction3(new AMTimedWaitActionInfo3(1.0)));

	// Energy and Scaler:
	// Energy to end energy
	// Trigger Action for Scaler
	AMListAction3 *fastActionsEnergyAndScaler = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Energy and Scaler", "SGM Fast Actions Energy and Scaler"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->energy();
	AMControlInfo energySetpoint = tmpControl->toInfo();
	energySetpoint.setValue(settings->energyEnd());
	moveActionInfo = new AMControlMoveActionInfo3(energySetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyAndScaler->addSubAction(moveAction);
	fastActionsEnergyAndScaler->addSubAction(SGMBeamline::sgm()->newTEYDetector()->createTriggerAction(AMDetectorDefinitions::ContinuousRead));
	fastActionsMasterList_->addSubAction(fastActionsEnergyAndScaler);
	// End Energy and Scaler

	// Read Detectors:
	// Loop over configured detectors
	// Read EncUp
	// Read EncDown
	AMListAction3 *fastActionsReadDetectors = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Read Detectors", "SGM Fast Actions Read Detectors"), AMListAction3::Parallel);
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++){
		readAction = SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().detectorInfoAt(x))->createReadAction();
		readAction->setGenerateScanActionMessage(true);
		fastActionsReadDetectors->addSubAction(readAction);
	}
	readAction = SGMBeamline::sgm()->newEncoderUpDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	fastActionsReadDetectors->addSubAction(readAction);
	readAction = SGMBeamline::sgm()->newEncoderDownDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	fastActionsReadDetectors->addSubAction(readAction);
	fastActionsMasterList_->addSubAction(fastActionsReadDetectors);
	// End Read Detectors

	// Finish Axis
	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("SGM Fast Axis")));
	fastActionsMasterList_->addSubAction(axisFinishAction);

	///////////////////////
	// END MASTER LIST ///
	/////////////////////

	connect(fastActionsMasterList_, SIGNAL(succeeded()), this, SLOT(onMasterActionsListSucceeded()));
	connect(fastActionsMasterList_, SIGNAL(failed()), this, SLOT(onMasterActionsListFailed()));

	AMActionRunner3::scanActionRunner()->addActionToQueue(fastActionsMasterList_);

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

			writeHeaderToFile();

			break;}
		case AMAgnosticDataAPIDefinitions::AxisFinished:{
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

				scan_->rawData()->endInsertRows();
				insertionIndex_[0] = insertionIndex_.i()+1;
			}
			writeDataToFiles();
			setFinished();

			break;}
		case AMAgnosticDataAPIDefinitions::LoopIncremented:

			break;
		case AMAgnosticDataAPIDefinitions::DataAvailable:{

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

			//qDebug() << "Data for " << message.uniqueID() << " is:\n" << localDetectorData;
			allDataMap_.insert(message.uniqueID(), localDetectorData);

			break;}
		case AMAgnosticDataAPIDefinitions::ControlMoved:

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

void SGMFastScanActionController::writeHeaderToFile(){
	AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
	QString separator = "|!|!|";
	QString rank1String;
	rank1String.append("Start Info\n");
	rank1String.append("Version: SGM Generic Fast 0.1\n");
	rank1String.append(QString("-1%1eV\n").arg(separator));

	for(int x = 0; x < scan_->rawData()->measurementCount(); x++){
		oneMeasurementInfo = scan_->rawData()->measurementAt(x);

		rank1String.append(QString("%1%2").arg(x).arg(separator));
		QString measurementInfoString;
		AMTextStream measurementInfoStream(&measurementInfoString);
		measurementInfoStream.write(oneMeasurementInfo);
		rank1String.append(measurementInfoString);
		rank1String.append("\n");
	}
	rank1String.append(QString("Encoder Start Value: %1\n").arg(allDataMap_.value("GratingEncoderFeedback").at(0)));
	rank1String.append(QString("SpacingParam: %1\n").arg(SGMBeamline::sgm()->energySpacingParam()->value()));
	rank1String.append(QString("C1Param: %1\n").arg(SGMBeamline::sgm()->energyC1Param()->value()));
	rank1String.append(QString("C2Param: %1\n").arg(SGMBeamline::sgm()->energyC2Param()->value()));
	rank1String.append(QString("SParam: %1\n").arg(SGMBeamline::sgm()->energySParam()->value()));
	rank1String.append(QString("ThetaParam: %1\n").arg(SGMBeamline::sgm()->energyThetaParam()->value()));
	rank1String.append("End Info\n");
	emit requestWriteToFile(1, rank1String);
}

void SGMFastScanActionController::writeDataToFiles(){
	QString rank1String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	AMnDIndex requestIndex = AMnDIndex(0);

	for(int x = 0; x < insertionIndex_.i()-1; x++){

		rank1String.clear();
		rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, requestIndex.i())));
		AMRawDataSource *oneRawDataSource;
		for(int y = 0; y < scan_->rawDataSourceCount(); y++){
			oneRawDataSource = scan_->rawDataSources()->at(y);
			if(oneRawDataSource->rank() == 1)
				rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(requestIndex)));
		}
		rank1String.append("\n");

		/* Stress testing
		QTime endTime = QTime::currentTime();
		qDebug() << "Time to ready data for writing " << startTime.msecsTo(endTime);
		*/

		emit requestWriteToFile(1, rank1String);
		requestIndex[0] = requestIndex.i()+1;
	}
}

AMAction3* SGMFastScanActionController::createInitializationActions(){
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	SGMFastScanParameters *settings = configuration_->currentParameters();

	//////////////////////
	// INITIALIZATION ///
	////////////////////

	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialization", "SGM Fast Actions Initialization"));

	// Grating to required value
	tmpControl = SGMBeamline::sgm()->grating();
	AMControlInfo gratingSetpoint = tmpControl->toInfo();
	gratingSetpoint.setValue(settings->scanInfo().start().sgmGrating());
	moveActionInfo = new AMControlMoveActionInfo3(gratingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	retVal->addSubAction(moveAction);

	// Tracking off:
	// Undulator Tracking to 0
	// Exit Slit Tracking to 0
	AMListAction3 *fastActionsTrackingOff = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Tracking Off", "SGM Fast Actions Tracking Off"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->undulatorTracking();
	AMControlInfo undulatorTrackingSetpoint = tmpControl->toInfo();
	undulatorTrackingSetpoint.setValue(0);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsTrackingOff->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->exitSlitTracking();
	AMControlInfo exitSlitTrackingSetpoint = tmpControl->toInfo();
	exitSlitTrackingSetpoint.setValue(0);
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsTrackingOff->addSubAction(moveAction);
	retVal->addSubAction(fastActionsTrackingOff);
	// End Tracking Off

	// Initialization:
	// Energy to start energy
	// Undulator Trigger to 0
	// Undulator Relative Step Storage to 12 (non value)
	// Undulator Relative Step to 12 (non value)
	// Undulator Velocity to undulator velocity
	// Grating Velocity, Velocity Base, Acceleration to motor settings
	AMListAction3 *fastActionsEnergyStartAndInit = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Energy Start and Init", "SGM Fast Actions Energy Start and Init"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->energy();
	AMControlInfo energyStartSetpoint = tmpControl->toInfo();
	//energyStartSetpoint.setValue(270);
	energyStartSetpoint.setValue(settings->energyStart());
	moveActionInfo = new AMControlMoveActionInfo3(energyStartSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorFastTracking();
	AMControlInfo undulatorFastTrackingInitSetpoint = tmpControl->toInfo();
	undulatorFastTrackingInitSetpoint.setValue(0);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorRelativeStepStorage();
	AMControlInfo undulatorRelativeStepStorageInitSetpoint = tmpControl->toInfo();
	undulatorRelativeStepStorageInitSetpoint.setValue(12);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepStorageInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorRelativeStep();
	AMControlInfo undulatorRelativeStepInitSetpoint = tmpControl->toInfo();
	undulatorRelativeStepInitSetpoint.setValue(12);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorVelocity();
	AMControlInfo undulatorVelocityInitSetpoint = tmpControl->toInfo();
	undulatorVelocityInitSetpoint.setValue(settings->undulatorVelocity());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorVelocityInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->gratingVelocity();
	AMControlInfo gratingVelocityInitSetpoint = tmpControl->toInfo();
	gratingVelocityInitSetpoint.setValue(settings->velocity());
	moveActionInfo = new AMControlMoveActionInfo3(gratingVelocityInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->gratingBaseVelocity();
	AMControlInfo gratingBaseVelocityInitSetpoint = tmpControl->toInfo();
	gratingBaseVelocityInitSetpoint.setValue(settings->velocityBase());
	moveActionInfo = new AMControlMoveActionInfo3(gratingBaseVelocityInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->gratingAcceleration();
	AMControlInfo gratingAccelerationInitSetpoint = tmpControl->toInfo();
	gratingAccelerationInitSetpoint.setValue(settings->acceleration());
	moveActionInfo = new AMControlMoveActionInfo3(gratingAccelerationInitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsEnergyStartAndInit->addSubAction(moveAction);

	retVal->addSubAction(fastActionsEnergyStartAndInit);
	// End Initialization

	// Initial Positions:
	// Undulator Step to undulator start step
	// Exit Slit to median position
	// Continuous Scaler to Off
	AMListAction3 *fastActionsInitialPositions = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initial Positions", "SGM Fast Actions Initial Positions"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->undulatorStep();
	AMControlInfo undulatorStepSetpoint = tmpControl->toInfo();
	undulatorStepSetpoint.setValue(settings->undulatorStartStep());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorStepSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsInitialPositions->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->exitSlit();
	AMControlInfo exitSlitSetpoint = tmpControl->toInfo();
	exitSlitSetpoint.setValue(settings->exitSlitDistance());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsInitialPositions->addSubAction(moveAction);
	fastActionsInitialPositions->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));
	retVal->addSubAction(fastActionsInitialPositions);
	// End Initial Positions

	// Scaler Settings:
	// Dwell time to milliseconds (seconds/1000)
	// Scans per Buffer to 1000
	// Total Scans to 1000
	AMListAction3 *fastActionsScalerSettings = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Settings", "SGM Fast Actions Scaler Settings"), AMListAction3::Parallel);
	fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(settings->scalerTime()/1000));
	fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1000));
	fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1000));
	retVal->addSubAction(fastActionsScalerSettings);

	AMListAction3 *fastActionsScalerEnableSettings = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Enable Settings", "SGM Fast Actions Scaler Enable Settings"), AMListAction3::Parallel);
	for(int x = 0; x < 32; x++)
		fastActionsScalerEnableSettings->addSubAction(SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction3(true));

	retVal->addSubAction(fastActionsScalerEnableSettings);
	// End Scaler Settings

	// Undulator Trigger to 1
	tmpControl = SGMBeamline::sgm()->undulatorFastTracking();
	AMControlInfo undulatorFastTrackingSetpoint = tmpControl->toInfo();
	undulatorFastTrackingSetpoint.setValue(1);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	retVal->addSubAction(moveAction);

	// Undulator Relative Step Storage to relative step
	tmpControl = SGMBeamline::sgm()->undulatorRelativeStepStorage();
	AMControlInfo undulatorRelativeStepStorageSetpoint = tmpControl->toInfo();
	undulatorRelativeStepStorageSetpoint.setValue(settings->undulatorRelativeStep());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepStorageSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	retVal->addSubAction(moveAction);

	// Beam on actions
	retVal->addSubAction(SGMBeamline::sgm()->createBeamOnActions3());

	///////////////////////////
	// END INITIALIZATION ///
	////////////////////////

	return retVal;
}

AMAction3* SGMFastScanActionController::createCleanupActions(){
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	////////////////
	// CLEAN UP ///
	//////////////
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Cleanup", "SGM Fast Actions Cleanup"));

	// Always close the fast shutter when the fast scan is done
	tmpControl = SGMBeamline::sgm()->fastShutterVoltage();
	AMControlInfo fastShutterSetpoint = tmpControl->toInfo();
	fastShutterSetpoint.setValue(5);
	moveActionInfo = new AMControlMoveActionInfo3(fastShutterSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	retVal->addSubAction(moveAction);

	// Tracking restore:
	// Undulator Tracking to current
	// Exit Slit Tracking to current
	AMListAction3 *fastActionsTrackingRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Tracking Restore", "SGM Fast Actions Tracking Restore"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->undulatorTracking();
	AMControlInfo undulatorTrackingRestoreSetpoint = tmpControl->toInfo();
	undulatorTrackingRestoreSetpoint.setValue(SGMBeamline::sgm()->undulatorTracking()->value());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingRestoreSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsTrackingRestore->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->exitSlitTracking();
	AMControlInfo exitSlitTrackingRestoreSetpoint = tmpControl->toInfo();
	exitSlitTrackingRestoreSetpoint.setValue(SGMBeamline::sgm()->exitSlitTracking()->value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingRestoreSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsTrackingRestore->addSubAction(moveAction);
	retVal->addSubAction(fastActionsTrackingRestore);
	// End Tracking Off

	// Grating Restore:
	// Grating Velocity, Velocity Base, Acceleration to current settings
	AMListAction3 *fastActionsGratingRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Grating Restore", "SGM Fast Actions Grating Restore"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->gratingVelocity();
	AMControlInfo gratingVelocityRestoreSetpoint = tmpControl->toInfo();
	gratingVelocityRestoreSetpoint.setValue(SGMBeamline::sgm()->gratingVelocity()->value());
	moveActionInfo = new AMControlMoveActionInfo3(gratingVelocityRestoreSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsGratingRestore->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->gratingBaseVelocity();
	AMControlInfo gratingBaseVelocityRestoreSetpoint = tmpControl->toInfo();
	gratingBaseVelocityRestoreSetpoint.setValue(SGMBeamline::sgm()->gratingBaseVelocity()->value());
	moveActionInfo = new AMControlMoveActionInfo3(gratingBaseVelocityRestoreSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsGratingRestore->addSubAction(moveAction);
	tmpControl = SGMBeamline::sgm()->gratingAcceleration();
	AMControlInfo gratingAccelerationRestoreSetpoint = tmpControl->toInfo();
	gratingAccelerationRestoreSetpoint.setValue(SGMBeamline::sgm()->gratingAcceleration()->value());
	moveActionInfo = new AMControlMoveActionInfo3(gratingAccelerationRestoreSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	fastActionsGratingRestore->addSubAction(moveAction);

	retVal->addSubAction(fastActionsGratingRestore);
	// End Grating Restore

	// Scaler Settings Restore:
	// Dwell time to current
	// Scans per Buffer to current
	// Total Scans to current
	AMListAction3 *fastActionsScalerSettingsRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Settings Restore", "SGM Fast Actions Scaler Settings Restore"), AMListAction3::Parallel);
	fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(SGMBeamline::sgm()->scaler()->dwellTime()));
	fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(SGMBeamline::sgm()->scaler()->isContinuous()));
	fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(SGMBeamline::sgm()->scaler()->isContinuous() ? 1 : SGMBeamline::sgm()->scaler()->scansPerBuffer()));
	fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(SGMBeamline::sgm()->scaler()->isContinuous() ? 1 : SGMBeamline::sgm()->scaler()->totalScans()));
	for(int x = 0; x < 32; x++)
		fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction3(SGMBeamline::sgm()->scaler()->channelAt(x)->isEnabled()));

	retVal->addSubAction(fastActionsScalerSettingsRestore);
	// End Scaler Settings

	// Synchronized Dwell Restore:
	// Loop elements and enable prior settings
	AMListAction3 *fastActionsSyncDwellEnablesRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Synchronized Dwell Time Enable Settings Restore", "SGM Fast Actions Synchronized Dwell Time Enable Settings Restore"), AMListAction3::Parallel);

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	for(int x = 0; x < syncDwell->elementCount(); x++)
		fastActionsSyncDwellEnablesRestore->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
	retVal->addSubAction(fastActionsSyncDwellEnablesRestore);
	// End Sync Dwell

	////////////////////
	// END CLEAN UP ///
	//////////////////
	return retVal;
}
