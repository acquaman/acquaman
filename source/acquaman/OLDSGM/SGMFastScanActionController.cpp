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


#include "SGMFastScanActionController.h"

#include <QFileInfo>
#include <QApplication>

#include "dataman/AMFastScan.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMWaitAction.h"
#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "dataman/AMTextStream.h"
#include "dataman/AMSample.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/AMCurrentAmplifier.h"



SGMFastScanActionController::SGMFastScanActionController(SGMFastScanConfiguration2013 *configuration, QObject *parent) :
	AMScanActionController(configuration, parent)
{
	goodInitialState_ = false;

	configuration_ = configuration;
	insertionIndex_ = AMnDIndex(0);

	enableUpDownScanning_ = configuration_->enableUpDownScanning();

	scan_ = new AMFastScan();
	scan_->rawData()->addScanAxis(AMAxisInfo("ev", 0, "Incident Energy", "eV"));
	scan_->setName("SGM Fast Scan");
	scan_->setFileFormat("sgm2013Fast");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(configuration_);
	configuration_->setEnergyParameters(SGMBeamlineInfo::sgmInfo()->energyParametersForGrating(SGMBeamline::sgm()->currentGrating()));
	scan_->setSample(SGMBeamline::sgm()->currentSample());
	if(SGMBeamline::sgm()->currentSample())
		SGMBeamline::sgm()->currentSample()->addScan(scan_);
	scan_->setIndexType("fileSystem");

	QString scanName;
	QString sampleName;
	if(scan_->sampleId() == -1)
		sampleName = "Unknown Sample";
	else if(scan_->sample())
		sampleName = scan_->sample()->name();
	else
		sampleName = AMSample(scan_->sampleId(), AMUser::user()->database()).name();
	if(configuration_->userScanName() == ""){
		scanName = configuration_->autoScanName();
		scan_->setName(QString("%1 - %2").arg(sampleName).arg(scanName));
	}
	else{
		scanName = configuration_->userScanName();
		scan_->setName(QString("%1 - %2").arg(scanName).arg(sampleName));
	}
	scan_->setNotes(buildNotes());

	configureStartEndValues();
}

SGMFastScanActionController::~SGMFastScanActionController()
{
	// No need to clean up fileWriterThread, we'll be informed to delete ourself after it is destroyed
}

