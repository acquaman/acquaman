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


#include "AMStepScanActionController.h"

#include <QFile>
#include <QFileInfo>
#include <QStringBuilder>

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/AMBeamline.h"
#include "dataman/AMXASScan.h"
#include "dataman/AMTextStream.h"
#include "dataman/AMSample.h"
#include "dataman/datastore/AMCDFDataStore.h"
#include "ui/util/AMMessageBoxWTimeout.h"

AMStepScanActionController::AMStepScanActionController(AMStepScanConfiguration *configuration, QObject *parent)
	: AMScanActionController(configuration, parent)
{
	stepConfiguration_ = configuration;

	useFeedback_ = false;
	stoppingAtEndOfLine_ = false;
	axisStackCounter_ = 0;
}

AMStepScanActionController::~AMStepScanActionController()
{
	// No need to clean up fileWriterThread, we'll be informed to delete ourself after it is destroyed
}

void AMStepScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMGenericScanActionControllerAssembler(this);
}

void AMStepScanActionController::createAxisOrderMap()
{
	for (int i = 0, size = stepConfiguration_->scanAxes().size(); i < size; i++)
		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(i).name, i);
}

void AMStepScanActionController::buildScanController()
{
	// Build the scan assembler.
	createScanAssembler();
	// Create the axis order map for higher dimensional scans.
	createAxisOrderMap();

	currentAxisValueIndex_ = AMnDIndex(scan_->rawData()->scanAxesCount(), AMnDIndex::DoInit, 0);

	// Setup all the axes.
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		currentAxisValues_ << 0;
		stepConfiguration_->scanAxisAt(i)->setName(scan_->rawData()->scanAxisAt(i).name);

		for (int j = 0, regionCount = stepConfiguration_->scanAxisAt(i)->regionCount(); j < regionCount; j++)
			stepConfiguration_->scanAxisAt(i)->regionAt(j)->setName(QString("%1 %2 %3").arg(scan_->rawData()->scanAxisAt(i).name).arg("region").arg(j+1));
	}

	// Configure the scan assemblers axes.
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		int actualAxis = axisOrderMap_.value(scan_->rawData()->scanAxisAt(i).name);
		scanAssembler_->insertAxis(i, AMBeamline::bl()->exposedControlByInfo(stepConfiguration_->axisControlInfos().at(actualAxis)), stepConfiguration_->scanAxisAt(actualAxis));
	}

	// Add all the detectors.
	for (int i = 0, size = stepConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(stepConfiguration_->detectorConfigurations().at(i));

		if (oneDetector && !scanAssembler_->addDetector(oneDetector)){

			AMErrorMon::alert(this, AMSTEPSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return;
		}
	}

	// Handle some general scan stuff, including setting the default file path.
	scan_->setRunId(AMUser::user()->currentRunId());

	bool has1DDetectors = false;

	for (int i = 0, size = stepConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(stepConfiguration_->detectorConfigurations().at(i));

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

		connect(this, SIGNAL(started()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(cancelled()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(failed()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(finished()), this, SLOT(flushCDFDataStoreToDisk()));
	}

	qRegisterMetaType<AMScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
	fileWriter_ = new AMScanActionControllerBasicFileWriter(AMUserSettings::userDataFolder % fullPath.filePath(), has1DDetectors);
	connect(fileWriter_, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter_, SIGNAL(fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter_, SLOT(writeToFile(int,QString)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter_, SLOT(finishWriting()));

	fileWriterThread_ = new QThread();
	fileWriter_->moveToThread(fileWriterThread_);
	fileWriterThread_->start();

	// Get all the detectors added to the scan.
	for (int i = 0, size = stepConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(stepConfiguration_->detectorConfigurations().at(i));

		if(oneDetector && scan_->rawData()->addMeasurement(AMMeasurementInfo(*oneDetector)))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), oneDetector->isVisible(), oneDetector->hiddenFromUsers());
	}

	if (scan_->rawData()->scanAxesCount() >= 2)
		prefillScanPoints();

	connect(scanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onScanningActionsGenerated(AMAction3*)));

	if (scanAssembler_->generateActionTree())
		buildScanControllerImplementation();

	else
		setFailed();
}

void AMStepScanActionController::flushCDFDataStoreToDisk()
{
	AMCDFDataStore *dataStore = qobject_cast<AMCDFDataStore *>(scan_->rawData());
	if(dataStore && !dataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}

void AMStepScanActionController::onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error)
{
	QString userErrorString;

	switch(error){

	case AMScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, AMSTEPSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, QString("Error, the %1 Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the Acquaman developers.").arg(stepConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, AMSTEPSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, QString("Error, the %1 Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the Acquaman developers.").arg(stepConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::FailedToWriteFile:
		AMErrorMon::error(this, AMSTEPSCANACTIONCONTROLLER_FAILE_TO_WRITE_FILE, QString("Error, the %1 Scan Action Controller failed to write your data. This is a serious problem, please contact the Acquaman developers.").arg(stepConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to write to the desired file (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	default:
		AMErrorMon::alert(this, AMSTEPSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, QString("Error, the %1 Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the Acquaman developers.").arg(stepConfiguration_->technique()));
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;
	}

	setFailed();

	AMMessageBoxWTimeout::showMessageWTimeout("Sorry! Your scan has been cancelled because a file writing error occured.",
											  "Acquaman saves files for long term storage, but some sort of error occured for your scan.",
											  userErrorString);
}

bool AMStepScanActionController::event(QEvent *e)
{
	if (e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){

		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){

		case AMAgnosticDataAPIDefinitions::AxisStarted:{

			axisStackCounter_++;

			if (message.uniqueID().contains(scan_->rawData()->scanAxisAt(0).name))
				writeHeaderToFile();

			for (int i = 0, size = scan_->rawData()->scanAxesCount(); i < size; i++)
				if (message.uniqueID().contains(scan_->rawData()->scanAxisAt(i).name))
					currentAxisValueIndex_[i] = 0;

			break;}

		case AMAgnosticDataAPIDefinitions::AxisFinished:{

			if (scan_->scanRank() == 0)
				writeDataToFiles();

			if (scan_->rawData()->scanAxesCount() == 1)
				scan_->rawData()->endInsertRows();

			// This should be safe and fine regardless of if the CDF data store is being used or not.
			flushCDFDataStoreToDisk();

			if (isStopping() && stoppingAtEndOfLine_){

				disconnect(AMActionRunner3::scanActionRunner()->currentAction(), SIGNAL(cancelled()), this, SLOT(onScanningActionsCancelled()));
				connect(AMActionRunner3::scanActionRunner()->currentAction(), SIGNAL(cancelled()), this, SLOT(onScanningActionsSucceeded()));
				AMActionRunner3::scanActionRunner()->cancelCurrentAction();
				emit finishWritingToFile();
			}

			else {

				axisStackCounter_--;

				if (axisStackCounter_ == 0)
					emit finishWritingToFile();
			}

			break;}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			if (scan_->rawData()->scanAxesCount() == 1)
				scan_->rawData()->endInsertRows();

			writeDataToFiles();

			for (int i = 0, size = scan_->rawData()->scanAxesCount(); i < size; i++)
				if (message.uniqueID().contains(scan_->rawData()->scanAxisAt(i).name))
					currentAxisValueIndex_[i] = currentAxisValueIndex_.at(i)+1;

			if (isStopping() && !stoppingAtEndOfLine_){

				disconnect(AMActionRunner3::scanActionRunner()->currentAction(), SIGNAL(cancelled()), this, SLOT(onScanningActionsCancelled()));
				connect(AMActionRunner3::scanActionRunner()->currentAction(), SIGNAL(cancelled()), this, SLOT(onScanningActionsSucceeded()));
				AMActionRunner3::scanActionRunner()->cancelCurrentAction();
				emit finishWritingToFile();
			}

			break;

		case AMAgnosticDataAPIDefinitions::DataAvailable:{

			if(scan_->rawData()->scanAxesCount() == 1 && currentAxisValueIndex_.i() >= scan_->rawData()->scanSize(0)){

				bool positiveStep = (double(stepConfiguration_->scanAxisAt(0)->regionAt(0)->regionStep()) > 0);

				if (positiveStep){

					scan_->rawData()->beginInsertRows(currentAxisValueIndex_.i()-scan_->rawData()->scanSize(0)+1, -1);
					scan_->rawData()->setAxisValue(0, currentAxisValueIndex_.i(), currentAxisValues_.at(0));
				}

				else {

					scan_->rawData()->beginInsertRows(1, 0);
					scan_->rawData()->setAxisValue(0, 0, currentAxisValues_.at(0));

				}
			}

			QVariantList detectorDataValues = message.value("DetectorData").toList();
			QVector<double> localDetectorData = QVector<double>(detectorDataValues.size());

			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData[x] = detectorDataValues.at(x).toDouble();

			if (scan_->rawData()->scanAxesCount() == 1){

				bool positiveStep = (double(stepConfiguration_->scanAxisAt(0)->regionAt(0)->regionStep()) > 0);

				if (positiveStep)
					scan_->rawData()->setValue(currentAxisValueIndex_, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());

				else
					scan_->rawData()->setValue(0, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());
			}

			else
				scan_->rawData()->setValue(currentAxisValueIndex_, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());

			// This should be safe and fine regardless of if the CDF data store is being used or not.
			flushCDFDataStoreToDisk();

			break;}

		case AMAgnosticDataAPIDefinitions::ControlMoved:

			for (int i = 0, size = stepConfiguration_->axisControlInfos().count(); i < size; i++){

				if (message.uniqueID().contains(stepConfiguration_->axisControlInfos().at(i).name())){

					if (!useFeedback_){
						if(message.value("ControlMovementType") == "Absolute")
							currentAxisValues_[i] = message.value("ControlMovementValue").toDouble();
						else if(message.value("ControlMovementType") == "Relative")
							currentAxisValues_[i] += message.value("ControlMovementValue").toDouble();
					}

					else
						currentAxisValues_[i] = message.value("ControlMovementFeedback").toDouble();
				}
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

void AMStepScanActionController::writeHeaderToFile()
{
	AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
	QString separator = "|!|!|";
	QString rank1String;
	rank1String.append("Start Info\n");
	rank1String.append("Version: Acquaman Generic Linear Step 0.1\n");

	for (int i = 0, scanRank = scan_->rawData()->scanAxesCount(); i < scanRank; i++){

		rank1String.append(QString("-%1%2").arg(scanRank-i).arg(separator));
		QString axisInfoString;
		AMTextStream axisInfoStream(&axisInfoString);
		axisInfoStream.write(scan_->rawData()->scanAxisAt(i));
		rank1String.append(axisInfoString);
		rank1String.append("\n");
	}

	for (int i = 0; i < scan_->rawData()->measurementCount(); i++){

		oneMeasurementInfo = scan_->rawData()->measurementAt(i);

		rank1String.append(QString("%1%2").arg(i).arg(separator));
		QString measurementInfoString;
		AMTextStream measurementInfoStream(&measurementInfoString);
		measurementInfoStream.write(oneMeasurementInfo);
		rank1String.append(measurementInfoString);
		rank1String.append("\n");
	}

	rank1String.append("End Info\n");
	emit requestWriteToFile(0, rank1String);
}

void AMStepScanActionController::writeDataToFiles()
{
	QString rank1String;
	QString rank2String;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	int scanRank = scan_->rawData()->scanRank();

	for (int i = 0; i < scanRank; i++)
		rank1String.append(QString("%1 ").arg((double)scan_->rawDataSources()->at(0)->axisValue(i, currentAxisValueIndex_.at(i)), 0, 'g', 19));

	foreach (AMRawDataSource *oneRawDataSource, scan_->rawDataSources()->toList()){

		if ((oneRawDataSource->rank() - scanRank) == 0)
			rank1String.append(QString("%1 ").arg((double)oneRawDataSource->value(currentAxisValueIndex_), 0, 'g', 19));

		if ((oneRawDataSource->rank() - scanRank) == 1){

			int dataSourceSize = oneRawDataSource->size(oneRawDataSource->rank()-1);
			QVector<double> outputValues = QVector<double>(dataSourceSize);
			AMnDIndex startIndex = AMnDIndex(currentAxisValueIndex_.i(), 0);
			AMnDIndex endIndex = AMnDIndex(currentAxisValueIndex_.i(), dataSourceSize-1);
			oneRawDataSource->values(startIndex, endIndex, outputValues.data());

			for(int y = 0; y < dataSourceSize; y++)
				rank2String.append(QString("%1 ").arg(outputValues.at(y), 0, 'g', 19));

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

void AMStepScanActionController::prefillScanPoints()
{
	int scanRank = scan_->rawData()->scanRank();

	if (scanRank >= 2){

		AMnDIndex insertIndex;
		QVector<double> starts;
		QVector<double> steps;
		QVector<double> ends;
		QVector<int> axisSizes;

		// This assumes only one region per axis for now.
		foreach (AMScanAxis *axis, stepConfiguration_->scanAxes()){

			AMScanAxisRegion *region = axis->regionAt(0);
			starts << double(region->regionStart());
			steps << double(region->regionStep());
			ends << double(region->regionEnd());
			axisSizes << (int(round((double(region->regionEnd())-double(region->regionStart()))/double(region->regionStep()))) + 1);
		}

		scan_->rawData()->beginInsertRows(axisSizes.at(0), -1);

		if (scanRank == 2){

			for (int j = 0; j < axisSizes.at(1); j++){

				for (int i = 0; i < axisSizes.at(0); i++){

					insertIndex = AMnDIndex(i, j);
					scan_->rawData()->setAxisValue(0, insertIndex.i(), starts.at(0) + i*steps.at(0));
					scan_->rawData()->setAxisValue(1, insertIndex.j(), starts.at(1) + j*steps.at(1));

					for (int di = 0, dataSourceCount = scan_->rawDataSourceCount(); di < dataSourceCount; di++){

						if ((scan_->rawDataSources()->at(di)->rank() - scanRank) == 0)
							scan_->rawData()->setValue(insertIndex, di, AMnDIndex(), -1);

						else if ((scan_->rawDataSources()->at(di)->rank() - scanRank) == 1){

							QVector<double> data = QVector<double>(scan_->rawDataSources()->at(di)->size(scan_->rawDataSources()->at(di)->rank()-1), -1);
							scan_->rawData()->setValue(insertIndex, di, data.constData());
						}
					}
				}
			}
		}

		else if (scanRank == 3){

			for (int k = 0; k < axisSizes.at(2); k++){

				for (int j = 0; j < axisSizes.at(1); j++){

					for (int i = 0; i < axisSizes.at(0); i++){

						insertIndex = AMnDIndex(i, j, k);
						scan_->rawData()->setAxisValue(0, insertIndex.i(), starts.at(0) + i*steps.at(0));
						scan_->rawData()->setAxisValue(1, insertIndex.j(), starts.at(1) + j*steps.at(1));
						scan_->rawData()->setAxisValue(2, insertIndex.k(), starts.at(2) + j*steps.at(2));

						for (int di = 0, dataSourceCount = scan_->rawDataSourceCount(); di < dataSourceCount; di++){

							if ((scan_->rawDataSources()->at(di)->rank() - scanRank) == 0)
								scan_->rawData()->setValue(insertIndex, di, AMnDIndex(), -1);

							else if ((scan_->rawDataSources()->at(di)->rank() - scanRank) == 1){

								QVector<double> data = QVector<double>(scan_->rawDataSources()->at(di)->size(scan_->rawDataSources()->at(di)->rank()-1), -1);
								scan_->rawData()->setValue(insertIndex, di, data.constData());
							}
						}
					}
				}
			}
		}

		else {

			// Something recursive.  Will do if 4D ever becomes a thing.
		}

		scan_->rawData()->endInsertRows();
	}
}

void AMStepScanActionController::stopImplementation(const QString &command)
{
	if (command == "Stop At End Of Line")
		stoppingAtEndOfLine_ = true;
}

