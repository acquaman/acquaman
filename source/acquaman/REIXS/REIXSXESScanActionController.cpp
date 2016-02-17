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


#include "REIXSXESScanActionController.h"

#include "dataman/AMXESScan.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "analysis/REIXS/REIXSXESImageInterpolationAB.h"
#include "analysis/REIXS/REIXSXESImageAB.h"
#include "dataman/datastore/AMCDFDataStore.h"
#include "dataman/AMTextStream.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMCurrentAmplifier.h"


#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMWaitAction.h"
#include "ui/util/AMMessageBoxWTimeout.h"

#include <QThread>
#include <QFileInfo>
#include <QStringBuilder>



REIXSXESScanActionController::REIXSXESScanActionController(REIXSXESScanConfiguration *configuration, QObject *parent) :
	AMScanActionController(configuration, parent)
{
	configuration_ = configuration;

	// create our new scan object
	scan_ = new AMXESScan();

	// Automatic or manual setting of meta-data for the scan:
	///////////////////////////////////
	initializeScanMetaData();
	///////////////////////////////////

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
	scan_->setFileFormat("amCDFv1");
	scan_->replaceRawDataStore(new AMCDFDataStore(QString("%1/%2").arg(AMUserSettings::userDataFolder).arg(scan_->filePath()), false));

	// set the scan configuration within the scan:
	scan_->setScanConfiguration(configuration_);
	///////////////////////////

	REIXSBeamline::bl()->mcpDetector()->setClearOnStart(!configuration_->doNotClearExistingCounts());

	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime(true);
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

}

REIXSXESScanActionController::~REIXSXESScanActionController()
{
	// No need to clean up fileWriterThread, we'll be informed to delete ourself after it is destroyed
//	fileWriterThread_->deleteLater();
}

void REIXSXESScanActionController::buildScanController()
{
	AMMeasurementInfo configuredDetector(REIXSBeamline::bl()->mcpDetector()->name(), REIXSBeamline::bl()->mcpDetector()->description(), REIXSBeamline::bl()->mcpDetector()->units(), REIXSBeamline::bl()->mcpDetector()->axes());
	if(REIXSBeamline::bl()->mcpDetector()->isConnected()) {
		configuredDetector.axes[0].size = REIXSBeamline::bl()->mcpDetector()->image()->size(0);
		configuredDetector.axes[1].size = REIXSBeamline::bl()->mcpDetector()->image()->size(1);
	}
	else {
		configuredDetector.axes[0].size = 1024;
		configuredDetector.axes[1].size = 64;
	}
	configuredDetector.name = "xesImage";	// necessary for file loader/saver.

	scan_->rawData()->addMeasurement(configuredDetector);
	AMRawDataSource* imageDataSource = new AMRawDataSource(scan_->rawData(), 0);
	scan_->addRawDataSource(imageDataSource);

	scan_->rawData()->addMeasurement(AMMeasurementInfo("totalCounts", "Total Counts", "counts"));
	AMRawDataSource* totalCountsDataSource = new AMRawDataSource(scan_->rawData(), 1);
	scan_->addRawDataSource(totalCountsDataSource, false, false);

	QString path = scan_->filePath().remove(".cdf");
	QFileInfo fullPath(path);	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	qRegisterMetaType<AMScanActionControllerBasicFileWriter::FileWriterError>("FileWriterError");
	fileWriter_ = new REIXSScanActionControllerMCPFileWriter(AMUserSettings::userDataFolder % fullPath.filePath());
	connect(fileWriter_, SIGNAL(fileWriterIsBusy(bool)), this, SLOT(onFileWriterIsBusy(bool)));
	connect(fileWriter_, SIGNAL(fileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)), this, SLOT(onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError)));
	connect(this, SIGNAL(requestWriteToFile(int,QString)), fileWriter_, SLOT(writeToFile(int,QString)));
	connect(this, SIGNAL(finishWritingToFile()), fileWriter_, SLOT(finishWriting()));

	fileWriterThread_ = new QThread();
	connect(this, SIGNAL(finished()), this, SLOT(onScanControllerFinished()));
	connect(this, SIGNAL(cancelled()), this, SLOT(onScanControllerFinished()));
	connect(this, SIGNAL(failed()), this, SLOT(onScanControllerFinished()));
	fileWriter_->moveToThread(fileWriterThread_);
	fileWriterThread_->start();

	buildScanControllerImplementation();
}

