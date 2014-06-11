#include "REIXSXESScanActionController.h"

#include "dataman/AMXESScan.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "analysis/REIXS/REIXSXESImageAB.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionRunner3.h"

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
	secondsTotal_ = configuration_->maximumDurationSeconds();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

}

REIXSXESScanActionController::~REIXSXESScanActionController(){}

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

	buildScanControllerImplementation();
}

void REIXSXESScanActionController::buildScanControllerImplementation()
{
	REIXSXESImageAB* xesSpectrum = new REIXSXESImageAB("xesSpectrum");
	xesSpectrum->setInputDataSources(QList<AMDataSource*>() << scan_->rawDataSources()->at(0));
	xesSpectrum->setSumRangeMaxY(58);
	xesSpectrum->setSumRangeMinY(5);
	xesSpectrum->setCorrelationHalfWidth(100);	// monitor for performance. Makes nicer fits when wider.
	xesSpectrum->enableLiveCorrelation(true);
	scan_->addAnalyzedDataSource(xesSpectrum);
}

void REIXSXESScanActionController::onDetectorAcquisitionSucceeded(){
	updateTimer_->stop();
	saveRawData();
	setFinished();
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

bool REIXSXESScanActionController::initializeImplementation(){

	/*
	AMControlInfoList positions(REIXSBeamline::bl()->exposedControls()->toInfoList());

	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
	positions.insert(9, grating);

	positions.append(REIXSBeamline::bl()->spectrometer()->tmSOE()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->tmMCPPreamp()->toInfo());
	positions.append(REIXSBeamline::bl()->sampleChamber()->tmSample()->toInfo());
	*/

	//Population Initial condition, prior to initialization moves
	AMControlInfoList positions;//(REIXSBeamline::bl()->exposedControls()->toInfoList());

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
	positions.append(REIXSBeamline::bl()->photonSource()->monoMirrorSelector()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()->toInfo());
	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
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
	positions.append(REIXSBeamline::bl()->sampleChamber()->tmSample()->toInfo());


	scan_->setScanInitialConditions(positions);


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


	//Update Scan Initial Conditions
	AMControlInfoList positions;//(REIXSBeamline::bl()->exposedControls()->toInfoList());

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
	positions.append(REIXSBeamline::bl()->photonSource()->monoMirrorSelector()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTranslation()->toInfo());
	positions.append(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()->toInfo());
	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
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
	positions.append(REIXSBeamline::bl()->sampleChamber()->tmSample()->toInfo());


	scan_->setScanInitialConditions(positions);

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
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("REIXS XES Initialization Actions",
																				   "REIXS XES Initialization Actions"),
															 AMListAction3::Parallel);

	AMListAction3 *initializationPreActions = new AMListAction3(new AMListActionInfo3("REIXS XES Initialization Actions",
																				   "REIXS XES Initialization Actions"),
															 AMListAction3::Parallel);

	AMListAction3 *initializationCombinedActions = new AMListAction3(new AMListActionInfo3("REIXS XES Initialization Actions",
																				   "REIXS XES Initialization Actions"),
															 AMListAction3::Sequential);




	if(configuration_->applyPolarization() && configuration_->polarization() == 5 && configuration_->polarizationAngle() != REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarizationAngle();
		AMControlInfo polarizationAngleSetpoint = tmpControl->toInfo();
		polarizationAngleSetpoint.setValue(configuration_->polarizationAngle());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationAngleSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationPreActions->addSubAction(moveAction);

	}

	if(configuration_->applyPolarization() && REIXSBeamline::bl()->photonSource()->epuPolarization()->value() != configuration_->polarization()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarization();
		AMControlInfo polarizationSetpoint = tmpControl->toInfo();
		polarizationSetpoint.setValue(configuration_->polarization());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationActions->addSubAction(moveAction);
	}



	if(configuration_->applySlitWidth()){
		tmpControl = REIXSBeamline::bl()->photonSource()->monoSlit();
		AMControlInfo monoSlitSetpoint = tmpControl->toInfo();
		monoSlitSetpoint.setValue(configuration_->slitWidth());
		moveActionInfo = new AMControlMoveActionInfo3(monoSlitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationActions->addSubAction(moveAction);
	}


	if(configuration_->applyEnergy()){
		tmpControl = REIXSBeamline::bl()->photonSource()->energy();
		AMControlInfo energySetpoint = tmpControl->toInfo();
		energySetpoint.setValue(configuration_->energy());
		moveActionInfo = new AMControlMoveActionInfo3(energySetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationActions->addSubAction(moveAction);
	}

	initializationCombinedActions->addSubAction(initializationPreActions);
	initializationCombinedActions->addSubAction(initializationActions);

	return initializationCombinedActions;
}




bool REIXSXESScanActionController::startImplementation(){
	REIXSBeamline::bl()->mcpDetector()->setAcquisitionTime(configuration_->maximumDurationSeconds());
	REIXSBeamline::bl()->mcpDetector()->setTotalCountTarget(configuration_->maximumTotalCounts());
	REIXSBeamline::bl()->mcpDetector()->setFinishedConditionTotalTimeOrTotalCounts();
	REIXSBeamline::bl()->mcpDetector()->acquire();
	setStarted();


	updateTimer_ = new QTimer();
	updateTimer_->setInterval(5000);
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(saveRawData()));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
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


void REIXSXESScanActionController::skip(const QString &command){

	ScanState currentState = state();

	if(currentState == Running || currentState == Paused) {
		disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	}
	REIXSBeamline::bl()->mcpDetector()->cancelAcquisition();
	saveRawData();
	setFinished();
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
	double averageCountRate =  REIXSBeamline::bl()->mcpDetector()->totalCounts() / secondsElapsed_;
	double estimatedTime = configuration_->maximumTotalCounts() / averageCountRate;

	secondsTotal_ = qMin(configuration_->maximumDurationSeconds(), estimatedTime);

	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}


