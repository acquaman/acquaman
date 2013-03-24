#include "SGMXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>

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
		qDebug() << "This configuration has a detector named " << cfg->detectorConfigurations().at(x).name();

		oneDetector = SGMBeamline::sgm()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x));
		newScanAssembler_->addDetector(oneDetector);
		if(oneDetector->rank() == 1)
			has1DDetectors = true;
		if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->exposedDetectorByInfo(cfg->detectorConfigurations().at(x))))))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
	}

	connect(newScanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onActionTreeGenerated(AMAction3*)));
	newScanAssembler_->generateActionTree();

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	QString path = fullPath.path();// just the path, not the file name. Still relative.
	QString file = fullPath.fileName() + ".dat"; // just the file name, now with an extension

	scan_->setFilePath(fullPath.filePath()+".dat");	// relative path and extension (is what the database wants)

	rank1File_ = new QFile(AMUserSettings::userDataFolder+path+"/"+file);
	rank1File_->open(QIODevice::WriteOnly | QIODevice::Text);
	rank1Stream_.setDevice(rank1File_);

	rank2File_ = 0; //NULL
	if(has1DDetectors){
		scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath()+"_spectra.dat" );
		rank2File_ = new QFile(AMUserSettings::userDataFolder+fullPath.filePath()+"_spectra.dat");
		rank2File_->open(QIODevice::WriteOnly | QIODevice::Text);
		rank2Stream_.setDevice(rank2File_);
	}
}

AMAction3* SGMXASScanActionController::actionsTree(){
	return actionTree_;
}

void SGMXASScanActionController::setPointer(QObject *pointer){
	pointer_ = pointer;
}

#include "actions3/AMActionRunner3.h"
void SGMXASScanActionController::onActionTreeGenerated(AMAction3 *actionTree){
	actionTree_ = actionTree;
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
			rank1Stream_ << "Start Info\n";
			rank1Stream_ << "-1" << separator << "eV" << "\n";

			for(int x = 0; x < scan_->rawData()->measurementCount(); x++){
				oneMeasurementInfo = scan_->rawData()->measurementAt(x);
				rank1Stream_ << x << separator << oneMeasurementInfo.rank() << separator << oneMeasurementInfo.name << separator << oneMeasurementInfo.description << separator << oneMeasurementInfo.units;
				if(oneMeasurementInfo.rank() == 1)
					rank1Stream_ << separator << oneMeasurementInfo.axes.at(0).name << separator << oneMeasurementInfo.axes.at(0).size << separator << oneMeasurementInfo.axes.at(0).description << separator << oneMeasurementInfo.axes.at(0).units;
				rank1Stream_ << "\n";
			}
			rank1Stream_ << "End Info\n";
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
	rank1Stream_ << "-1 " << (double)scan_->rawDataSources()->at(0)->axisValue(0, insertionIndex_.i()) << " ";
	rank2Stream_ << "-1 " << (double)scan_->rawDataSources()->at(0)->axisValue(0, insertionIndex_.i()) << "\n";
	AMRawDataSource *oneRawDataSource;
	for(int x = 0; x < scan_->rawDataSourceCount(); x++){
		oneRawDataSource = scan_->rawDataSources()->at(x);
		if(oneRawDataSource->rank() == 1)
			rank1Stream_ << x << " " << (double)oneRawDataSource->value(insertionIndex_) << " ";
		if(oneRawDataSource->rank() == 2){
			int dataSourceSize = oneRawDataSource->size(oneRawDataSource->rank()-1);
			double outputValues[dataSourceSize];
			AMnDIndex startIndex = AMnDIndex(insertionIndex_.i(), 0);
			AMnDIndex endIndex = AMnDIndex(insertionIndex_.i(), dataSourceSize-1);
			oneRawDataSource->values(startIndex, endIndex, outputValues);
			rank2Stream_ << x << " ";
			for(int y = 0; y < dataSourceSize; y++)
				rank2Stream_ << outputValues[y] << " ";
			rank2Stream_ << "\n";
		}
	}
	rank1Stream_ << "-27\n";
}