void REIXSXESScanActionController::buildScanControllerImplementation()
{
	initializePositions();  //initialized here so that they're ready for the the AB when it's created.

	if (scan_ && scan_->rawDataSourceCount() > 0) {
		REIXSXESImageInterpolationAB* xesSpectrum = new REIXSXESImageInterpolationAB("xesSpectrum");
		xesSpectrum->setInputDataSources(QList<AMDataSource*>() << scan_->rawDataSources()->at(0));
		scan_->addAnalyzedDataSource(xesSpectrum);
	}
}

void REIXSXESScanActionController::onDetectorAcquisitionSucceeded(){
	updateTimer_->stop();
	disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(writeDataToFiles()));
	saveRawData();
	scanControllerStateMachineFinished_ = true;
	if(readyForFinished())
		setFinished();
	else if(fileWriterIsBusy_)
		emit finishWritingToFile();
}


void REIXSXESScanActionController::onDetectorAcquisitionFailed(){
	updateTimer_->stop();
	saveRawData();
	setFailed();
}



void REIXSXESScanActionController::onNewImageValues(){
	QVector<int> imageData = REIXSBeamline::bl()->mcpDetector()->imageData();

	if(!scan_->rawData()->setValue(AMnDIndex(), 0, imageData.constData()))
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 37, "Error setting the new values from the MCP Detector. The size of the image didn't match what it should be.  This is probably a problem with the network connection to the detector, or a bug in the detector driver."));

	if(!scan_->rawData()->setValue(AMnDIndex(), 1, AMnDIndex(), double(REIXSBeamline::bl()->mcpDetector()->totalCounts())))
		AMErrorMon::debug(this, 377, "Error setting new values for the MCP Detector total counts. Please report this bug to the REIXS Acquaman developers.");

}

void REIXSXESScanActionController::saveRawData(){
	AMCDFDataStore* cdfDataStore = static_cast<AMCDFDataStore*>(scan_->rawData());
	if(!cdfDataStore || !cdfDataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running XES scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to the beamline software developers."));
}

void REIXSXESScanActionController::initializePositions()
{
	//Population Initial condition, prior to initialization moves
	AMControlInfoList positions;

	positions.append(REIXSBeamline::bl()->photonSource()->ringCurrent()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->epuPolarization()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->toInfo());

	positions.append(REIXSBeamline::bl()->photonSource()->energy()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->userEnergyOffset()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->monoSlit()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->x()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->y()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->z()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->r()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->gratingMask()->toInfo());  //D
	positions.append(REIXSBeamline::bl()->spectrometer()->toInfo());
	// add the polarization selection, since it's not a "control" anywhere.
	AMControlInfo polarization("beamlinePolarization", REIXSBeamline::bl()->photonSource()->epuPolarization()->value(), 0, 0, "[choice]", 0.1, "EPU Polarization");
	polarization.setEnumString(REIXSBeamline::bl()->photonSource()->epuPolarization()->enumNameAt(REIXSBeamline::bl()->photonSource()->epuPolarization()->value()));
	positions.append(polarization);
	if(REIXSBeamline::bl()->photonSource()->epuPolarization()->value() == 5)
	{
		AMControlInfo polarizationAngle("beamlinePolarizationAngle", REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value(), 0, 0, "degrees", 0.1, "EPU Polarization Angle");
		positions.append(polarizationAngle);
	}
	positions.append(REIXSBeamline::bl()->photonSource()->monoGratingSelector()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->monoGratingTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->monoMirrorSelector()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->monoMirrorTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()->toInfo());
	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(int(grating.value())).name());
	positions.append(grating);
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->x()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->y()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->z()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->u()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->v()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->w()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->r()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->s()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->hexapod()->t()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->endstationTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->M5Pitch()->toInfo());
	positions.append(REIXSBeamline::bl()->photonSource()->M5Yaw()->toInfo());

	positions.append(REIXSBeamline::bl()->spectrometer()->tmSOE()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->tmMCPPreamp()->toInfo());

	positions.append(REIXSBeamline::bl()->photonSource()->ringCurrent()->toInfo());

	positions.append(REIXSBeamline::bl()->I0Current()->toInfo());
	double I0CurrentValue = REIXSBeamline::bl()->scaler()->channelAt(16)->voltage() * REIXSBeamline::bl()->scaler()->channelAt(16)->currentAmplifier()->value();
	AMControlInfo I0Value("I0Value", I0CurrentValue, 0, 0, QString(REIXSBeamline::bl()->scaler()->channelAt(16)->currentAmplifier()->units().remove("/V")), 0.1, "I0 Amplifier Output");
	positions.append(I0Value);
	AMControlInfo I0Sensitivity("I0Sensitivity", REIXSBeamline::bl()->scaler()->channelAt(16)->currentAmplifier()->value(), 0, 0, REIXSBeamline::bl()->scaler()->channelAt(16)->currentAmplifier()->units(), 0.1, "I0 Amplifier Sensitivity");
	positions.append(I0Sensitivity);

	positions.append(REIXSBeamline::bl()->TEYCurrent()->toInfo());
	double TEYCurrentValue = REIXSBeamline::bl()->scaler()->channelAt(18)->voltage() * REIXSBeamline::bl()->scaler()->channelAt(18)->currentAmplifier()->value();
	AMControlInfo TEYValue("TEYValue", TEYCurrentValue, 0, 0, QString(REIXSBeamline::bl()->scaler()->channelAt(18)->currentAmplifier()->units().remove("/V")), 0.1, "TEY Amplifier Output");
	positions.append(TEYValue);
	AMControlInfo TEYSensitivity("TEYSensitivity", REIXSBeamline::bl()->scaler()->channelAt(18)->currentAmplifier()->value(), 0, 0, REIXSBeamline::bl()->scaler()->channelAt(18)->currentAmplifier()->units(), 0.1, "TEY Amplifier Sensitivity");
	positions.append(TEYSensitivity);


	scan_->setScanInitialConditions(positions);
}


