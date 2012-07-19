#include "AMSA1DScanController.h"

#include "dataman/AMXASScan.h"
#include "beamline/AMControl.h"
#include "acquaman/AMSADetector.h"
#include "acquaman/AMRegionScanConfiguration.h"
#include "actions2/actions/AMInternalControlMoveAction.h"
#include "dataman/datastore/AMCDFDataStore.h"
#include "dataman/AMUser.h"

#include <QStringBuilder>

AMSA1DScanController::AMSA1DScanController(AMControl *control, QList<AMSADetector *> detectors, AMRegionScanConfiguration *scanConfig, QObject* parent)
	: AMScanController(scanConfig, parent)
{
	control_ = control;
	detectors_ = detectors;
	regionScanConfig_ = scanConfig;

	currentRegion_ = -1;
	currentStepInRegion_ = -1;

	initializingDetectors_ = false;
	triggeringDetectors_ = false;

	moveAction_ = 0;

	// create our new scan object
	scan_ = new AMXASScan();
	QString scanName = regionScanConfig_->userScanName();
	if(scanName.isEmpty())
		scanName = regionScanConfig_->autoScanName();
	scan_->setName(scanName);

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
	scan_->setFileFormat("amCDFv1");
	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % "/" % scan_->filePath(), false));
	scan_->setRunId(AMUser::user()->currentRunId());

	// set the scan configuration within the scan:
	scan_->setScanConfiguration(regionScanConfig_);
	///////////////////////////
}

AMSA1DScanController::~AMSA1DScanController() {
	delete moveAction_;
	moveAction_ = 0;
}

void AMSA1DScanController::createRawDataSources()
{
	for(int i=0, cc=scan_->rawData()->measurementCount(); i<cc; ++i)
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i));
}

bool AMSA1DScanController::setControl(AMControl *control)
{
	if(state() != Constructed)
		return false;

	control_ = control;
	return true;
}

bool AMSA1DScanController::setDetectors(const QList<AMSADetector *> &detectors)
{
	if(state() != Constructed)
		return false;

	detectors_ = detectors;
	return true;
}

bool AMSA1DScanController::initializeImplementation()
{
	if(!control_ || !control_->canMove()) {
		AMErrorMon::alert(this, -11, "Could not start the scan because the control to move was not valid or connected. Please report this bug to your beamline's software developers.");
		return false;
	}

	// Create scan axis
	scan_->rawData()->addScanAxis(AMAxisInfo(control_->name(), 0, control_->description(), control_->units()));

	// Create measurements in the data store for each detector.
	foreach(AMSADetector* detector, detectors_) {
		if(!scan_->rawData()->addMeasurement(detector->measurementInfo())) {
			AMErrorMon::alert(this, -10, QString("Could not create a measurement in the data store for detector '%1', possibly because another with that name already exists. Please report this bug to your beamline's software developers.").arg(detector->name()));
			return false;
		}
	}

	// Create raw data sources (by default, one for each detector)
	createRawDataSources();
	// create analysis blocks as defined here:
	createAnalysisBlocks();


	// initialize detectors:
	detectorInitializationFailed_ = false;
	initializingDetectors_ = true;
	foreach(AMSADetector* detector, detectors_) {
		connect(detector, SIGNAL(initialized(bool)), this, SLOT(onDetectorInitialized(bool)));
		if(!detector->init()) {
			AMErrorMon::alert(this, -12, QString("Could not start the scan because the detector '%1' could not be initialized. Please report this bug to your beamline's software developers.").arg(detector->description()));
			return false;
		}
	}
	initializingDetectors_ = false;
	onDetectorInitialized();

	return true;
}

void AMSA1DScanController::onDetectorInitialized(bool succeeded)
{
	if(!succeeded)
		detectorInitializationFailed_ = true;

	// Don't proceed with this check until all detectors have been told to init(). If we're still inside initializeImplementation(), don't do anything for now.
	if(initializingDetectors_)
		return;

	// Find out if all are initialized:
	foreach(AMSADetector* detector, detectors_) {
		if(!detector->isInitialized())
			return;
	}
	// OK, everyone's ready to go.

	foreach(AMSADetector* detector, detectors_)
		disconnect(detector, SIGNAL(initialized(bool)), this, SLOT(onDetectorInitialized(bool)));

	if(detectorInitializationFailed_) {
		AMErrorMon::alert(this, -13, QString("Could not start the scan because a detector could not be initialized. Please report this problem to your beamline's software developers."));
		setFailed();
	}
	else
		setInitialized();
}

bool AMSA1DScanController::startImplementation()
{
	setStarted();

	doNextPosition();

	return true;
}

