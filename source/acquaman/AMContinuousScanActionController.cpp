#include "AMContinuousScanActionController.h"

#include <stdint.h>

#include <QFile>
#include <QFileInfo>
#include <QStringBuilder>

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/AMBeamline.h"

#include "dataman/datastore/AMCDFDataStore.h"
#include "ui/util/AMMessageBoxWTimeout.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"

#include "acquaman/AMContinuousScanActionControllerAMDSClientDataRequestFileWriter.h"

#include <QDebug>

AMContinuousScanActionController::AMContinuousScanActionController(AMContinuousScanConfiguration *configuration, QObject *parent)
	: AMScanActionController(configuration, parent)
{
	continuousConfiguration_ = configuration;
	insertionIndex_ = AMnDIndex(continuousConfiguration_->scanAxes().size(), AMnDIndex::DoInit, 0);

	connect(this, SIGNAL(started()), this, SLOT(onScanningActionsStarted()));
}

AMContinuousScanActionController::~AMContinuousScanActionController()
{

}

void AMContinuousScanActionController::buildScanController()
{
	// Build the scan assembler.
	createScanAssembler();
	// Create the axis order map for higher dimensional scans.
	createAxisOrderMap();

	// Setup all the axes.
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		continuousConfiguration_->scanAxisAt(i)->setName(scan_->rawData()->scanAxisAt(i).name);

		for (int j = 0, regionCount = continuousConfiguration_->scanAxisAt(i)->regionCount(); j < regionCount; j++)
			continuousConfiguration_->scanAxisAt(i)->regionAt(j)->setName(QString("%1 %2 %3").arg(scan_->rawData()->scanAxisAt(i).name).arg("region").arg(j+1));
	}

	// Configure the scan assemblers axes.
	qDebug() << "Scan axes count = " << scan_->rawData()->scanAxesCount();
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		int actualAxis = axisOrderMap_.value(scan_->rawData()->scanAxisAt(i).name);
		scanAssembler_->insertAxis(i, AMBeamline::bl()->exposedControlByInfo(continuousConfiguration_->axisControlInfoAt(actualAxis)), continuousConfiguration_->scanAxisAt(actualAxis));
		qDebug() << "Adding axis " << continuousConfiguration_->axisControlInfoAt(i).name();
		qDebug() << "Start: " << double(continuousConfiguration_->scanAxisAt(i)->axisStart());
	}

	// Add all the detectors.
	for (int i = 0, size = continuousConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(continuousConfiguration_->detectorConfigurations().at(i));

		if (oneDetector && !scanAssembler_->addDetector(oneDetector)){

			AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return;
		}
	}

	// Handle some general scan stuff, including setting the default file path.
	scan_->setRunId(AMUser::user()->currentRunId());

	bool has1DDetectors = false;

	for (int i = 0, size = continuousConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(continuousConfiguration_->detectorConfigurations().at(i));

		if (detector && detector->rank() == 1)
			has1DDetectors = true;
	}

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	// If you want to use the CDF data file format.
	if (scan_->fileFormat() == "amCDFv1"){

		// Get all the scan axes so they can be added to the new data store.
		QList<AMAxisInfo> scanAxes;

		for (int i = 0, size = scan_->rawData()->scanAxesCount(); i < size; i++)
			scanAxes << scan_->rawData()->scanAxisAt(i);

		scan_->setFilePath(fullPath.filePath() % ".cdf");
		scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));

		// Add all the old scan axes.
		foreach (AMAxisInfo axis, scanAxes)
			scan_->rawData()->addScanAxis(axis);

		connect(this, SIGNAL(started()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(cancelled()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(failed()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(finished()), this, SLOT(flushCDFDataStoreToDisk()));
	}

	scan_->setAdditionalFilePaths( QStringList() << QString("%1.cdr").arg(fullPath.filePath()));

	qRegisterMetaType<AMScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
	qRegisterMetaType<AMDSClientDataRequestMap>("AMDSClientDataRequestMap");
	fileWriter_ = new AMContinuousScanActionControllerAMDSClientDataRequestFileWriter(QString("%1%2").arg(AMUserSettings::userDataFolder).arg(fullPath.filePath()));
	connect(fileWriter_, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter_, SIGNAL(fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(const AMDSClientDataRequestMap &)), fileWriter_, SLOT(writeToFile(const AMDSClientDataRequestMap &)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter_, SLOT(finishWriting()));

	fileWriterThread_ = new QThread();
	fileWriter_->moveToThread(fileWriterThread_);
	fileWriterThread_->start();

	// Get all the detectors added to the scan.
	for (int i = 0, size = continuousConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(continuousConfiguration_->detectorConfigurations().at(i));

		if(oneDetector && scan_->rawData()->addMeasurement(AMMeasurementInfo(*oneDetector)))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), oneDetector->isVisible(), oneDetector->hiddenFromUsers());
	}

	connect(scanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onScanningActionsGenerated(AMAction3*)));

	if (scanAssembler_->generateActionTree())
		buildScanControllerImplementation();

	else
		setFailed();
}