bool REIXSXESScanActionController::initializeImplementation(){

	// Is the detector connected?
	//if(!REIXSBeamline::bl()->mcpDetector()->canRead() || !REIXSBeamline::bl()->mcpDetector()->canConfigure()) {
	if(!REIXSBeamline::bl()->mcpDetector()->isConnected()){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 17, "Could not connect to the MCP detector before starting an XES scan. Please report this problem to the beamline staff."));
		return false;
	}

	AMAction3 *initializationActions = createInitializationActions();
	if(qobject_cast<AMListAction3*>(initializationActions))
		xesActionsInitializationList_ = qobject_cast<AMListAction3*>(initializationActions);

	connect(xesActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	connect(xesActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(xesActionsInitializationList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);

	return true;
}

void REIXSXESScanActionController::onInitializationActionsListSucceeded(){

	initializePositions();

	disconnect(xesActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(xesActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	setInitialized();
}

void REIXSXESScanActionController::onInitializationActionsListFailed(){

	REIXSBeamline::bl()->mcpDetector()->cancelAcquisition();
	saveRawData();
	setFailed();
}


AMAction3* REIXSXESScanActionController::createInitializationActions(){

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("REIXS XES Initialization Actions",
																				   "REIXS XES Initialization Actions"),
															 AMListAction3::Parallel);

	AMListAction3 *initializationPreActions = new AMListAction3(new AMListActionInfo3("REIXS XES Initialization Actions",
																				   "REIXS XES Initialization Actions"),
															 AMListAction3::Parallel);

	AMListAction3 *initializationCombinedActions = new AMListAction3(new AMListActionInfo3("REIXS XES Initialization Actions",
																				   "REIXS XES Initialization Actions"),
															 AMListAction3::Sequential);




	if(configuration_->applyPolarization() && configuration_->polarization() == 5 && configuration_->polarizationAngle() != REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value())
		initializationPreActions->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->photonSource()->epuPolarizationAngle(), configuration_->polarizationAngle()));

	if(configuration_->applyPolarization() && REIXSBeamline::bl()->photonSource()->epuPolarization()->value() != configuration_->polarization())
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->photonSource()->epuPolarization(), configuration_->polarization()));



	if(configuration_->applySlitWidth())
		initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->photonSource()->monoSlit(), configuration_->slitWidth()));


	if(configuration_->applyEnergy()){
		AMListAction3 *energyMoveAction = new AMListAction3(new AMListActionInfo3("REIXS Mono Energy Initialization Actions",
											  "REIXS Mono Energy Initialization Actions"),
								    AMListAction3::Sequential);

		energyMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->photonSource()->energy(), configuration_->energy()));
		energyMoveAction->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.1)));  // for mono encoder time averaging to settle

		energyMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->photonSource()->energy(), configuration_->energy()));
		//Yes, three times to be sure that we're         there, but REIXSBrokenMonoContorl will ignore latter moves if we already made it.
		energyMoveAction->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.1)));  // for mono encoder time averaging to settle

		energyMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->photonSource()->energy(), configuration_->energy()));
		energyMoveAction->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.1)));  // for mono encoder time averaging to settle

		initializationActions->addSubAction(energyMoveAction);

	}

	initializationCombinedActions->addSubAction(initializationPreActions);
	initializationCombinedActions->addSubAction(initializationActions);

	return initializationCombinedActions;
}