void SGMFastScanActionController::buildScanController()
{
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
		if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().detectorInfoAt(x))))))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)
	scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)

	fileWriterThread_ = new QThread();

	qRegisterMetaType<AMScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
	fileWriter_ = new AMScanActionControllerBasicFileWriter(AMUserSettings::userDataFolder+fullPath.filePath(), false);
	connect(fileWriter_, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter_, SIGNAL(fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter_, SLOT(writeToFile(int,QString)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter_, SLOT(finishWriting()));
	fileWriter_->moveToThread(fileWriterThread_);
	fileWriterThread_->start();
}

#include "ui/util/AMMessageBoxWTimeout.h"
void SGMFastScanActionController::onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error){
	QString userErrorString;
	switch(error){
	case AMScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, SGMFASTSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, "Error, SGM Fast Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, SGMFASTSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, "Error, SGM Fast Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::FailedToWriteFile:
		AMErrorMon::error(this, SGMFASTSCANACTIONCONTROLLER_FAILED_TO_WRITE_FILE, QString("Error, SGM Fast Scan Action Controller failed to write your data. This is a serious problem, please contact the Acquaman developers."));
		userErrorString = "Your scan has been aborted because Acquaman was unable to write to the desired file (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	default:
		AMErrorMon::alert(this, SGMFASTSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, "Error, SGM Fast Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	}

	setFailed();

	AMMessageBoxWTimeout::showMessageWTimeout("Sorry! Your scan has been cancelled because a file writing error occured.",
											  "Acquaman saves files for long term storage, but some sort of error occured for your scan.",
											  userErrorString);
}

void SGMFastScanActionController::onEverythingFinished(){
	if(goodInitialState_ && (!SGMBeamline::sgm()->undulatorTracking()->withinTolerance(1) || !SGMBeamline::sgm()->exitSlitTracking()->withinTolerance(1)) ){
		//		qDebug() << "\n\n\nDETECTED A LOSS OF TRACKING STATE\n\n";
	}
}

bool SGMFastScanActionController::startImplementation(){
	if(qFuzzyIsNull(SGMBeamline::sgm()->undulatorForcedOpen()->value() - 1))
	{
		this->setFailed();
		return false;
	}

	connect(this, SIGNAL(finished()), this, SLOT(onEverythingFinished()));

	AMAgnosticDataMessageHandler *dataMessager = AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions");
	AMAgnosticDataMessageQEventHandler *scanActionMessager = qobject_cast<AMAgnosticDataMessageQEventHandler*>(dataMessager);
	if(scanActionMessager)
		scanActionMessager->addReceiver(this);


	///////////////////
	// MASTER LIST ///
	/////////////////

	AMListAction3 *masterFastScanActionList = new AMListAction3(new AMListActionInfo3("SGM Fast Actions", "SGM Fast Actions"));

	// Undulator Trigger Propogate to 1
	masterFastScanActionList->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorFastTrackingTrigger(), 1));

	// Read grating encoder start point
	AMAction3 *readAction;
	readAction = SGMBeamline::sgm()->gratingEncoderDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	masterFastScanActionList->addSubAction(readAction);

	// Start Axis
	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("SGM Fast Axis"), AMScanAxis::ContinuousMoveAxis));
	masterFastScanActionList->addSubAction(axisStartAction);

	// Wait 1.0 seconds
	masterFastScanActionList->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.25)));

	// Energy and Scaler:
	// Energy to end energy
	// Trigger Action for Scaler
	AMListAction3 *fastActionsEnergyAndScaler = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Energy and Scaler", "SGM Fast Actions Energy and Scaler"), AMListAction3::Parallel);
	fastActionsEnergyAndScaler->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->energy(), endEnergy_));
	fastActionsEnergyAndScaler->addSubAction(SGMBeamline::sgm()->newTEYDetector()->createTriggerAction(AMDetectorDefinitions::ContinuousRead));
	masterFastScanActionList->addSubAction(fastActionsEnergyAndScaler);
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
	masterFastScanActionList->addSubAction(fastActionsReadDetectors);
	// End Read Detectors

	// Finish Axis
	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("SGM Fast Axis")));
	masterFastScanActionList->addSubAction(axisFinishAction);

	///////////////////////
	// END MASTER LIST ///
	/////////////////////

	scanningActions_ = masterFastScanActionList;
	connect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onScanningActionsSucceeded()));
	connect(scanningActions_, SIGNAL(failed()), this, SLOT(onScanningActionsFailed()));

	AMActionRunner3::scanActionRunner()->addActionToQueue(scanningActions_);

	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	setStarted();
	return true;
}

void SGMFastScanActionController::cancelImplementation(){
	if(lastState() == AMScanController::Running){
		SGMBeamline::sgm()->energy()->stop();
		SGMBeamline::sgm()->undulatorStep()->stop();
		SGMBeamline::sgm()->scaler()->setScanning(false);
	}
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

			QVector<double> energyList = QVector<double>(1000);
			for(int x = 0; x < allDataMap_.value("EncoderUp").count(); x++){
				currentEncoderValue += allDataMap_.value("EncoderUp").at(x);
				currentEncoderValue -= allDataMap_.value("EncoderDown").at(x);
				energyFeedback = (1.0e-9*1239.842*sParam_)/(2*spacingParam_*c1Param_*c2Param_*(double)currentEncoderValue*cos(thetaParam_/2));

				energyList[x] = energyFeedback;
			}

			bool upScan = false;
			if(energyList.at(0) < energyList.at(999))
				upScan = true;

			if(upScan){
				for(int x = 0; x < allDataMap_.value("EncoderUp").count(); x++){
					scan_->rawData()->beginInsertRows(1, -1);
					scan_->rawData()->setAxisValue(0, insertionIndex_.i(), energyList.at(x));
					for(int y = 0; y < configuration_->detectorConfigurations().count(); y++)
						scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(configuration_->detectorConfigurations().detectorInfoAt(y).name()), AMnDIndex(), allDataMap_.value(configuration_->detectorConfigurations().detectorInfoAt(y).name()).at(x));

					scan_->rawData()->endInsertRows();
					insertionIndex_[0] = insertionIndex_.i()+1;
				}
			}
			else{
				for(int x = 0; x < allDataMap_.value("EncoderUp").count(); x++){
					scan_->rawData()->beginInsertRows(1, 0);
					scan_->rawData()->setAxisValue(0, 0, energyList.at(x));
					for(int y = 0; y < configuration_->detectorConfigurations().count(); y++)
						scan_->rawData()->setValue(AMnDIndex(0), scan_->rawData()->idOfMeasurement(configuration_->detectorConfigurations().detectorInfoAt(y).name()), AMnDIndex(), allDataMap_.value(configuration_->detectorConfigurations().detectorInfoAt(y).name()).at(x));

					scan_->rawData()->endInsertRows();
					insertionIndex_[0] = insertionIndex_.i()+1;
				}
			}
			writeDataToFiles();

			emit finishWritingToFile();

			break;}
		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			break;
		case AMAgnosticDataAPIDefinitions::DataAvailable:{

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

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
	emit requestWriteToFile(0, rank1String);
}

