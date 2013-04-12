#include "SGMXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "dataman/AMTextStream.h"

#include "acquaman/AMXASScanConfigurationConverter.h"

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent) :
	AMScanActionController(cfg, parent)
{
	scan_ = new AMXASScan();
	scan_->setFileFormat("sgm2013XAS");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(cfg);
	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	insertionIndex_ = AMnDIndex(0);

	newScanAssembler_ = new AMScanActionControllerScanAssembler(this);
	AMXASScanConfigurationConverter xasScanConfigurationConverter(newScanAssembler_, cfg, this);

	if(xasScanConfigurationConverter.convert()){
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

		qRegisterMetaType<SGMXASScanActionControllerFileWriter::FileWriterError>("FileWriterError");
		SGMXASScanActionControllerFileWriter *fileWriter = new SGMXASScanActionControllerFileWriter(AMUserSettings::userDataFolder+fullPath.filePath(), has1DDetectors);
		connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter, SLOT(writeToFile(int,QString)));
		connect(fileWriter, SIGNAL(fileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError)), this, SLOT(onFileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError)));
		fileWriter->moveToThread(fileWriterThread_);
		fileWriterThread_->start();
	}
	else
		AMErrorMon::alert(this, SGMXASSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION, "Error, SGM XAS Scan Action Controller failed to convert the configuration into a scan action tree. This is a serious problem, please contact the SGM Acquaman developers.");
}

AMAction3* SGMXASScanActionController::actionsTree(){
	return actionTree_;
}

#include "actions3/AMActionRunner3.h"
#include "application/AMAppControllerSupport.h"
void SGMXASScanActionController::onActionTreeGenerated(AMAction3 *actionTree){
	actionTree_ = actionTree;

	AMAppControllerSupport::optimize(AMAppControllerSupport::principleOptimizersCopy(), actionTree_);
	if(!AMAppControllerSupport::validate(AMAppControllerSupport::principleValidatorsCopy(), actionTree_))
		return;

	AMActionRunner3::scanActionRunner()->addActionToQueue(actionTree_);
}

#include "ui/util/AMMessageBoxWTimeout.h"
void SGMXASScanActionController::onFileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError error){
	qDebug() << "Got a file writer error " << error;
	QString userErrorString;
	switch(error){
	case SGMXASScanActionControllerFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, SGMXASSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, "Error, SGM XAS Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the SGM Acquaman developers.");
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the SGM Acquaman developers immediately.";
		break;
	case SGMXASScanActionControllerFileWriter::CouldNotOpenError:
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

bool SGMXASScanActionController::initializeImplementation(){
	QTimer::singleShot(0, this, SLOT(setInitialized()));
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
			setFinished();
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
	emit requestWriteToFile(1, rank1String);
}

void SGMXASScanActionController::writeDataToFiles(){
	QString rank1String;
	QString rank2String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, insertionIndex_.i())));
	AMRawDataSource *oneRawDataSource;
	for(int x = 0; x < scan_->rawDataSourceCount(); x++){
		oneRawDataSource = scan_->rawDataSources()->at(x);
		if(oneRawDataSource->rank() == 1)
			rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(insertionIndex_)));
		if(oneRawDataSource->rank() == 2){
			int dataSourceSize = oneRawDataSource->size(oneRawDataSource->rank()-1);
			double outputValues[dataSourceSize];
			AMnDIndex startIndex = AMnDIndex(insertionIndex_.i(), 0);
			AMnDIndex endIndex = AMnDIndex(insertionIndex_.i(), dataSourceSize-1);
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

	emit requestWriteToFile(1, rank1String);
	emit requestWriteToFile(2, rank2String);
}

SGMXASScanActionControllerFileWriter::SGMXASScanActionControllerFileWriter(const QString &filePath, bool hasRank2Data, QObject *parent) :
	QObject(parent)
{
	filePath_ = filePath;
	hasRank2Data_ = hasRank2Data;

	QFileInfo rank1FileInfo(filePath+".dat");
	if(rank1FileInfo.exists())
		emitError(SGMXASScanActionControllerFileWriter::AlreadyExistsError);

	rank1File_ = new QFile(rank1FileInfo.filePath());
	if(!rank1File_->open(QIODevice::WriteOnly | QIODevice::Text))
		emitError(SGMXASScanActionControllerFileWriter::CouldNotOpenError);

	rank2File_ = 0; //NULL
	if(hasRank2Data_){
		QFileInfo rank2FileInfo(filePath+"_spectra.dat");
		if(rank2FileInfo.exists())
			emitError(SGMXASScanActionControllerFileWriter::AlreadyExistsError);

		rank2File_ = new QFile(rank2FileInfo.filePath());
		if(!rank2File_->open(QIODevice::WriteOnly | QIODevice::Text))
			emitError(SGMXASScanActionControllerFileWriter::CouldNotOpenError);
	}
}

void SGMXASScanActionControllerFileWriter::writeToFile(int fileRank, const QString &textToWrite){
	switch(fileRank){
	case 1:{
		/* Stress testing
		QTime startTime = QTime::currentTime();
		AMTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		sleep(2);
		QTime endTime = QTime::currentTime();
		qDebug() << "Stream (1) writing differential " << startTime.msecsTo(endTime) << " for size " << textToWrite.size();
		*/

		AMTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		break;}
	case 2:{
		if(hasRank2Data_){
			/* Stress testing
			QTime startTime = QTime::currentTime();
			AMTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
			sleep(2);
			QTime endTime = QTime::currentTime();
			qDebug() << "Stream (2) writing differential " << startTime.msecsTo(endTime) << " for size " << textToWrite.size();
			*/

			AMTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
		}
		break;}
	default:
		break;
	}
}

void SGMXASScanActionControllerFileWriter::emitError(SGMXASScanActionControllerFileWriter::FileWriterError error){
	errorsList_.append(error);
	QTimer::singleShot(0, this, SLOT(emitErrors()));
}

void SGMXASScanActionControllerFileWriter::emitErrors(){
	while(errorsList_.count() > 0)
		emit fileWriterError(errorsList_.takeFirst());
}
