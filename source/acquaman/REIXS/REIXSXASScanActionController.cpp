#include "REIXSXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "dataman/AMXASScan.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "dataman/AMTextStream.h"

#include "acquaman/AMXASScanConfigurationConverter.h"
#include "actions3/AMActionRunner3.h"
#include "application/AMAppControllerSupport.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

#include "dataman/AMSample.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DNormalizationAB.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"

REIXSXASScanActionController::REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent) :
	AMScanActionController(cfg, parent)
{
	fileWriterIsBusy_ = false;
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("sgm2013XAS");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(cfg);
	scan_->setSampleId(REIXSBeamline::bl()->currentSampleId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString rangeString;
	if(configuration_->regionCount())
		rangeString = QString("%1-%2 eV").arg(configuration_->regionStart(0)).arg(configuration_->regionEnd(configuration_->regionCount()-1));

	if(configuration_->namedAutomatically()) {
		int sampleId = REIXSBeamline::bl()->currentSampleId();
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			scan_->setName(QString("%1 %2 %3").arg(sampleName).arg(configuration_->autoScanName()).arg(rangeString));
			scan_->setNumber(scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString("sampleId = %1").arg(sampleId))+1);
		}
		else {
			scan_->setName(QString("%1 %2").arg(configuration_->autoScanName()).arg(rangeString));
			scan_->setNumber(0);
			scan_->setSampleId(-1);
		}
	}
	else {
		scan_->setName(configuration_->userScanName());
		if(scan_->name().isEmpty())
			scan_->setName(QString("%1 %2").arg(configuration_->autoScanName()).arg(rangeString));
		scan_->setNumber(configuration_->scanNumber());
		scan_->setSampleId(configuration_->sampleId());
	}

	insertionIndex_ = AMnDIndex(0);

	newScanAssembler_ = new AMScanActionControllerScanAssembler(this);
	//cfg->xasRegions()->setEnergyControl(REIXSBeamline::bl()->photonSource()->directEnergy());
	AMXASScanConfigurationConverter xasScanConfigurationConverter(newScanAssembler_, cfg, this);
	qDebug() << "\n\nCurrent cfg says default control: " << cfg->regions()->defaultControl()->name();

	AMDetectorInfoSet reixsDetectors;
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("I0")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("TEY")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("TFY")->toInfo());
	reixsDetectors.addDetectorInfo(REIXSBeamline::bl()->exposedDetectorByName("PFY")->toInfo());
	configuration_->setDetectorConfigurations(reixsDetectors);

	if(xasScanConfigurationConverter.convert()){
		AMDetector *oneDetector;
		for(int x = 0; x < cfg->detectorConfigurations().count(); x++){
			oneDetector = REIXSBeamline::bl()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x));
			if(oneDetector){
				if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(REIXSBeamline::bl()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x))))))
					scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
			}
		}

		//
		// DM: add normalized detector channels, if datasources are present
		//
		QList<AMDataSource*> raw1DDataSources;
		for(int i=0; i<scan_->rawDataSources()->count(); i++)
			if(scan_->rawDataSources()->at(i)->rank() == 1)
				raw1DDataSources << scan_->rawDataSources()->at(i);

		int rawTeyIndex = scan_->rawDataSources()->indexOfKey("TEY");
		int rawTfyIndex = scan_->rawDataSources()->indexOfKey("TFY");
		int rawPfyIndex = scan_->rawDataSources()->indexOfKey("PFY");
		int rawI0Index = scan_->rawDataSources()->indexOfKey("I0");

		if(rawTeyIndex != -1 && rawI0Index != -1) {
			AM1DNormalizationAB* teyChannel = new AM1DNormalizationAB("TEYNorm");
			teyChannel->setDescription("Normalized TEY");
			teyChannel->setInputDataSources(raw1DDataSources);
			teyChannel->setDataName("TEY");
			teyChannel->setNormalizationName("I0");

			scan_->addAnalyzedDataSource(teyChannel);
		}

		if(rawTfyIndex != -1 && rawI0Index != -1) {
			AM1DNormalizationAB* tfyChannel = new AM1DNormalizationAB("TFYNorm");
			tfyChannel->setDescription("Normalized TFY");
			tfyChannel->setInputDataSources(raw1DDataSources);
			tfyChannel->setDataName("TFY");
			tfyChannel->setNormalizationName("I0");

			scan_->addAnalyzedDataSource(tfyChannel);
		}

		if(rawPfyIndex != -1 && rawI0Index != -1) {
			AM1DNormalizationAB* pfyChannel = new AM1DNormalizationAB("PFYNorm");
			pfyChannel->setDescription("Normalized PFY");
			pfyChannel->setInputDataSources(raw1DDataSources);
			pfyChannel->setDataName("PFY");
			pfyChannel->setNormalizationName("I0");


			scan_->addAnalyzedDataSource(pfyChannel);
		}
		//
		// DM: added
		//


		connect(newScanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onActionTreeGenerated(AMAction3*)));
		newScanAssembler_->generateActionTree();

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
	else
		AMErrorMon::alert(this, REIXSXASSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION, "Error, REIXS XAS Scan Action Controller failed to convert the configuration into a scan action tree. This is a serious problem, please contact the REIXS Acquaman developers.");

}

