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
		SGMXASScanActionControllerFileWriter *fileWriter = new SGMXASScanActionControllerFileWriter(AMUserSettings::userDataFolder+fullPath.filePath(), has1DDetectors);
		connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter, SLOT(writeToFile(int,QString)));
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

	emit requestWriteToFile(1, rank1String);
	emit requestWriteToFile(2, rank2String);
}

SGMXASScanActionControllerFileWriter::SGMXASScanActionControllerFileWriter(const QString &filePath, bool hasRank2Data, QObject *parent) :
	QObject(parent)
{
	filePath_ = filePath;
	hasRank2Data_ = hasRank2Data;

	rank1File_ = new QFile(filePath+".dat");
	rank1File_->open(QIODevice::WriteOnly | QIODevice::Text);

	rank2File_ = 0; //NULL
	if(hasRank2Data_){
		rank2File_ = new QFile(filePath+"_spectra.dat");
		rank2File_->open(QIODevice::WriteOnly | QIODevice::Text);
	}
}

void SGMXASScanActionControllerFileWriter::writeToFile(int fileRank, const QString &textToWrite){
	switch(fileRank){
	case 1:{
		//QTextStream rank1Stream(rank1File_);
		AMTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		break;}
	case 2:{
		if(hasRank2Data_){
			//QTextStream rank2Stream(rank2File_);
			AMTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
		}
		break;}
	default:
		break;
	}
}

/*
AMTextStream::AMTextStream() :
	QTextStream()
{
	initializeTextStream();
}

AMTextStream::AMTextStream(QIODevice *device) :
	QTextStream(device)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(FILE *fileHandle, QIODevice::OpenMode openMode) :
	QTextStream(fileHandle, openMode)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(QString *string, QIODevice::OpenMode openMode) :
	QTextStream(string, openMode)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(QByteArray *array, QIODevice::OpenMode openMode) :
	QTextStream(array, openMode)
{
	initializeTextStream();
}

AMTextStream::AMTextStream(const QByteArray &array, QIODevice::OpenMode openMode) :
	QTextStream(array, openMode)
{
	initializeTextStream();
}

void AMTextStream::write(const AMAxisInfo &axisInfo){
	QTextStream::operator <<(axisInfo.name);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.size);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.description);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.units);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(axisInfo.isUniform);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<((double)axisInfo.start);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<((double)axisInfo.increment);
	QTextStream::operator <<(terminator_);
}

void AMTextStream::write(const AMMeasurementInfo &measurementInfo){
	QTextStream::operator <<(measurementInfo.rank());
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(measurementInfo.name);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(measurementInfo.description);
	QTextStream::operator <<(separator_);
	QTextStream::operator <<(measurementInfo.units);
	QTextStream::operator <<(objectMarker_);

	if(measurementInfo.rank() > 0){
		QTextStream::operator <<(separator_);
		for(int x = 0; x < measurementInfo.rank(); x++){
			AMAxisInfo oneAxisInfo = measurementInfo.axes.at(x);
			write(oneAxisInfo);
		}
	}
	QTextStream::operator <<(terminator_);
}

void AMTextStream::read(AMAxisInfo &axisInfo){
	QString fullString;
	QStringList allStrings;
	QChar oneChar;

	while(!fullString.endsWith(terminator_)){
		QTextStream::operator >>(oneChar);
		fullString.append(oneChar);
	}

	fullString.remove(terminator_);
	allStrings = fullString.split(separator_);

	axisInfo.name = allStrings.at(0);
	axisInfo.size = allStrings.at(1).toLong();
	axisInfo.description = allStrings.at(2);
	axisInfo.units = allStrings.at(3);
	if(allStrings.at(4).toInt() == 1)
		axisInfo.isUniform = true;
	else
		axisInfo.isUniform = false;
	axisInfo.start = allStrings.at(5).toDouble();
	axisInfo.increment = allStrings.at(6).toDouble();
}

void AMTextStream::read(AMMeasurementInfo &measurementInfo){
	QString partialString;
	QStringList measurementInfoStrings;
	QChar oneChar;

	while(!partialString.endsWith(objectMarker_)){
		QTextStream::operator >>(oneChar);
		partialString.append(oneChar);
	}

	partialString.remove(objectMarker_);
	measurementInfoStrings = partialString.split(separator_);

	int rank = measurementInfoStrings.at(0).toInt();
	measurementInfo.name = measurementInfoStrings.at(1);
	measurementInfo.description = measurementInfoStrings.at(2);
	measurementInfo.units = measurementInfoStrings.at(3);

	QList<AMAxisInfo> axes;
	for(int x = 0; x < rank; x++){
		AMAxisInfo tempAxisInfo(QString(), 0);
		read(tempAxisInfo);
		axes << tempAxisInfo;
	}
	measurementInfo.axes = axes;

	qDebug() << measurementInfo.name << measurementInfo.description << measurementInfo.units;
	for(int x = 0; x < measurementInfo.axes.count(); x++)
		qDebug() << measurementInfo.axes.at(x).name << measurementInfo.axes.at(x).size << measurementInfo.axes.at(x).description << measurementInfo.axes.at(x).units << measurementInfo.axes.at(x).isUniform << (double)measurementInfo.axes.at(x).start << (double)measurementInfo.axes.at(x).increment;

	partialString.clear();
	while(!partialString.endsWith(terminator_)){
		QTextStream::operator >>(oneChar);
		partialString.append(oneChar);
	}
}

void AMTextStream::initializeTextStream(){
	separator_ = "|%|%|";
	terminator_ = "|@|@|";
	objectMarker_ = "|#|#|";
}
*/
