#include "SGMXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include "acquaman/AMDetectorTriggerSourceScanOptimizer.h"
#include "acquaman/AMListActionScanOptimizer.h"
#include "acquaman/AMNestedAxisTypeValidator.h"

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

	AMScanAxisRegion firstRegion(cfg->regionStart(0), cfg->regionDelta(0), cfg->regionEnd(0), cfg->regionTime(0), this);
	AMScanAxis *energyAxis = new AMScanAxis(AMScanAxis::StepAxis, firstRegion, this);
	for(int x = 1; x < cfg->regionCount(); x++){
		AMScanAxisRegion anotherRegion(cfg->regionStart(x), cfg->regionDelta(x), cfg->regionEnd(x), cfg->regionTime(x), this);
		energyAxis->appendRegion(anotherRegion);
	}

	newScanAssembler_->appendAxis(SGMBeamline::sgm()->energy(), energyAxis);

	bool has1DDetectors = false;
	AMDetector *oneDetector;
	for(int x = 0; x < cfg->detectorConfigurations().count(); x++){
		oneDetector = SGMBeamline::sgm()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x));
		if(oneDetector){
			newScanAssembler_->addDetector(oneDetector);
			if(oneDetector->rank() == 1)
				has1DDetectors = true;
			if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x))))))
				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
		}
	}

	connect(newScanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onActionTreeGenerated(AMAction3*)));
	newScanAssembler_->generateActionTree();

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	QString path = fullPath.path();// just the path, not the file name. Still relative.
	QString file = fullPath.fileName() + ".dat"; // just the file name, now with an extension

	scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)
	if(has1DDetectors)
		scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath()+"_spectra.dat" );

	fileWriterThread_ = new QThread();
	SGMXASScanActionControllerFileWriter *fileWriter = new SGMXASScanActionControllerFileWriter(AMUserSettings::userDataFolder+fullPath.filePath(), has1DDetectors);
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter, SLOT(writeToFile(int,QString)));
	fileWriter->moveToThread(fileWriterThread_);
	fileWriterThread_->start();
}

AMAction3* SGMXASScanActionController::actionsTree(){
	return actionTree_;
}

#include "actions3/AMActionRunner3.h"
void SGMXASScanActionController::onActionTreeGenerated(AMAction3 *actionTree){
	actionTree_ = actionTree;
	AMDetectorTriggerSourceScanOptimizer triggerOptimizer(actionTree_);
	triggerOptimizer.optimize();
	AMEmptyListScanOptimizer emptyListOptimizer(actionTree_);
	emptyListOptimizer.optimize();
	AMSingleElementListOptimizer singleElementListOptimizer(actionTree_);
	singleElementListOptimizer.optimize();

	AMNestedAxisTypeValidator nestedAxisValidator(actionTree_);
	if(!nestedAxisValidator.validate()){
		qDebug() << "Holy crap, we have an invalid scan action tree";
		return;
	}

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
				SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE,
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
			AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
			QString separator = "|%|%|";
			QString rank1String;
			rank1String.append("Start Info\n");
			rank1String.append(QString("-1%1eV\n").arg(separator));

			for(int x = 0; x < scan_->rawData()->measurementCount(); x++){
				oneMeasurementInfo = scan_->rawData()->measurementAt(x);
				rank1String.append(QString("%1%2%3%2%4%2%5%2%6").arg(x).arg(separator).arg(oneMeasurementInfo.rank()).arg(oneMeasurementInfo.name).arg(oneMeasurementInfo.description).arg(oneMeasurementInfo.units));
				if(oneMeasurementInfo.rank() == 1)
					rank1String.append(QString("%1%2%1%3%1%4%1%5").arg(separator).arg(oneMeasurementInfo.axes.at(0).name).arg(oneMeasurementInfo.axes.at(0).size).arg(oneMeasurementInfo.axes.at(0).description).arg(oneMeasurementInfo.axes.at(0).units));
				rank1String.append("\n");
			}
			rank1String.append("End Info\n");
			emit requestWriteToFile(1, rank1String);
			break;}
		case AMAgnosticDataAPIDefinitions::AxisFinished:{
			scan_->rawData()->endInsertRows();
			writeToFiles();
			setFinished();
			break;}
		case AMAgnosticDataAPIDefinitions::LoopIncremented:
			scan_->rawData()->endInsertRows();
			writeToFiles();
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

void SGMXASScanActionController::writeToFiles(){
	QString rank1String;
	QString rank2String;

	rank1String.append(QString("-1 %1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, insertionIndex_.i())));
	rank2String.append(QString("-1 %1\n").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, insertionIndex_.i())));
	AMRawDataSource *oneRawDataSource;
	for(int x = 0; x < scan_->rawDataSourceCount(); x++){
		oneRawDataSource = scan_->rawDataSources()->at(x);
		if(oneRawDataSource->rank() == 1)
			rank1String.append(QString("%1 %2 ").arg(x).arg((double)oneRawDataSource->value(insertionIndex_)));
		if(oneRawDataSource->rank() == 2){
			int dataSourceSize = oneRawDataSource->size(oneRawDataSource->rank()-1);
			double outputValues[dataSourceSize];
			AMnDIndex startIndex = AMnDIndex(insertionIndex_.i(), 0);
			AMnDIndex endIndex = AMnDIndex(insertionIndex_.i(), dataSourceSize-1);
			oneRawDataSource->values(startIndex, endIndex, outputValues);
			rank2String.append(QString("%1 ").arg(x));
			for(int y = 0; y < dataSourceSize; y++)
				rank2String.append(QString("%1 ").arg(outputValues[y]));
			rank2String.append("\n");
		}
	}
	rank1String.append("-27\n");

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
		QTextStream rank1Stream(rank1File_);
		rank1Stream << textToWrite;
		break;}
	case 2:{
		if(hasRank2Data_){
			QTextStream rank2Stream(rank2File_);
			rank2Stream << textToWrite;
		}
		break;}
	default:
		break;
	}
}