void AMSA1DScanController::doNextPosition()
{
	currentStepInRegion_++;

	if(currentRegion_<0 || currentPosition() > regionScanConfig_->regionEnd(currentRegion_)) { // Entering new region.
		currentRegion_++;
		currentStepInRegion_ = 0;

		if(currentRegion_ == regionScanConfig_->regionCount()) { // all done!
			// set progress, save file?
			setFinished();
			return;
		}

		// set acquisition time for this region.
		foreach(AMSADetector* detector, detectors_)
			detector->setAcquisitionTime(regionScanConfig_->regionTime(currentRegion_));
	}

	// move to the position.
	moveAction_ = new AMInternalControlMoveAction(control_, currentPosition());
	connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onMoveActionSucceeded()));
	connect(moveAction_, SIGNAL(failed()), this, SLOT(onMoveActionFailed()));
	moveAction_->start();
}

double AMSA1DScanController::currentPosition() const
{
	return regionScanConfig_->regionStart(currentRegion_) + regionScanConfig_->regionDelta(currentRegion_)*currentStepInRegion_;
}

void AMSA1DScanController::onMoveActionSucceeded()
{
	disconnect(moveAction_, 0, this, 0);
	moveAction_->deleteLater();
	moveAction_ = 0;

	doNextAcquisition();
}

void AMSA1DScanController::onMoveActionFailed()
{
	disconnect(moveAction_, 0, this, 0);
	moveAction_->deleteLater();
	moveAction_ = 0;

	AMErrorMon::alert(this, -14, QString("The scan failed because the control (%1)failed to move properly to %2 %3.").arg(control_->description()).arg(currentPosition()).arg(control_->units()));
	setFailed();
}

void AMSA1DScanController::doNextAcquisition()
{
	// trigger all the detectors
	detectorAcquisitionFailed_ = false;
	triggeringDetectors_ = true;
	foreach(AMSADetector* detector, detectors_) {
		if(detector->type() == AMSADetector::Triggered) {
			connect(detector, SIGNAL(acquisitionFinished(bool)), this, SLOT(onDetectorAcquisitionFinished(bool)));
			detector->acquireNow();	/// \todo Check for failure?
		}
	}
	triggeringDetectors_ = false;
	onDetectorAcquisitionFinished();
}

void AMSA1DScanController::onDetectorAcquisitionFinished(bool succeeded)
{
	if(!succeeded)
		detectorAcquisitionFailed_ = true;

	if(triggeringDetectors_)
		return;	// wait to do this check until we've triggered all detectors.


	// Find out if all triggered detectors are done acquiring:
	foreach(AMSADetector* detector, detectors_) {
		if(detector->type() == AMSADetector::Triggered && !detector->isAcquisitionFinished())
			return;
	}
	// OK, everyone's ready to go.
	// All acquisition now complete.
	onAllAcquisitionFinished();
}

void AMSA1DScanController::onAllAcquisitionFinished()
{
	// Disconnect signals from all detectors
	foreach(AMSADetector* detector, detectors_)
		disconnect(detector, SIGNAL(acquisitionFinished(bool)), this, SLOT(onDetectorAcquisitionFinished(bool)));

	if(detectorAcquisitionFailed_) {
		AMErrorMon::alert(this, -16, QString("Acquisition failed for one or more detectors. Please report this problem to your beamline's software developers."));
	}

	// insert one row for the new data here.
	scan_->rawData()->beginInsertRows(1, -1);

	// Insert all values into the data store
	AMnDIndex scanIndex(scan_->rawData()->scanSize(0)-1);
	for(int i=0, cc=detectors_.count(); i<cc; ++i) {
		AMSADetector* det = detectors_.at(i);
		if(det->rank() == 0)
			scan_->rawData()->setValue(scanIndex, i, AMnDIndex(), det->value(AMnDIndex()));

		// multi-dimensional:
		else {
			const double* data = det->data();
			if(data)
				scan_->rawData()->setValue(scanIndex, i, data);
			else
				AMErrorMon::alert(this, -15, QString("Could not store data from detector '%1': it did not return valid data. Please report this bug to your beamline's software developers."));
		}
	}

	scan_->rawData()->setAxisValue(0, scanIndex.i(), control_->value());	// taking the axis value from the control feedback.

	scan_->rawData()->endInsertRows();

	doNextPosition();
}

void AMSA1DScanController::cancelImplementation()
{
	foreach(AMSADetector* detector, detectors_) {
		if(detector->isAcquiring())
			detector->cancelAcquisition();
		disconnect(detector, 0, this, 0);
	}

	scan_->rawData()->endInsertRows();	// harmless if not inserting.

	// currently moving?
	if(moveAction_) {
		connect(moveAction_, SIGNAL(stateChanged(int,int)), this, SLOT(onMoveActionStateChangedWhileCancelling()));
		moveAction_->cancel();
	}
	else {
		setCancelled();	// we can be done right now.
	}
}

void AMSA1DScanController::onMoveActionStateChangedWhileCancelling()
{
	if(moveAction_->inFinalState()) {
		disconnect(moveAction_, 0, this, 0);
		moveAction_->deleteLater();
		moveAction_ = 0;

		setCancelled();
	}
}