void SGMFastScanActionController::writeDataToFiles(){
	QString rank1String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	AMnDIndex requestIndex = AMnDIndex(0);
	for(int x = 0; x < insertionIndex_.i()-1; x++){
		rank1String.clear();
		rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, requestIndex.i()), 0, 'g', 19));
		AMRawDataSource *oneRawDataSource;
		for(int y = 0; y < scan_->rawDataSourceCount(); y++){
			oneRawDataSource = scan_->rawDataSources()->at(y);
			if(oneRawDataSource->rank() == 1)
				rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(requestIndex), 0, 'g', 19));
		}
		rank1String.append("\n");

		/* Stress testing
		QTime endTime = QTime::currentTime();
		qdebug() << "Time to ready data for writing " << startTime.msecsTo(endTime);
		*/
		emit requestWriteToFile(0, rank1String);
		requestIndex[0] = requestIndex.i()+1;
	}
}

AMAction3* SGMFastScanActionController::createInitializationActions(){

	SGMFastScanParameters *settings = configuration_->currentParameters();

	//////////////////////
	// INITIALIZATION ///
	////////////////////

	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialization", "SGM Fast Actions Initialization"));

	// Grating to required value
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->grating(),settings->scanInfo().start().sgmGrating()));

	// Tracking off:
	// Undulator Tracking to 0
	// Exit Slit Tracking to 0
	AMListAction3 *fastActionsTrackingOff = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Tracking Off", "SGM Fast Actions Tracking Off"), AMListAction3::Parallel);

	fastActionsTrackingOff->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorTracking(), 0));
	fastActionsTrackingOff->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->exitSlitTracking(), 0));

	retVal->addSubAction(fastActionsTrackingOff);
	// End Tracking Off

	// Initial velocity for mono and undulator ... if we haven't done a regular move in a while we can easily wind up with a slow velocity stuck in there
	AMListAction3 *fastActionsInitializeVelocity = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialize Velocity", "SGM Fast Actions Initialize Velocity"), AMListAction3::Parallel);

	fastActionsInitializeVelocity->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorVelocity(), 11811));
	fastActionsInitializeVelocity->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingVelocity(), 10000));

	retVal->addSubAction(fastActionsInitializeVelocity);
	// End initial velocity

	// Initial positions while still moving quickly (energy and undulator)
	// Energy to start position
	// Undulator to start step
	AMListAction3 *fastActionsFastInitialPositions = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Fast Initial Positions", "SGM Fast Actions Fast Initial Positions"), AMListAction3::Parallel);

	fastActionsFastInitialPositions->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->energy(), startEnergy_));
	fastActionsFastInitialPositions->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorStep(), startUndulatorStep_));

	retVal->addSubAction(fastActionsFastInitialPositions);

	// Initialization:
	// Undulator Trigger to 0
	// Undulator Relative Step Storage to 12 (non value)
	// Undulator Relative Step to 12 (non value)
	// Undulator Velocity to undulator velocity
	// Grating Velocity, Velocity Base, Acceleration to motor settings
	AMListAction3 *fastActionsInitializeTriggers = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialize Triggers", "SGM Fast Actions Initialize Triggers"), AMListAction3::Parallel);

	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorFastTracking(), 0));
	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorRelativeStepStorage(), 12));
	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorRelativeStep(), 12));
	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorVelocity(), settings->undulatorVelocity()));
	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingVelocity(), settings->velocity()));
	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingBaseVelocity(), settings->velocityBase()));
	fastActionsInitializeTriggers->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingAcceleration(), settings->acceleration()));

	retVal->addSubAction(fastActionsInitializeTriggers);
	// End Initialization

	// Initial Positions:
	// Exit Slit to median position
	// Continuous Scaler to Off
	AMListAction3 *fastActionsInitialPositions = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initial Positions", "SGM Fast Actions Initial Positions"), AMListAction3::Parallel);

	fastActionsInitialPositions->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->exitSlit(), settings->exitSlitDistance()));
	fastActionsInitialPositions->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));

	retVal->addSubAction(fastActionsInitialPositions);
	// End Initial Positions

	// Scaler and Synchronized Dwell Settings:
	// Dwell time to milliseconds (seconds/1000)
	// Scans per Buffer to 1000
	// Total Scans to 1000
	// Turn off synchronized dwell coordination for the scaler
	AMListAction3 *fastActionsScalerSettings = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Settings", "SGM Fast Actions Scaler Settings"), AMListAction3::Parallel);
	if(QApplication::instance()->arguments().contains("--advanced"))
		fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(settings->fastScanSettings().runSeconds()/1000));
	else
		fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(settings->scalerTime()/1000));
	fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1000));
	fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1000));
	retVal->addSubAction(fastActionsScalerSettings);

	AMListAction3 *fastActionsScalerEnableSettings = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Enable Settings", "SGM Fast Actions Scaler Enable Settings"), AMListAction3::Parallel);
	for(int x = 0; x < 32; x++)
		fastActionsScalerEnableSettings->addSubAction(SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction3(true));

	fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->synchronizedDwellTime()->createEnableAtAction3(0, false));
	retVal->addSubAction(fastActionsScalerEnableSettings);
	// End Scaler Settings

	// Undulator Trigger to 1
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorFastTracking(), 1));

	// Undulator Relative Step Storage to relative step
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorRelativeStepStorage(), undulatorRelativeStep_));

	// Beam on actions
	if(!QApplication::instance()->arguments().contains("--enableTesting"))
		retVal->addSubAction(SGMBeamline::sgm()->createBeamOnActions3());

	///////////////////////////
	// END INITIALIZATION ///
	////////////////////////

	return retVal;
}

