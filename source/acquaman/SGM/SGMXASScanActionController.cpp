#include "SGMXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "dataman/AMTextStream.h"

#include "acquaman/AMRegionScanConfigurationConverter.h"
#include "actions3/AMActionRunner3.h"
#include "application/AMAppControllerSupport.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

#include "dataman/AMSample.h"

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent) :
	AMScanActionController(cfg, parent)
{
	fileWriterIsBusy_ = false;
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("sgm2013XAS");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(cfg);
	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString scanName;
	QString sampleName;
	if(scan_->sampleId() == -1)
		sampleName = "Unknown Sample";
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

	currentAxisValueIndex_ = AMnDIndex(0);

	newScanAssembler_ = new AMScanActionControllerScanAssembler(this);
	AMRegionScanConfigurationConverter regionScanConfigurationConverter(newScanAssembler_, cfg, this);

	if(regionScanConfigurationConverter.convert()){
		bool has1DDetectors = false;
		AMDetector *oneDetector;
		for(int x = 0; x < cfg->detectorConfigurations().count(); x++){
			oneDetector = SGMBeamline::sgm()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x));
			if(oneDetector){
				if(oneDetector->rank() == 1)
					has1DDetectors = true;
				if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x))))))
					scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
			}
		}

		connect(newScanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onActionTreeGenerated(AMAction3*)));
		newScanAssembler_->generateActionTree();

		QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)
		scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)
		if(has1DDetectors)
			scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath()+"_spectra.dat" );

		fileWriterThread_ = new QThread();

		qRegisterMetaType<AMRegionScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
		AMRegionScanActionControllerBasicFileWriter *fileWriter = new AMRegionScanActionControllerBasicFileWriter(AMUserSettings::userDataFolder+fullPath.filePath(), has1DDetectors);
		connect(fileWriter, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
		connect(fileWriter, SIGNAL(fileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError)));
		connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter, SLOT(writeToFile(int,QString)));
		connect(this, SIGNAL(finishWritingToFile()), fileWriter, SLOT(finishWriting()));
		fileWriter->moveToThread(fileWriterThread_);
		fileWriterThread_->start();
	}
	else
		AMErrorMon::alert(this, SGMXASSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION, "Error, SGM XAS Scan Action Controller failed to convert the configuration into a scan action tree. This is a serious problem, please contact the SGM Acquaman developers.");
}

AMAction3* SGMXASScanActionController::scanningActions(){
	return scanningActions_;
}

bool SGMXASScanActionController::isReadyForDeletion() const{
	return !fileWriterIsBusy_;
}