void AMContinuousScanActionController::onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error)
{
	QString userErrorString;

	switch(error){

	case AMScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, QString("Error, the %1 Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the Acquaman developers.").arg(continuousConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, QString("Error, the %1 Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the Acquaman developers.").arg(continuousConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::FailedToWriteFile:
		AMErrorMon::error(this, AMCONTINUOUSSCANACTIONCONTROLLER_FAILE_TO_WRITE_FILE, QString("Error, the %1 Scan Action Controller failed to write your data. This is a serious problem, please contact the Acquaman developers.").arg(continuousConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to write to the desired file (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	default:
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, QString("Error, the %1 Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the Acquaman developers.").arg(continuousConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;
	}

	setFailed();

	AMMessageBoxWTimeout::showMessageWTimeout("Sorry! Your scan has been cancelled because a file writing error occured.",
											  "Acquaman saves files for long term storage, but some sort of error occured for your scan.",
											  userErrorString);
}

void AMContinuousScanActionController::flushCDFDataStoreToDisk()
{
	AMCDFDataStore *dataStore = qobject_cast<AMCDFDataStore *>(scan_->rawData());
	if(dataStore && !dataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 38, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}

void AMContinuousScanActionController::onScanningActionsStarted()
{
	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onScanningActionsSucceeded()));
}

bool AMContinuousScanActionController::event(QEvent *e)
{
	if (e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){

		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){

		case AMAgnosticDataAPIDefinitions::AxisStarted:{

			break;}

		case AMAgnosticDataAPIDefinitions::AxisFinished:{

			onAxisFinished();
			flushCDFDataStoreToDisk();

			break;
		}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			// Not relevant for continuous scans.
			break;

		case AMAgnosticDataAPIDefinitions::DataAvailable:{

			AMAgnosticDataAPIDataAvailableMessage *dataAvailableMessage = static_cast<AMAgnosticDataAPIDataAvailableMessage*>(&message);
			fillDataMaps(dataAvailableMessage);
			flushCDFDataStoreToDisk();

			break;}

		case AMAgnosticDataAPIDefinitions::ControlMoved:

			// Not relevant for continuous scans.
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

void AMContinuousScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMGenericScanActionControllerAssembler(continuousConfiguration_->automaticDirectionAssessment(), continuousConfiguration_->direction(), this);
}

void AMContinuousScanActionController::createAxisOrderMap()
{
	if (scan_->scanRank() == 1){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 0);
	}

	else if (scan_->scanRank() == 2){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
	}
}

void AMContinuousScanActionController::onAxisFinished()
{
}

void AMContinuousScanActionController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
{
	Q_UNUSED(message)
}

bool AMContinuousScanActionController::cleanupClientDataRequests()
{
	clientDataRequestMap_.clear();

	return true;
}


