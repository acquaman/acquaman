/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMSA1DScanController.h"

#include "dataman/AMXASScan.h"
#include "beamline/AMControl.h"
#include "acquaman/AMSADetector.h"
#include "acquaman/AMRegionScanConfiguration.h"
#include "dataman/datastore/AMCDFDataStore.h"
#include "dataman/AMUser.h"

#include <QStringBuilder>

AMSA1DScanController::AMSA1DScanController(AMControl *control, QList<AMSADetector *> detectors, AMRegionScanConfiguration *scanConfig, QObject* parent)
	: AMScanController(scanConfig, parent)
{
	control_ = control;
	detectors_ = detectors;
	regionScanConfig_ = scanConfig;

	connectionTimeoutSeconds_ = 3;

	currentRegion_ = -1;
	currentStepInRegion_ = -1;

	stepsCompleted_ = -1;

	initializingDetectors_ = false;
	triggeringDetectors_ = false;

//	moveAction_ = 0;

	// create our new scan object
	scan_ = new AMXASScan();
	initializeScanMetaData();

	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
	scan_->setFileFormat("amCDFv1");
	scan_->replaceRawDataStore((dataStore_ = new AMCDFDataStore(AMUserSettings::userDataFolder % "/" % scan_->filePath(), false)));

	// set the scan configuration within the scan:
	scan_->setScanConfiguration(regionScanConfig_);
	///////////////////////////
}

