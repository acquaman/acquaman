#include "REIXSXESScanActionController.h"

#include "dataman/AMXESScan.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "analysis/REIXS/REIXSXESImageAB.h"
#include "dataman/datastore/AMCDFDataStore.h"

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

	REIXSXESImageAB* xesSpectrum = new REIXSXESImageAB("xesSpectrum");
	xesSpectrum->setInputDataSources(QList<AMDataSource*>() << imageDataSource);
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
	// Is the detector connected?
	//if(!REIXSBeamline::bl()->mcpDetector()->canRead() || !REIXSBeamline::bl()->mcpDetector()->canConfigure()) {
	if(!REIXSBeamline::bl()->mcpDetector()->isConnected()){
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 17, "Could not connect to the MCP detector before starting an XES scan. Please report this problem to the beamline staff."));
		return false;
	}

	AMControlInfoList positions(REIXSBeamline::bl()->exposedControls()->toInfoList());
	// add the spectrometer grating selection, since it's not a "control" anywhere.
	AMControlInfo grating("spectrometerGrating", REIXSBeamline::bl()->spectrometer()->specifiedGrating(), 0, 0, "[choice]", 0.1, "Spectrometer Grating");
	grating.setEnumString(REIXSBeamline::bl()->spectrometer()->spectrometerCalibration()->gratingAt(grating.value()).name());
	positions.insert(0, grating);

	//scan_->scanInitialConditions()->setValuesFrom(positions);
	scan_->setScanInitialConditions(positions);

	setInitialized();
	return true;
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


#include "dataman/AMSample.h"
void REIXSXESScanActionController::initializeScanMetaData()
{
	//qDebug() << "For Starters REIXSBeamline::bl()->photonSource()->energy().value(): "<< REIXSBeamline::bl()->photonSource()->energy()->value();

	int sampleId = REIXSBeamline::bl()->currentSampleId();

	if(configuration_->namedAutomatically()) {
		if(sampleId >= 1) {
			scan_->setSampleId(sampleId);
			QString sampleName = AMSample::sampleNameForId(AMDatabase::database("user"), sampleId); // scan_->sampleName() won't work until the scan is saved to the database.
			scan_->setName(QString("%1 %2 %3 eV").arg(sampleName).arg(configuration_->autoScanName()).arg(REIXSBeamline::bl()->photonSource()->energy()->value()));

			scan_->setNumber(scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString("sampleId = %1").arg(sampleId))+1);
		}
		else {
			scan_->setName(QString("%1 %2 eV").arg(configuration_->autoScanName()).arg(REIXSBeamline::bl()->photonSource()->energy()->value()));

			scan_->setNumber(0);
			scan_->setSampleId(-1);
		}
	}
	else {
		scan_->setName(QString("%1 %2 eV").arg(configuration_->userScanName()).arg(REIXSBeamline::bl()->photonSource()->energy()->value()));
		if(scan_->name().isEmpty())
			scan_->setName(QString("%1 %2 eV").arg(configuration_->autoScanName()).arg(REIXSBeamline::bl()->photonSource()->energy()->value()));

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
