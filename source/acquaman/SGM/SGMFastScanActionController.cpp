#include "SGMFastScanActionController.h"
#include "dataman/AMFastScan.h"

SGMFastScanActionController::SGMFastScanActionController(SGMFastScanConfiguration *configuration, QObject *parent) :
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

	if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->newTEYDetector()))))
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
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
#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
bool SGMFastScanActionController::startImplementation(){
	AMAgnosticDataMessageHandler *dataMessager = AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions");
	AMAgnosticDataMessageQEventHandler *scanActionMessager = qobject_cast<AMAgnosticDataMessageQEventHandler*>(dataMessager);
	if(scanActionMessager)
		scanActionMessager->addReceiver(this);


	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *hackedFastActions = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions", "SGM Hacked Fast Actions"));

	tmpControl = SGMBeamline::sgm()->undulatorFastTracking();
	AMControlInfo undulatorFastTrackingSetpoint = tmpControl->toInfo();
	undulatorFastTrackingSetpoint.setValue(1);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActions->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorRelativeStepStorage();
	AMControlInfo undulatorRelativeStepStorageSetpoint = tmpControl->toInfo();
	undulatorRelativeStepStorageSetpoint.setValue(16930);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorRelativeStepStorageSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActions->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->undulatorFastTrackingTrigger();
	AMControlInfo undulatorFastTrackingTriggerSetpoint = tmpControl->toInfo();
	undulatorFastTrackingTriggerSetpoint.setValue(1);
	moveActionInfo = new AMControlMoveActionInfo3(undulatorFastTrackingTriggerSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActions->addSubAction(moveAction);

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("SGM Fast Axis"), AMScanAxis::ContinuousMoveAxis));
	hackedFastActions->addSubAction(axisStartAction);

	AMAction3 *readAction;
	readAction = SGMBeamline::sgm()->gratingEncoderDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActions->addSubAction(readAction);

	AMListAction3 *hackedFastActionsEnergyAndScaler = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Energy and Scaler", "SGM Hacked Fast Actions Energy and Scaler"), AMListAction3::Parallel);
	tmpControl = SGMBeamline::sgm()->energy();
	AMControlInfo energySetpoint = tmpControl->toInfo();
	energySetpoint.setValue(320);
	moveActionInfo = new AMControlMoveActionInfo3(energySetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	hackedFastActionsEnergyAndScaler->addSubAction(moveAction);
	//hackedFastActionsEnergyAndScaler->addSubAction(SGMBeamline::sgm()->scaler()->createStartAction3(true));
	hackedFastActionsEnergyAndScaler->addSubAction(SGMBeamline::sgm()->newTEYDetector()->createTriggerAction(AMDetectorDefinitions::ContinuousRead));
	hackedFastActions->addSubAction(hackedFastActionsEnergyAndScaler);

	AMListAction3 *hackedFastActionsReadDetectors = new AMListAction3(new AMListActionInfo3("SGM Hacked Fast Actions Read Detectors", "SGM Hacked Fast Actions Read Detectors"), AMListAction3::Parallel);
	readAction = SGMBeamline::sgm()->newTEYDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	readAction = SGMBeamline::sgm()->newEncoderUpDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	readAction = SGMBeamline::sgm()->newEncoderDownDetector()->createReadAction();
	readAction->setGenerateScanActionMessage(true);
	hackedFastActionsReadDetectors->addSubAction(readAction);
	hackedFastActions->addSubAction(hackedFastActionsReadDetectors);

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
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("NEWTEY"), AMnDIndex(), allDataMap_.value("NEWTEY").at(x));
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