AMAction3* SGMFastScanActionController::createCleanupActions(){

	////////////////
	// CLEAN UP ///
	//////////////
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Cleanup", "SGM Fast Actions Cleanup"));

	// Always close the fast shutter when the fast scan is done
	retVal->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->fastShutterVoltage(), 5));

	// Tracking restore:
	// Undulator Tracking to current
	// Exit Slit Tracking to current
	AMListAction3 *fastActionsTrackingRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Tracking Restore", "SGM Fast Actions Tracking Restore"), AMListAction3::Parallel);

	fastActionsTrackingRestore->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->undulatorTracking(),  SGMBeamline::sgm()->undulatorTracking()->value()));
	fastActionsTrackingRestore->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->exitSlitTracking(), SGMBeamline::sgm()->exitSlitTracking()->value()));
	retVal->addSubAction(fastActionsTrackingRestore);
	// End Tracking Off

	if(SGMBeamline::sgm()->undulatorTracking()->withinTolerance(1))
		goodInitialState_ = true;

	// Grating Restore:
	// Grating Velocity, Velocity Base, Acceleration to current settings
	AMListAction3 *fastActionsGratingRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Grating Restore", "SGM Fast Actions Grating Restore"), AMListAction3::Parallel);


	fastActionsGratingRestore->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingVelocity(), SGMBeamline::sgm()->gratingVelocity()->value()));
	fastActionsGratingRestore->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingBaseVelocity(), SGMBeamline::sgm()->gratingBaseVelocity()->value()));
	fastActionsGratingRestore->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingAcceleration(), SGMBeamline::sgm()->gratingAcceleration()->value()));

	retVal->addSubAction(fastActionsGratingRestore);
	// End Grating Restore

	// Scaler and Synchronized Dwell Settings Restore:
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

	fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->synchronizedDwellTime()->createEnableAtAction3(0, SGMBeamline::sgm()->synchronizedDwellTime()->enabledAt(0)));
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