bool REIXSXESScanActionController::startImplementation()
{
	writeHeaderToFile();

	REIXSBeamline::bl()->mcpDetector()->setAcquisitionTime(configuration_->maximumDurationSeconds());
	REIXSBeamline::bl()->mcpDetector()->setTotalCountTarget(int(configuration_->maximumTotalCounts()));
	REIXSBeamline::bl()->mcpDetector()->setFinishedConditionTotalTimeOrTotalCounts();
	REIXSBeamline::bl()->mcpDetector()->acquire();
	setStarted();


	updateTimer_ = new QTimer();
	updateTimer_->setInterval(5000);
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(saveRawData()));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(writeDataToFiles()));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(acquisitionSucceeded()), this, SLOT(onDetectorAcquisitionSucceeded()));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(acquisitionFailed()), this, SLOT(onDetectorAcquisitionFailed()));
	return true;
}

void REIXSXESScanActionController::cancelImplementation(){

	ScanState currentState = state();

	if(currentState == Running || currentState == Paused) {
		disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	}
	REIXSBeamline::bl()->mcpDetector()->cancelAcquisition();
	saveRawData();
	setCancelled();
}

void REIXSXESScanActionController::stopImplementation(const QString &command)
{
	Q_UNUSED(command);

	disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(writeDataToFiles()));

	REIXSBeamline::bl()->mcpDetector()->cancelAcquisition();
	saveRawData();
	writeDataToFiles();
	scanControllerStateMachineFinished_ = true;
	if(readyForFinished())
		setFinished();
	else if(fileWriterIsBusy_)
		emit finishWritingToFile();
}

#include "dataman/AMSample.h"
void REIXSXESScanActionController::initializeScanMetaData()
{

	int sampleId = REIXSBeamline::bl()->currentSampleId();

	double energyForName = REIXSBeamline::bl()->photonSource()->energy()->value();

	if(configuration_->applyEnergy()) energyForName = configuration_->energy();

	if(configuration_->namedAutomatically()) {  //Named Automatically
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			QString sampleName = AMSamplePre2013::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			scan_->setName(QString("%1 %2 %3 eV").arg(sampleName).arg(configuration_->autoScanName()).arg(energyForName, 0, 'f', 2));

		}
		else {
			scan_->setName(QString("%1 %2 eV").arg(configuration_->autoScanName()).arg(energyForName, 0, 'f', 2));

			scan_->setSampleId(-1);
		}
	}
	else {									//Named Manually
		scan_->setName(QString("%1 %2 eV").arg(configuration_->userScanName()).arg(energyForName, 0, 'f', 2));
		scan_->setSampleId(configuration_->sampleId());
	}

	scan_->setRunId(AMUser::user()->currentRunId());
}