AMSA1DScanController::~AMSA1DScanController() {
//	delete moveAction_;
//	moveAction_ = 0;
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
	if(!control()) {
		AMErrorMon::alert(this, -11, "Could not start the scan because the control to move was not valid. Please report this bug to your beamline's software developers.");
		return false;
	}

	// Create scan axis
	scan_->rawData()->addScanAxis(AMAxisInfo(control()->name(), 0, control()->description(), control()->units()));

	// Create measurements in the data store for each detector.
	foreach(AMSADetector* detector, detectors_) {
		if(!scan_->rawData()->addMeasurement(detector->measurementInfo())) {
			AMErrorMon::alert(this, -10, QString("Could not create a measurement in the data store for detector '%1', possibly because another with that name already exists. Please report this bug to your beamline's software developers.").arg(detector->name()));
			return false;
		}
	}
	if(!scan_->rawData()->addMeasurement(AMMeasurementInfo(control()->name() % "_fbk", control()->description() % " feedback", control()->units()))) {
		AMErrorMon::alert(this, -10, QString("Could not create a measurement in the data store for the control feedback '%1', possibly because another with that name already exists. Please report this bug to your beamline's software developers.").arg(control()->name() % "_fbk"));
		return false;
	}

	// Create raw data sources (by default, one for each detector, and for the control feedback)
	createRawDataSources();
	// create analysis blocks as defined here:
	createAnalysisBlocks();

	// Wait (if necessary) for all detectors and the control to be connected. For long-lived controls and detectors, this will usually be instant.
	connect(control(), SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	foreach(AMSADetector* detector, detectors_)
		connect(detector, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	QTimer::singleShot(connectionTimeoutSeconds_*1000, this, SLOT(onConnectionTimeout()));
	reviewConnected();

	return true;
}

void AMSA1DScanController::onDetectorInitialized(bool succeeded)
{
	if(!succeeded)
		detectorInitializationFailed_ = true;

	// Don't proceed with this check until all detectors have been told to init(). If we're still inside initializeImplementation(), don't do anything for now.
	if(initializingDetectors_)
		return;

	// Find out if all are finished:
	foreach(AMSADetector* detector, detectors_) {
		if(!detector->initializationFinished())
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
	computeTotalSteps();

	timeStarted_ = QDateTime::currentDateTime();
	setStarted();

	doNextPosition();

	return true;
}

void AMSA1DScanController::doNextPosition()
{
	currentStepInRegion_++;
	stepsCompleted_++;

	if(currentRegion_<0 || currentPosition() > regionScanConfig_->regionEnd(currentRegion_)) { // Entering new region.
		currentRegion_++;
		currentStepInRegion_ = 0;

		if(currentRegion_ == regionScanConfig_->regionCount()) { // all done!
			flushRawData();
			emit progress(totalSteps_, totalSteps_);
			customCleanupImplementation();
			return;
		}

		// set acquisition time for this region.
		double acquisitionTime = currentAcquisitionTime();
		foreach(AMSADetector* detector, detectors_)
			detector->setAcquisitionTime(acquisitionTime);
	}

	emitProgressUpdate();


	// move to the position.
//	moveAction_ = new AMInternalControlMoveAction(control(), currentPosition());
//	connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onMoveActionSucceeded()));
//	connect(moveAction_, SIGNAL(failed()), this, SLOT(onMoveActionFailed()));
//	moveAction_->start();
}

double AMSA1DScanController::currentPosition() const
{
	return regionScanConfig_->regionStart(currentRegion_) + regionScanConfig_->regionDelta(currentRegion_)*currentStepInRegion_;
}

void AMSA1DScanController::onMoveActionSucceeded()
{
//	disconnect(moveAction_, 0, this, 0);
//	moveAction_->deleteLater();
//	moveAction_ = 0;

	doNextAcquisition();
}

void AMSA1DScanController::onMoveActionFailed()
{
//	disconnect(moveAction_, 0, this, 0);
//	moveAction_->deleteLater();
//	moveAction_ = 0;

	AMErrorMon::alert(this, -14, QString("The scan failed because the control (%1)failed to move properly to %2 %3.").arg(control()->description()).arg(currentPosition()).arg(control()->units()));
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
		if(detector->type() == AMSADetector::Triggered && !detector->lastAcquisitionFinished())
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
	// add control feedback:
	scan_->rawData()->setValue(scanIndex, detectors_.count(), AMnDIndex(), control()->value());
	// and the axis value: the current target position
	scan_->rawData()->setAxisValue(0, scanIndex.i(), currentPosition());	// taking the axis value from the control feedback.


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
	flushRawData();

	// currently moving?
//	if(moveAction_) {
//		connect(moveAction_, SIGNAL(stateChanged(int,int)), this, SLOT(onMoveActionStateChangedWhileCancelling()));
//		moveAction_->cancel();
//	}
//	else {
//		customCleanupImplementation();
//	}
}

void AMSA1DScanController::onMoveActionStateChangedWhileCancelling()
{
//	if(moveAction_->inFinalState()) {
//		disconnect(moveAction_, 0, this, 0);
//		moveAction_->deleteLater();
//		moveAction_ = 0;

//		customCleanupImplementation();
//	}
}

void AMSA1DScanController::setCustomInitializationFinished(bool succeeded)
{
	if(state() != Initializing)
		return;

	if(succeeded)
		internalInitializeImplementation();
	else
		setFailed();
}

void AMSA1DScanController::internalInitializeImplementation()
{
	// initialize detectors:
	detectorInitializationFailed_ = false;
	initializingDetectors_ = true;
	foreach(AMSADetector* detector, detectors_) {
		connect(detector, SIGNAL(initialized(bool)), this, SLOT(onDetectorInitialized(bool)));
		if(!detector->init()) {
			AMErrorMon::alert(this, -12, QString("Could not start the scan because the detector '%1' could not be initialized. Please report this bug to your beamline's software developers.").arg(detector->description()));
			setFailed();
			return;
		}
	}
	initializingDetectors_ = false;
	onDetectorInitialized();
}

void AMSA1DScanController::setCustomCleanupFinished()
{
	if(state() == Cancelling) {
		setCancelled();
	}
	else if(state() == Running) {
		setFinished();
	}
}

void AMSA1DScanController::flushRawData()
{
	if(!dataStore_->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 38, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}

void AMSA1DScanController::emitProgressUpdate()
{
	emit progress(stepsCompleted_, totalSteps_);

	double secondsElapsed = timeStarted_.msecsTo(QDateTime::currentDateTime()) / 1000.0;
	double estimatedTotalSeconds = secondsElapsed / (double(stepsCompleted_)/totalSteps_);

	emit timeRemaining(estimatedTotalSeconds - secondsElapsed);
}

void AMSA1DScanController::computeTotalSteps()
{
	// We'll need to modify currentRegion_ and currentStepInRegion_ before calling currentPosition(). Remember old values to avoid side-effects for this function.
	int restoreCurrentRegion = currentRegion_;
	int restoreCurrentStep = currentStepInRegion_;

	currentRegion_ = 0;
	currentStepInRegion_ = 0;
	totalSteps_ = 0;

	while(currentRegion_ < regionScanConfig_->regionCount()) {
		currentStepInRegion_++;
		totalSteps_++;

		if(currentPosition() > regionScanConfig_->regionEnd(currentRegion_)) { // Entering new region.
			currentRegion_++;
			currentStepInRegion_ = 0;
		}
	}

	// totalSteps_ is now ready.

	// Restore to avoid side effects:
	currentRegion_ = restoreCurrentRegion;
	currentStepInRegion_ = restoreCurrentStep;
}

double AMSA1DScanController::currentAcquisitionTime() const
{
	return regionScanConfig_->regionTime(currentRegion_);
}

void AMSA1DScanController::reviewConnected()
{
	// Are they all connected?
	if(!control()->canMove())
		return;
	foreach(AMSADetector* detector, detectors_)
		if(!detector->isConnected())
			return;

	// OK, everyone's ready.
	disconnect(control(), SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	foreach(AMSADetector* detector, detectors_)
		disconnect(detector, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));

	// At this point, give custom subclasses a chance to do their own initialization.
	if(!customInitializeImplementation()) {
		setFailed();
	}
}

void AMSA1DScanController::onConnectionTimeout()
{
	if(state() != Initializing)
		return;

	disconnect(control(), SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	foreach(AMSADetector* detector, detectors_)
		disconnect(detector, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));

	bool allConnected = control()->canMove();
	foreach(AMSADetector* detector, detectors_)
		allConnected &= detector->isConnected();

	if(!allConnected) {
		AMErrorMon::alert(this, -111, QString("Could not start the scan because either the control or the detectors were not connected after %1 seconds. Please report this problem to the beamline staff.").arg(connectionTimeoutSeconds_));
		setFailed();
	}
}

void AMSA1DScanController::initializeScanMetaData()
{
	QString scanName = regionScanConfig_->userScanName();
	if(scanName.isEmpty())
		scanName = regionScanConfig_->autoScanName();
	scan_->setName(scanName);
	// sample? number?
	scan_->setRunId(AMUser::user()->currentRunId());
}