AMAction3* REIXSXASScanActionController::actionsTree(){
	return actionTree_;
}

bool REIXSXASScanActionController::isReadyForDeletion() const{
	return !fileWriterIsBusy_;
}

void REIXSXASScanActionController::onInitializationActionsListSucceeded(){
	disconnect(xasActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(xasActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	setInitialized();
}

void REIXSXASScanActionController::onInitializationActionsListFailed(){
	disconnect(xasActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(xasActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));


	connect(xasActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(xasActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(xasActionsCleanupList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void REIXSXASScanActionController::onActionsTreeSucceeded(){
	actionTreeSucceeded_ = true;
	disconnect(actionTree_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	disconnect(actionTree_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));

	connect(xasActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(xasActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(xasActionsCleanupList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void REIXSXASScanActionController::onActionsTreeFailed(){
	disconnect(actionTree_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	disconnect(actionTree_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));

	connect(xasActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(xasActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(xasActionsCleanupList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void REIXSXASScanActionController::onCleanupActionsListSucceeded(){
	disconnect(xasActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(xasActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	if(actionTreeSucceeded_)
		setFinished();
	else
		setFailed();
}

void REIXSXASScanActionController::onCleanupActionsListFailed(){
	disconnect(xasActionsCleanupList_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(xasActionsCleanupList_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));
	setFailed();
}

void REIXSXASScanActionController::onActionTreeGenerated(AMAction3 *actionTree){
	actionTree_ = actionTree;

	AMAppControllerSupport::optimize(AMAppControllerSupport::principleOptimizersCopy(), actionTree_);
	if(!AMAppControllerSupport::validate(AMAppControllerSupport::principleValidatorsCopy(), actionTree_))
		return;
}

#include "ui/util/AMMessageBoxWTimeout.h"
void REIXSXASScanActionController::onFileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError error){
	qDebug() << "Got a file writer error " << error;
	QString userErrorString;
	switch(error){
	case SGMXASScanActionControllerFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, REIXSXASSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, "Error, REIXS XAS Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the REIXS Acquaman developers.");
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the REIXS Acquaman developers immediately.";
		break;
	case SGMXASScanActionControllerFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, REIXSXASSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, "Error, REIXS XAS Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the REIXS Acquaman developers.");
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the REIXS Acquaman developers immediately.";
		break;
	default:
		AMErrorMon::alert(this, REIXSXASSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, "Error, REIXS XAS Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the REIXS Acquaman developers.");
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the REIXS Acquaman developers immediately.";
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

void REIXSXASScanActionController::onFileWriterIsBusy(bool isBusy){
	fileWriterIsBusy_ = isBusy;
	emit readyForDeletion(!fileWriterIsBusy_);
}

bool REIXSXASScanActionController::initializeImplementation(){
	AMAction3 *cleanupActions = createCleanupActions();
	if(qobject_cast<AMListAction3*>(cleanupActions))
		xasActionsCleanupList_ = qobject_cast<AMListAction3*>(cleanupActions);

	AMAction3 *initializationActions = createInitializationActions();
	if(qobject_cast<AMListAction3*>(initializationActions))
		xasActionsInitializationList_ = qobject_cast<AMListAction3*>(initializationActions);

	connect(xasActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	connect(xasActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(xasActionsInitializationList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);

	return true;
}

bool REIXSXASScanActionController::startImplementation(){
	if(REIXSBeamline::bl()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				REIXSXASSCANACTIONCONTROLLER_CANNOT_INTIALIZE,
				"Error, REIXS XAS Scan Action Controller failed to start (REIXS is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning."));
		return false;
	}
	AMActionRunner3::scanActionRunner()->addActionToQueue(actionTree_);
	connect(actionTree_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	connect(actionTree_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));
	return AMScanActionController::startImplementation();
}

void REIXSXASScanActionController::cancelImplementation(){
	AMScanActionController::cancelImplementation();
}

#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
bool REIXSXASScanActionController::event(QEvent *e){
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
			insertionIndex_[0] = insertionIndex_.i()+1;

			break;
		case AMAgnosticDataAPIDefinitions::DataAvailable:{
			if(insertionIndex_.i() >= scan_->rawData()->scanSize(0)){
				scan_->rawData()->beginInsertRows(insertionIndex_.i()-scan_->rawData()->scanSize(0)+1, -1);
				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), currentAxisValue_);
			}

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());
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

void REIXSXASScanActionController::writeHeaderToFile(){
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
	emit requestWriteToFile(1, rank1String);
}

void REIXSXASScanActionController::writeDataToFiles(){
	QString rank1String;
	QString rank2String;

	rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, insertionIndex_.i())));
	AMRawDataSource *oneRawDataSource;
	for(int x = 0; x < scan_->rawDataSourceCount(); x++){
		oneRawDataSource = scan_->rawDataSources()->at(x);
		if(oneRawDataSource->rank() == 1)
			rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(insertionIndex_)));
	}
	rank1String.append("\n");

	emit requestWriteToFile(1, rank1String);
	emit requestWriteToFile(2, rank2String);
}

AMAction3* REIXSXASScanActionController::createInitializationActions(){
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Actions", "REIXS XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 1", "REIXS XAS Initialization Stage 1"), AMListAction3::Parallel);

	if(configuration_->applySlitWidth()){
		tmpControl = REIXSBeamline::bl()->photonSource()->monoSlit();
		AMControlInfo monoSlitSetpoint = tmpControl->toInfo();
		monoSlitSetpoint.setValue(configuration_->slitWidth());
		moveActionInfo = new AMControlMoveActionInfo3(monoSlitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);
	}

	if(configuration_->applyPolarization()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarization();
		AMControlInfo polarizationSetpoint = tmpControl->toInfo();
		polarizationSetpoint.setValue(configuration_->polarization());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);

		/* NEEDS TO BE FIXED TO GO IN SEQUENCE BEFORE POLARIZATION ENUM IS CHOOSEN
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarizationAngle();
		AMControlInfo polarizationAngleSetpoint = tmpControl->toInfo();
		polarizationAngleSetpoint.setValue(configuration_->polarizationAngle());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationAngleSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationStage1->addSubAction(moveAction);
		*/
	}

	initializationStage1->addSubAction(REIXSBeamline::bl()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(REIXSBeamline::bl()->scaler()->createContinuousEnableAction3(false));


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 2", "REIXS XAS Initialization Stage 2"), AMListAction3::Parallel);
	initializationStage2->addSubAction(REIXSBeamline::bl()->scaler()->createScansPerBufferAction3(1));
	initializationStage2->addSubAction(REIXSBeamline::bl()->scaler()->createTotalScansAction3(1));

	AMListAction3 *initializationStage3 = new AMListAction3(new AMListActionInfo3("REIXS XAS Initialization Stage 3", "REIXS XAS Initialization Stage 3"), AMListAction3::Parallel);
	tmpControl = REIXSBeamline::bl()->photonSource()->energy();
	AMControlInfo initialEnergySetpoint = tmpControl->toInfo();
	initialEnergySetpoint.setValue(configuration_->startEnergy());
	moveActionInfo = new AMControlMoveActionInfo3(initialEnergySetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);
	initializationActions->addSubAction(initializationStage3);

	return initializationActions;
}

AMAction3* REIXSXASScanActionController::createCleanupActions(){
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("REIXS XAS Cleanup Actions", "REIXS XAS Cleanup Actions"), AMListAction3::Parallel);

	CLSSIS3820Scaler *scaler = REIXSBeamline::bl()->scaler();
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));

	return cleanupActions;
}