void REIXSXESScanActionController::onScanTimerUpdate()
{
	secondsTotal_ = configuration_->totalTime(true);

	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

void REIXSXESScanActionController::writeHeaderToFile()
{
	AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo("Invalid", "Invalid");
	QString separator = "|!|!|";
	QString rank1String;
	rank1String.append("Start Info\n");
	rank1String.append("Version: Acquaman Generic Linear Step 0.1\n");

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
	headerText_ = rank1String;
}

void REIXSXESScanActionController::writeDataToFiles()
{
	QString rank1String = headerText_;

	/* Stress testing
	QTime startTime  = QTime::currentTime();
	*/

	// First do the MCP detector.
	AMnDIndex mcpDataSize = REIXSBeamline::bl()->mcpDetector()->size();
	QVector<double> mcpData = QVector<double>(mcpDataSize.product());
	scan_->rawDataSources()->at(0)->values(AMnDIndex(0, 0), AMnDIndex(mcpDataSize.i()-1, mcpDataSize.j()-1), mcpData.data());

	// Important!  Darren's thoughts on this PV/getting data off the detector.
	// This loop might seem like it has weird logic, but that is because of how the data is read directly from the PV (seemingly).
	// This required a vertical mirroring and indexing vertically rather than horizontally.  I don't really like it, but you can't argue with working.
	for (int j = mcpDataSize.j() - 1, jSize = j+1; j >= 0; j--)
		for (int i = 0, iSize = mcpDataSize.i(); i < iSize; i++)
			rank1String.append(QString("%1%2").arg(double(mcpData.at(j+i*jSize)), 0, 'g', 19).arg(i == (iSize-1) ? "\n" : "\t"));

	// Next is the total counts.
	rank1String.append(QString("%1").arg(int(scan_->rawDataSources()->at(1)->value(AMnDIndex()))));
	rank1String.append("\n");

	/* Stress testing
	QTime endTime = QTime::currentTime();
	qDebug() << "Time to ready data for writing " << startTime.msecsTo(endTime);
	*/

	emit requestWriteToFile(2, rank1String);
}

void REIXSXESScanActionController::onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error)
{
	qDebug() << "Got a file writer error " << error;

	QString userErrorString;

	switch(error){

	case AMScanActionControllerBasicFileWriter::AlreadyExistsError:
		AMErrorMon::alert(this, REIXSXESSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS, QString("Error, the %1 Scan Action Controller attempted to write you data to file that already exists. This is a serious problem, please contact the Acquaman developers.").arg(configuration_->technique()));
		userErrorString = "Your scan has been aborted because the file Acquaman wanted to write to already exists (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::CouldNotOpenError:
		AMErrorMon::alert(this, REIXSXESSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE, QString("Error, the %1 Scan Action Controller failed to open the file to write your data. This is a serious problem, please contact the Acquaman developers.").arg(configuration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to open the desired file for writing (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	case AMScanActionControllerBasicFileWriter::FailedToWriteFile:
		AMErrorMon::error(this, REIXSXESSCANACTIONCONTROLLER_FAILED_TO_WRITE_FILE, QString("Error, the %1 Scan Action Controller failed to write your data. This is a serious problem, please contact the Acquaman developers.").arg(configuration_->technique()));
		userErrorString = "Your scan has been aborted because Acquaman was unable to write to the desired file (for internal storage). This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;

	default:
		AMErrorMon::alert(this, REIXSXESSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR, QString("Error, the %1 Scan Action Controller encountered a serious, but unknown, file problem. This is a serious problem, please contact the Acquaman developers.").arg(configuration_->technique()));
		userErrorString = "Your scan has been aborted because an unknown file error (for internal storage) has occured. This is a serious problem and would have resulted in collecting data but not saving it. Please contact the Acquaman developers immediately.";
		break;
	}

	setFailed();

	AMMessageBoxWTimeout::showMessageWTimeout("Sorry! Your scan has been cancelled because a file writing error occured.",
											  "Acquaman saves files for long term storage, but some sort of error occured for your scan.",
											  userErrorString);
}

//void REIXSXESScanActionController::onFileWriterIsBusy(bool isBusy)
//{
//	fileWriterIsBusy_ = isBusy;
//	emit readyForDeletion(!fileWriterIsBusy_);
//}

void REIXSXESScanActionController::onScanControllerFinished()
{
	writeDataToFiles();
//	fileWriterThread_->quit();
}