QString SGMFastScanActionController::buildNotes()
{
	QString returnString;

	// Scaler (gets scalar from beamline, iterates through each channel, if channel has a sr570 connected then adds its name, value and the units)
	returnString.append("\nScaler:\n");
	CLSSIS3820Scaler* scaler = SGMBeamline::sgm()->scaler();
	for(int iChannel = 0; iChannel < scaler->channels().count(); iChannel++)
	{
		CLSSIS3820ScalerChannel* currentChannel = scaler->channelAt(iChannel);
		if(currentChannel->currentAmplifier() != 0)
		{
			AMCurrentAmplifier *channelSR570 = currentChannel->currentAmplifier();
			if(channelSR570)
				returnString.append(QString("%1:\t%2 %3\n").arg(currentChannel->customChannelName()).arg(channelSR570->value()).arg(channelSR570->units()));
		}
	}


	// Current presets

	returnString.append(QString("\nPreset:\t%1\n").arg(configuration_->currentParameters()->scanInfo().scanName()));
	returnString.append(QString("Run Time:\t%1s\n").arg(configuration_->runTime()));
	returnString.append(QString("Motor Settings:\t%1\n").arg(configuration_->velocity()));
	returnString.append(QString("Base Line:\t%1\n").arg(configuration_->baseLine()));
	returnString.append(QString("Undulator Velocity:\t%1\n").arg(configuration_->undulatorVelocity()));

	returnString.append(QString("\nStart Energy:\t%1ev\n").arg(configuration_->currentParameters()->scanInfo().start().energy()));
	returnString.append(QString("Start Undulator Step:\t%1\n").arg(configuration_->currentParameters()->scanInfo().start().undulatorStepSetpoint()));
	returnString.append(QString("Start Exit Slit Position:\t%1\n").arg(configuration_->currentParameters()->scanInfo().start().exitSlitDistance()));
	returnString.append(QString("Start Mono Encoder Target:\t%1\n").arg(configuration_->currentParameters()->scanInfo().start().monoEncoderTarget()));

	returnString.append(QString("\nMiddle Energy:\t%1ev\n").arg(configuration_->currentParameters()->scanInfo().middle().energy()));
	returnString.append(QString("Middle Undulator Step:\t%1\n").arg(configuration_->currentParameters()->scanInfo().middle().undulatorStepSetpoint()));
	returnString.append(QString("Middle Exit Slit Position:\t%1\n").arg(configuration_->currentParameters()->scanInfo().middle().exitSlitDistance()));
	returnString.append(QString("Middle Mono Encoder Target:\t%1\n").arg(configuration_->currentParameters()->scanInfo().middle().monoEncoderTarget()));

	returnString.append(QString("\nStart Energy:\t%1ev\n").arg(configuration_->currentParameters()->scanInfo().end().energy()));
	returnString.append(QString("Start Undulator Step:\t%1\n").arg(configuration_->currentParameters()->scanInfo().end().undulatorStepSetpoint()));
	returnString.append(QString("Start Exit Slit Position:\t%1\n").arg(configuration_->currentParameters()->scanInfo().end().exitSlitDistance()));
	returnString.append(QString("Start Mono Encoder Target:\t%1\n").arg(configuration_->currentParameters()->scanInfo().end().monoEncoderTarget()));

	// Add X,Y,Z, Theta manip
	returnString.append(QString("\nManipulator X:\t%1\n").arg(SGMBeamline::sgm()->ssaManipulatorX()->value()));
	returnString.append(QString("Manipulator Y:\t%1\n").arg(SGMBeamline::sgm()->ssaManipulatorY()->value()));
	returnString.append(QString("Manipulator X:\t%1\n").arg(SGMBeamline::sgm()->ssaManipulatorZ()->value()));
	returnString.append(QString("Manipulator Theta:\t%2\n").arg(SGMBeamline::sgm()->ssaManipulatorRot()->value()));

	// Add Exit Slit Gap
	returnString.append(QString("Exit Slit Gap:\t%2\n").arg(configuration_->exitSlitGap()));

	// Add Grating
	returnString.append(QString("Grating:\t%2\n").arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(configuration_->grating())));

	// Add Harmonic
	returnString.append(QString("Harmonic:\t%1\n").arg(SGMBeamlineInfo::sgmInfo()->sgmHarmonicDescription(configuration_->harmonic())));


	return returnString;
}

void SGMFastScanActionController::configureStartEndValues(){
	SGMFastScanParameters *settings = configuration_->currentParameters();

	double currentEnergy = SGMBeamline::sgm()->energy()->value();
	double deltaUp = fabs(settings->energyStart() - currentEnergy);
	double deltaDown = fabs(settings->energyEnd() - currentEnergy);
	if(enableUpDownScanning_ && deltaDown < deltaUp){
		startEnergy_ = settings->energyEnd();
		startUndulatorStep_ = settings->scanInfo().end().undulatorStepSetpoint();
		endEnergy_ = settings->energyStart();
		undulatorRelativeStep_ = settings->scanInfo().start().undulatorStepSetpoint() - settings->scanInfo().end().undulatorStepSetpoint();
	}
	else{
		startEnergy_ = settings->energyStart();
		startUndulatorStep_ = settings->undulatorStartStep();
		endEnergy_ = settings->energyEnd();
		undulatorRelativeStep_ = settings->undulatorRelativeStep();
	}
}