void SGMXASScanActionController::onInitializationActionsListSucceeded(){
	disconnect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(initializationActions_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	setInitialized();
}

void SGMXASScanActionController::onInitializationActionsListFailed(){
	disconnect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(initializationActions_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void SGMXASScanActionController::onActionsTreeSucceeded(){
	scanningActionsSucceeded_ = true;
	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	disconnect(scanningActions_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));

	connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void SGMXASScanActionController::onActionsTreeFailed(){
	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	disconnect(scanningActions_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));

	connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void SGMXASScanActionController::onCleanupActionsListSucceeded(){
	disconnect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	if(scanningActionsSucceeded_)
		setFinished();
	else
		setFailed();
}

void SGMXASScanActionController::onCleanupActionsListFailed(){
	disconnect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	setFailed();
}

void SGMXASScanActionController::onActionTreeGenerated(AMAction3 *actionTree){
	scanningActions_ = actionTree;

	AMAppControllerSupport::optimize(AMAppControllerSupport::principleOptimizersCopy(), scanningActions_);
	if(!AMAppControllerSupport::validate(AMAppControllerSupport::principleValidatorsCopy(), scanningActions_))
		return;
}

#include "ui/util/AMMessageBoxWTimeout.h"
void SGMXASScanActionController::onFileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError error){
	qDebug() << "Got a file writer error " << error;
	QString userErrorString;
	switch(error){
	case AMRegionScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, SGMXASSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, "Error, SGM XAS Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	case AMRegionScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, SGMXASSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, "Error, SGM XAS Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	default:
		AMErrorMon::alert(this, SGMXASSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, "Error, SGM XAS Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the SGM Acquaman developers.");
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

void SGMXASScanActionController::onFileWriterIsBusy(bool isBusy){
	fileWriterIsBusy_ = isBusy;
	emit readyForDeletion(!fileWriterIsBusy_);
}

bool SGMXASScanActionController::initializeImplementation(){
	AMAction3 *cleanupActions = createCleanupActions();
	if(qobject_cast<AMListAction3*>(cleanupActions))
		cleanupActions_ = qobject_cast<AMListAction3*>(cleanupActions);

	AMAction3 *initializationActions = createInitializationActions();
	if(qobject_cast<AMListAction3*>(initializationActions))
		initializationActions_ = qobject_cast<AMListAction3*>(initializationActions);

	connect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	connect(initializationActions_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(initializationActions_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);

	return true;
}

bool SGMXASScanActionController::startImplementation(){
	if(SGMBeamline::sgm()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASSCANACTIONCONTROLLER_CANNOT_INTIALIZE,
				"Error, SGM XAS Scan Action Controller failed to start (SGM is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning."));
		return false;
	}
	AMActionRunner3::scanActionRunner()->addActionToQueue(scanningActions_);
	connect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	connect(scanningActions_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));
	return AMScanActionController::startImplementation();
}

void SGMXASScanActionController::cancelImplementation(){
	AMScanActionController::cancelImplementation();
}

#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
bool SGMXASScanActionController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){
		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){
		case AMAgnosticDataAPIDefinitions::AxisStarted:{
			writeHeaderToFile();
			break;}
		case AMAgnosticDataAPIDefinitions::AxisFinished:{
			scan_->rawData()->endInsertRows();
			writeDataToFiles();
			emit finishWritingToFile();
//			setFinished();
			break;}
		case AMAgnosticDataAPIDefinitions::LoopIncremented:
			scan_->rawData()->endInsertRows();
			writeDataToFiles();
			currentAxisValueIndex_[0] = currentAxisValueIndex_.i()+1;

			break;
		case AMAgnosticDataAPIDefinitions::DataAvailable:{
			if(currentAxisValueIndex_.i() >= scan_->rawData()->scanSize(0)){
				scan_->rawData()->beginInsertRows(currentAxisValueIndex_.i()-scan_->rawData()->scanSize(0)+1, -1);
				scan_->rawData()->setAxisValue(0, currentAxisValueIndex_.i(), currentAxisValue_);
			}

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

			scan_->rawData()->setValue(currentAxisValueIndex_, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());
			break;}
		case AMAgnosticDataAPIDefinitions::ControlMoved:
			if(message.value("ControlMovementType") == "Absolute")
				currentAxisValue_ = message.value("ControlMovementValue").toDouble();
			else if(message.value("ControlMovementType") == "Relative")
				currentAxisValue_ += message.value("ControlMovementValue").toDouble();

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

void SGMXASScanActionController::writeHeaderToFile(){
	AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
	QString separator = "|!|!|";
	QString rank1String;
	rank1String.append("Start Info\n");
	rank1String.append("Version: SGM Generic 0.1\n");
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
	rank1String.append("End Info\n");
	emit requestWriteToFile(0, rank1String);
}

void SGMXASScanActionController::writeDataToFiles(){
	QString rank1String;
	QString rank2String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, currentAxisValueIndex_.i())));
	AMRawDataSource *oneRawDataSource;
	for(int x = 0; x < scan_->rawDataSourceCount(); x++){
		oneRawDataSource = scan_->rawDataSources()->at(x);
		if(oneRawDataSource->rank() == 1)
			rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(currentAxisValueIndex_)));
		if(oneRawDataSource->rank() == 2){
			int dataSourceSize = oneRawDataSource->size(oneRawDataSource->rank()-1);
			double outputValues[dataSourceSize];
			AMnDIndex startIndex = AMnDIndex(currentAxisValueIndex_.i(), 0);
			AMnDIndex endIndex = AMnDIndex(currentAxisValueIndex_.i(), dataSourceSize-1);
			oneRawDataSource->values(startIndex, endIndex, outputValues);
			for(int y = 0; y < dataSourceSize; y++)
				rank2String.append(QString("%1 ").arg(outputValues[y]));
			rank2String.append("\n");
		}
	}
	rank1String.append("\n");

	/* Stress testing
	QTime endTime = QTime::currentTime();
	qDebug() << "Time to ready data for writing " << startTime.msecsTo(endTime);
	*/

	emit requestWriteToFile(0, rank1String);
	emit requestWriteToFile(1, rank2String);
}

