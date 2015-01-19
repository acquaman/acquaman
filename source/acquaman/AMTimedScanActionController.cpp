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


#include "AMTimedScanActionController.h"

#include <QFile>
#include <QFileInfo>
#include <QStringBuilder>

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
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

AMTimedScanActionController::AMTimedScanActionController(AMTimedRegionScanConfiguration *configuration, QObject *parent)
	: AMScanActionController(configuration, parent)
{
	timedRegionsConfiguration_ = configuration;
	currentAxisValueIndex_ = AMnDIndex(0);
	currentAxisValue_ = 0.0;
	newScanAssembler_ = new AMGenericScanActionControllerAssembler(this);
}

AMTimedScanActionController::~AMTimedScanActionController()
{
	// No need to clean up fileWriterThread, we'll be informed to delete ourself after it is destroyed}
}

void AMTimedScanActionController::buildScanController()
{
	// Handle some general scan stuff, including setting the default file path.
	scan_->setRunId(AMUser::user()->currentRunId());

	bool has1DDetectors = false;

	for (int i = 0, size = timedRegionsConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(timedRegionsConfiguration_->detectorConfigurations().at(i));

		if (detector && detector->rank() == 1)
			has1DDetectors = true;
	}

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	// If using just the basic ascii files.
	if (scan_->fileFormat() == "amRegionAscii2013"){

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

	qRegisterMetaType<AMScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
	fileWriter_ = new AMScanActionControllerBasicFileWriter(AMUserSettings::userDataFolder % fullPath.filePath(), has1DDetectors);
	connect(fileWriter_, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter_, SIGNAL(fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter_, SLOT(writeToFile(int,QString)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter_, SLOT(finishWriting()));

	fileWriterThread_ = new QThread();
	connect(this, SIGNAL(finished()), fileWriterThread_, SLOT(quit()));
	connect(this, SIGNAL(cancelled()), fileWriterThread_, SLOT(quit()));
	connect(this, SIGNAL(failed()), fileWriterThread_, SLOT(quit()));
	fileWriter_->moveToThread(fileWriterThread_);
	fileWriterThread_->start();

	// Get all the detectors added to the scan.
	for (int i = 0, size = timedRegionsConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(timedRegionsConfiguration_->detectorConfigurations().at(i));

		if(oneDetector && scan_->rawData()->addMeasurement(AMMeasurementInfo(*oneDetector)))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), oneDetector->isVisible(), oneDetector->hiddenFromUsers());
	}

	connect(newScanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onScanningActionsGenerated(AMAction3*)));
	newScanAssembler_->generateActionTree();

	buildScanControllerImplementation();
}

//void AMTimedScanActionController::scheduleForDeletion()
//{
//	connect(fileWriterThread_, SIGNAL(destroyed()), this, SLOT(deleteLater()));
//	if(!fileWriterIsBusy_ && fileWriterThread_->isFinished()){
//		qDebug() << "Going to clean up the fast scan controller because we're ready right now";
//		fileWriterThread_->deleteLater();
//	}
//	else if(!fileWriterIsBusy_){
//		qDebug() << "Catch the file writer once its done but before the thread is cleaned up";
//		deleteFileWriterImmediately_ = true;
//	}
//	else{
//		qDebug() << "Cancel or fail I guess, do manual clean up";
//		deleteFileWriterImmediately_ = true;
//		emit finishWritingToFile();
//	}
//}

void AMTimedScanActionController::flushCDFDataStoreToDisk()
{
	AMCDFDataStore *dataStore = qobject_cast<AMCDFDataStore *>(scan_->rawData());
	if(dataStore && !dataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}

void AMTimedScanActionController::onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error)
{
	QString userErrorString;

	switch(error){

	case AMScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, AMTIMEDSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, QString("Error, the %1 Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the Acquaman developers.").arg(timedRegionsConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, AMTIMEDSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, QString("Error, the %1 Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the Acquaman developers.").arg(timedRegionsConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	default:
		AMErrorMon::alert(this, AMTIMEDSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, QString("Error, the %1 Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the Acquaman developers.").arg(timedRegionsConfiguration_->technique()));
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

bool AMTimedScanActionController::event(QEvent *e)
{
	if (e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){

		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){

		case AMAgnosticDataAPIDefinitions::AxisStarted:{
			writeHeaderToFile();
			scanElapsedTime_.start();
			break;}

		case AMAgnosticDataAPIDefinitions::AxisFinished:{
			scan_->rawData()->endInsertRows();
			writeDataToFiles();
			emit finishWritingToFile();
			break;}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:
			scan_->rawData()->endInsertRows();
			writeDataToFiles();
			currentAxisValueIndex_[0] = currentAxisValueIndex_.i()+1;
			currentAxisValue_ = scanElapsedTime_.elapsed()/1000;

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

			// There are no controls in this scan.
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

void AMTimedScanActionController::writeHeaderToFile()
{
	AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
	QString separator = "|!|!|";
	QString rank1String;
	rank1String.append("Start Info\n");
	rank1String.append("Version: Acquaman Generic Linear Step 0.1\n");

	rank1String.append(QString("-1%1").arg(separator));
	QString axisInfoString;
	AMTextStream axisInfoStream(&axisInfoString);
	axisInfoStream.write(scan_->rawData()->scanAxisAt(0));
	rank1String.append(axisInfoString);
	rank1String.append("\n");

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

void AMTimedScanActionController::writeDataToFiles()
{
	QString rank1String;
	QString rank2String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(0, currentAxisValueIndex_.i()), 0, 'g', 19));

	foreach (AMRawDataSource *oneRawDataSource, scan_->rawDataSources()->toList()){

		if (oneRawDataSource->rank() == 1)
			rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(currentAxisValueIndex_), 0, 'g', 19));

		if (oneRawDataSource->rank() == 2){

			int dataSourceSize = oneRawDataSource->size(oneRawDataSource->rank()-1);
			double outputValues[dataSourceSize];
			AMnDIndex startIndex = AMnDIndex(currentAxisValueIndex_.i(), 0);
			AMnDIndex endIndex = AMnDIndex(currentAxisValueIndex_.i(), dataSourceSize-1);
			oneRawDataSource->values(startIndex, endIndex, outputValues);

			for(int y = 0; y < dataSourceSize; y++)
				rank2String.append(QString("%1 ").arg(outputValues[y], 0, 'g', 19));

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
