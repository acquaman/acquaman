#include "AMRegionScanActionController.h"

#include <QFile>
#include <QFileInfo>
#include <QStringBuilder>

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMRegionScanConfigurationConverter.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "beamline/AMBeamline.h"
#include "dataman/AMXASScan.h"
#include "dataman/AMTextStream.h"
#include "dataman/AMSample.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "ui/util/AMMessageBoxWTimeout.h"
#include "dataman/datastore/AMCDFDataStore.h"

AMRegionScanActionController::AMRegionScanActionController(AMRegionScanConfiguration *configuration, QObject *parent)
	: AMScanActionController(configuration, parent)
{
	fileWriterIsBusy_ = false;
	regionsConfiguration_ = configuration;
	currentAxisValueIndex_ = AMnDIndex(0);
	currentAxisValue_ = 0.0;
	newScanAssembler_ = new AMScanActionControllerScanAssembler(this);
	useFeedback_ = false;
}

AMRegionScanActionController::~AMRegionScanActionController()
{
	delete fileWriterThread_;
}

void AMRegionScanActionController::buildScanController()
{
	AMRegionScanConfigurationConverter regionScanConfigurationConverter(newScanAssembler_, regionsConfiguration_, this);

	if(regionScanConfigurationConverter.convert()){

		// Handle some general scan stuff, including setting the default file path.
		scan_->setRunId(AMUser::user()->currentRunId());

		bool has1DDetectors = false;

		for (int i = 0, size = regionsConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

			AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(regionsConfiguration_->detectorConfigurations().at(i));

			if (detector && detector->rank() == 1)
				has1DDetectors = true;
		}

		QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

		// If using just the basic ascii files.
		if (scan_->fileFormat() == "sgm2013XAS" || scan_->fileFormat() == "amRegionAscii2013"){

			scan_->setFilePath(fullPath.filePath() % ".dat");	// relative path and extension (is what the database wants)

			if(has1DDetectors)
				scan_->setAdditionalFilePaths( QStringList() << fullPath.filePath() % "_spectra.dat" );
		}

		// If you want to use the CDF data file format.
		else if (scan_->fileFormat() == "amCDFv1"){

			// Get all the scan axes so they can be added to the new data store.
			QList<AMAxisInfo> scanAxes;

			for (int i = 0, size = scan_->rawData()->scanAxesCount(); i < size; i++)
				scanAxes << scan_->rawData()->scanAxisAt(i);

			scan_->setFilePath(fullPath.filePath() % ".cdf");
			scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));

			// Add all the old scan axes.
			foreach (AMAxisInfo axis, scanAxes)
				scan_->rawData()->addScanAxis(axis);

			flushToDiskTimer_.setInterval(300000);
			connect(this, SIGNAL(started()), &flushToDiskTimer_, SLOT(start()));
			connect(this, SIGNAL(cancelled()), &flushToDiskTimer_, SLOT(stop()));
			connect(this, SIGNAL(paused()), &flushToDiskTimer_, SLOT(stop()));
			connect(this, SIGNAL(resumed()), &flushToDiskTimer_, SLOT(start()));
			connect(this, SIGNAL(failed()), &flushToDiskTimer_, SLOT(stop()));
			connect(this, SIGNAL(finished()), &flushToDiskTimer_, SLOT(stop()));
			connect(&flushToDiskTimer_, SIGNAL(timeout()), this, SLOT(flushCDFDataStoreToDisk()));
			flushToDiskTimer_.start();
		}

		qRegisterMetaType<AMRegionScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
		AMRegionScanActionControllerBasicFileWriter *fileWriter = new AMRegionScanActionControllerBasicFileWriter(AMUserSettings::userDataFolder % fullPath.filePath(), has1DDetectors);
		connect(fileWriter, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
		connect(fileWriter, SIGNAL(fileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError)));
		connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter, SLOT(writeToFile(int,QString)));
		connect(this, SIGNAL(finishWritingToFile()), fileWriter, SLOT(finishWriting()));

		fileWriterThread_ = new QThread();
		connect(this, SIGNAL(finished()), fileWriterThread_, SLOT(quit()));
		connect(this, SIGNAL(cancelled()), fileWriterThread_, SLOT(quit()));
		connect(this, SIGNAL(failed()), fileWriterThread_, SLOT(quit()));
		fileWriter->moveToThread(fileWriterThread_);
		fileWriterThread_->start();

		// Get all the detectors added to the scan.
		for (int i = 0, size = regionsConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

			AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(regionsConfiguration_->detectorConfigurations().at(i));

			if(oneDetector && scan_->rawData()->addMeasurement(AMMeasurementInfo(*oneDetector)))
				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), oneDetector->isVisible(), oneDetector->hiddenFromUsers());
		}

		connect(newScanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onScanningActionsGenerated(AMAction3*)));
		newScanAssembler_->generateActionTree();

		buildScanControllerImplementation();
	}

	else
		AMErrorMon::alert(this, AMREGIONSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION, QString("Error, the %1 Scan Action Controller failed to convert the configuration into a scan action tree. This is a serious problem, please contact the Acquaman developers.").arg(regionsConfiguration_->technique()));
}

bool AMRegionScanActionController::isReadyForDeletion() const
{
	return !fileWriterIsBusy_;
}

void AMRegionScanActionController::flushCDFDataStoreToDisk()
{
	AMCDFDataStore *dataStore = qobject_cast<AMCDFDataStore *>(scan_->rawData());
	if(dataStore && !dataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}

void AMRegionScanActionController::onFileWriterError(AMRegionScanActionControllerBasicFileWriter::FileWriterError error)
{
	qDebug() << "Got a file writer error " << error;

	QString userErrorString;

	switch(error){

	case AMRegionScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, AMREGIONSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, QString("Error, the %1 Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the Acquaman developers.").arg(regionsConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMRegionScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, AMREGIONSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, QString("Error, the %1 Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the Acquaman developers.").arg(regionsConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	default:
		AMErrorMon::alert(this, AMREGIONSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, QString("Error, the %1 Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the Acquaman developers.").arg(regionsConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
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

void AMRegionScanActionController::onFileWriterIsBusy(bool isBusy)
{
	fileWriterIsBusy_ = isBusy;
	emit readyForDeletion(!fileWriterIsBusy_);
}

bool AMRegionScanActionController::event(QEvent *e)
{
	if (e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){

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

			if (!useFeedback_){
				if(message.value("ControlMovementType") == "Absolute")
					currentAxisValue_ = message.value("ControlMovementValue").toDouble();
				else if(message.value("ControlMovementType") == "Relative")
					currentAxisValue_ += message.value("ControlMovementValue").toDouble();
			}

			else{
				qDebug() << "Inside and using feedback instead of setpoint." << message.value("ControlMovementFeedback").toDouble();
				currentAxisValue_ = message.value("ControlMovementFeedback").toDouble();
			}

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

void AMRegionScanActionController::writeHeaderToFile()
{
	AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
	QString separator = "|!|!|";
	QString rank1String;
	rank1String.append("Start Info\n");
	rank1String.append("Version: Acquaman Generic Linear Step 0.1\n");
	rank1String.append(QString("-1%1eV\n").arg(separator));

	for (int x = 0; x < scan_->rawData()->measurementCount(); x++){

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

void AMRegionScanActionController::writeDataToFiles()
{
	QString rank1String;
	QString rank2String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, currentAxisValueIndex_.i())));

	foreach (AMRawDataSource *oneRawDataSource, scan_->rawDataSources()->toList()){

		if (oneRawDataSource->rank() == 1)
			rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(currentAxisValueIndex_)));

		if (oneRawDataSource->rank() == 2){

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