AMAction3* SGMXASScanActionController::createInitializationActions(){
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Actions", "SGM XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 1", "SGM XAS Initialization Stage 1"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->grating();
	AMControlInfo gratingSetpoint = tmpControl->toInfo();
	gratingSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(gratingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->exitSlitGap();
	AMControlInfo exitSlitGapSetpoint = tmpControl->toInfo();
	exitSlitGapSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitGapSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	QList<QString> allKeys;
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
		if(!allKeys.contains(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey()))
			allKeys.append(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey());

	for(int x = 0; x < syncDwell->elementCount(); x++){
		if(syncDwell->enabledAt(x) && !allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(false));
		else if(!syncDwell->enabledAt(x) && allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(true));
	}

	CLSAmptekSDD123DetectorNew *amptek1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(amptek1)
		initializationStage1->addSubAction(amptek1->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD1()->name())));
	CLSAmptekSDD123DetectorNew *amptek2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(amptek2)
		initializationStage1->addSubAction(amptek2->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD2()->name())));
	CLSAmptekSDD123DetectorNew *amptek3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(amptek3)
		initializationStage1->addSubAction(amptek3->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD3()->name())));
	CLSAmptekSDD123DetectorNew *amptek4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(amptek4)
		initializationStage1->addSubAction(amptek4->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD4()->name())));

	initializationStage1->addSubAction(SGMBeamline::sgm()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 2", "SGM XAS Initialization Stage 2"), AMListAction3::Parallel);
	initializationStage2->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1));
	initializationStage2->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1));


	AMListAction3 *initializationStage3 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 3", "SGM XAS Initialization Stage 3"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->undulatorTracking();
	AMControlInfo undulatorTrackingSetpoint = tmpControl->toInfo();
	undulatorTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->exitSlitTracking();
	AMControlInfo exitSlitTrackingSetpoint = tmpControl->toInfo();
	exitSlitTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->monoTracking();
	AMControlInfo monoTrackingSetpoint = tmpControl->toInfo();
	monoTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(monoTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	//really? here?
	tmpControl = SGMBeamline::sgm()->harmonic();
	AMControlInfo harmonicSetpoint = tmpControl->toInfo();
	harmonicSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(harmonicSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	AMListAction3 *initializationStage4 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 4", "SGM XAS Initialization Stage 4"), AMListAction3::Parallel);
	initializationStage4->addSubAction(SGMBeamline::sgm()->createBeamOnActions3());

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);
	initializationActions->addSubAction(initializationStage3);
	initializationActions->addSubAction(initializationStage4);

	return initializationActions;
}

AMAction3* SGMXASScanActionController::createCleanupActions(){
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("SGM XAS Cleanup Actions", "SGM XAS Cleanup Actions"), AMListAction3::Parallel);

	CLSSIS3820Scaler *scaler = SGMBeamline::sgm()->scaler();
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	for(int x = 0; x < syncDwell->elementCount(); x++)
		cleanupActions->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
	cleanupActions->addSubAction(syncDwell->createMasterTimeAction3(syncDwell->time()));

	CLSAmptekSDD123DetectorNew *amptek1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(amptek1)
		cleanupActions->addSubAction(amptek1->createEnableAction3(amptek1->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(amptek2)
		cleanupActions->addSubAction(amptek2->createEnableAction3(amptek2->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(amptek3)
		cleanupActions->addSubAction(amptek3->createEnableAction3(amptek3->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(amptek4)
		cleanupActions->addSubAction(amptek4->createEnableAction3(amptek4->isEnabled()));

	AMControlInfo fastShutterSetpoint = SGMBeamline::sgm()->fastShutterVoltage()->toInfo();
	AMControlMoveAction3 *fastShutterAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(fastShutterSetpoint), SGMBeamline::sgm()->fastShutterVoltage());
	cleanupActions->addSubAction(fastShutterAction);

	return cleanupActions;
}
