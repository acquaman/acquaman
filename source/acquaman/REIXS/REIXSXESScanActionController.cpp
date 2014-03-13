#include "REIXSXESScanActionController.h"

#include "dataman/AMXESScan.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "analysis/REIXS/REIXSXESImageAB.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionRunner3.h"



 REIXSXESScanActionController::~REIXSXESScanActionController(){}

REIXSXESScanActionController::REIXSXESScanActionController(REIXSXESScanConfiguration *configuration, QObject *parent) :
	AMScanActionController(configuration_, parent)
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

	//Population Initial condition, prior to initialization moves
	AMControlInfoList positions(REIXSBeamline::bl()->exposedControls()->toInfoList());

	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
	positions.insert(9, grating);

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
	//connect(xesActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));
	AMActionRunner3::scanActionRunner()->addActionToQueue(xesActionsInitializationList_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);

	return true;
}

void REIXSXESScanActionController::onInitializationActionsListSucceeded(){


	//Update Scan Initial Conditions
	AMControlInfoList positions(REIXSBeamline::bl()->exposedControls()->toInfoList());

	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
	positions.insert(9, grating);

	// add the polarization selection, since it's not a "control" anywhere.
	AMControlInfo polarization("beamlinePolarization", REIXSBeamline::bl()->photonSource()->epuPolarization()->value(), 0, 0, "[choice]", 0.1, "EPU Polarization");
	polarization.setEnumString(REIXSBeamline::bl()->photonSource()->epuPolarization()->enumNameAt(REIXSBeamline::bl()->photonSource()->epuPolarization()->value()));
	positions.append(polarization);

	if(REIXSBeamline::bl()->photonSource()->epuPolarization()->value() == 5)
	{
	AMControlInfo polarizationAngle("beamlinePolarizationAngle", REIXSBeamline::bl()->photonSource()->epuPolarizationAngle()->value(), 0, 0, "degrees", 0.1, "EPU Polarization Angle");
	positions.append(polarizationAngle);
	}



	scan_->setScanInitialConditions(positions);

	disconnect(xesActionsInitializationList_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	//disconnect(xesActionsInitializationList_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	setInitialized();
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



	//qDebug() << "applySlitWidth() returns: " << configuration_->applySlitWidth();

	if(configuration_->applyPolarization() && configuration_->polarization() == 5){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarizationAngle();
		AMControlInfo polarizationAngleSetpoint = tmpControl->toInfo();
		polarizationAngleSetpoint.setValue(configuration_->polarizationAngle());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationAngleSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationPreActions->addSubAction(moveAction);

	}

	if(configuration_->applyPolarization()){
		tmpControl = REIXSBeamline::bl()->photonSource()->epuPolarization();
		AMControlInfo polarizationSetpoint = tmpControl->toInfo();
		polarizationSetpoint.setValue(configuration_->polarization());
		moveActionInfo = new AMControlMoveActionInfo3(polarizationSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationActions->addSubAction(moveAction);
	}



	if(configuration_->applySlitWidth()){
//		qDebug() << "it would have queued a slit move to: " << configuration_->slitWidth();
		tmpControl = REIXSBeamline::bl()->photonSource()->monoSlit();
		AMControlInfo monoSlitSetpoint = tmpControl->toInfo();
		monoSlitSetpoint.setValue(configuration_->slitWidth());
		moveActionInfo = new AMControlMoveActionInfo3(monoSlitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		initializationActions->addSubAction(moveAction);
	}


	qDebug() << "applyEnergy() returns: " << configuration_->applyEnergy();
	if(configuration_->applyEnergy()){
//		qDebug() << "it would have queued an energy move to: " << configuration_->energy();
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
	return true;
}

void REIXSXESScanActionController::cancelImplementation(){

	ScanState currentState = state();

	if(currentState == Running || currentState == Paused) {
		disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	}
	REIXSBeamline::bl()->mcpDetector()->cancelAcquisition();
	setCancelled();
}


void REIXSXESScanActionController::skip(const QString &command){

	ScanState currentState = state();

	if(currentState == Running || currentState == Paused) {
		disconnect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(imageDataChanged()), this, SLOT(onNewImageValues()));
	}
	REIXSBeamline::bl()->mcpDetector()->cancelAcquisition();
	setFinished();
}


#include "dataman/AMSample.h"
void REIXSXESScanActionController::initializeScanMetaData()
{
	//qDebug() << "For Starters REIXSBeamline::bl()->photonSource()->energy().value(): "<< REIXSBeamline::bl()->photonSource()->energy()->value();

	int sampleId = REIXSBeamline::bl()->currentSampleId();

	if(configuration_->namedAutomatically()) {
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			scan_->setName(QString("%1 %2 %3 eV").arg(sampleName).arg(configuration_->autoScanName()).arg(configuration_->centerEV()));

			scan_->setNumber(scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString("sampleId = %1").arg(sampleId))+1);
		}
		else {
			scan_->setName(QString("%1 %2 eV").arg(configuration_->autoScanName()).arg(configuration_->centerEV()));

			scan_->setNumber(0);
			scan_->setSampleId(-1);
		}
	}
	else {
		scan_->setName(QString("%1 %2 eV").arg(configuration_->userScanName()).arg(configuration_->centerEV()));
		if(scan_->name().isEmpty())
			scan_->setName(QString("%1 %2 eV").arg(configuration_->autoScanName()).arg(configuration_->centerEV()));

		if (configuration_->scanNumber() == 0)
		{
			scan_->setNumber(scan_->largestNumberInScansWhere(AMDatabase::database("user"), "")+1);
		}
		else
		{
			scan_->setNumber(configuration_->scanNumber());
		}

		scan_->setSampleId(configuration_->sampleId());
	}

	scan_->setRunId(AMUser::user()->currentRunId());
}
