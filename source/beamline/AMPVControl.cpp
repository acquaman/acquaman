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

#include "AMPVControl.h"

#include <QDebug>

#include "util/AMErrorMonitor.h"

// Class AMReadOnlyPVControl
///////////////////////////////////////

 AMReadOnlyPVControl::~AMReadOnlyPVControl(){}
AMReadOnlyPVControl::AMReadOnlyPVControl(const QString& name, const QString& readPVname, QObject* parent, const QString description)
	: AMControl(name, "?", parent, description)  {

	wasConnected_ = false;
	readPV_ = new AMProcessVariable(readPVname, true, this);

	lowLimitPV_ = 0;
	highLimitPV_ = 0;

	allowLowLimitValuePVUpdates_ = true;
	allowHighLimitValuePVUpdates_ = true;

	lowLimitValue_ = -1;
	highLimitValue_ = -1;

	connect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(readPV_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)));
	connect(readPV_, SIGNAL(readReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SIGNAL(readConnectionTimeoutOccurred()));
	connect(readPV_, SIGNAL(error(int)), this, SLOT(onReadPVError(int)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	connect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));

	// If the readPV_ is already initialized as soon as we create it [possible if it's sharing an existing connection], we'll never get the inialized() signal, do it here now:
	wasConnected_ = readPV_->readReady();	// same as isConnected(), but we cannot call virtual functions from a constructor, potentially breaks subclasses.
	if(readPV_->isInitialized())
		onReadPVInitialized();

}


void AMReadOnlyPVControl::enableLimitMonitoring()
{
	// ReadOnlyPV uses the graph limits (ie LOPR and HOPR)
	lowLimitPV_ = new AMProcessVariable(QString("%1.LOPR").arg(readPV_->pvName()), true, this);
	highLimitPV_ = new AMProcessVariable(QString("%1.HOPR").arg(readPV_->pvName()), true, this);

	connect(lowLimitPV_, SIGNAL(initialized()), this, SLOT(onLowLimitPVValueChanged()));
	connect(lowLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onLowLimitPVValueChanged()));

	connect(highLimitPV_, SIGNAL(initialized()), this, SLOT(onHighLimitPVValueChanged()));
	connect(highLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onHighLimitPVValueChanged()));
}

// This will work for subclasses with more PVs, since it checks virtual isConnected(). Subclasses should reimplement isConnected() to specify what counts as connected.
void AMReadOnlyPVControl::onPVConnected(bool) {

	bool nowConnected = isConnected();

	// Do change detection so that we don't emit disconnected() when already disconnected()...
	if(wasConnected_ != nowConnected)
		emit connected(wasConnected_ = nowConnected);
}

void AMReadOnlyPVControl::onReadPVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if( (errorCode == AMPROCESSVARIABLE_CANNOT_READ) && shouldMeasure() ){
			emit error(AMControl::CannotReadError);
			AMErrorMon::debug(this, AMPVCONTROL_READ_PROCESS_VARIABLE_ERROR, QString("AMReadOnlyPVControl: Read Process Variable error %1: code %2.").arg(source->pvName()).arg(errorCode));
		}
	}
}

void AMReadOnlyPVControl::setMinimumValueOverride(double newValue)
{
	setLowLimitValue(newValue);
	allowLowLimitValuePVUpdates_ = false;
}

void AMReadOnlyPVControl::setMaximumValueOverride(double newValue)
{
	setHighLimitValue(newValue);
	allowHighLimitValuePVUpdates_ = false;
}

void AMReadOnlyPVControl::setLowLimitValue(double newLowLimit)
{
	lowLimitValue_ = newLowLimit;
	emit minimumValueChanged(lowLimitValue_);
}

void AMReadOnlyPVControl::setHighLimitValue(double newHighLimit)
{
	highLimitValue_ = newHighLimit;
	emit maximumValueChanged(highLimitValue_);
}

void AMReadOnlyPVControl::onReadPVInitialized() {
	setUnits(readPV_->units());	// copy over the new unit string
	setEnumStates(readPV_->enumStrings());
	setDisplayPrecision(readPV_->displayPrecision());

	if (allowLowLimitValuePVUpdates_)
		setLowLimitValue(readPV_->lowerGraphicalLimit());

	if (allowHighLimitValuePVUpdates_)
		setHighLimitValue(readPV_->upperGraphicalLimit());
}

void AMReadOnlyPVControl::onLowLimitPVValueChanged()
{
	if (lowLimitPV_ && lowLimitPV_->isInitialized() && allowLowLimitValuePVUpdates_)
		setLowLimitValue(lowLimitPV_->getDouble());
}

void AMReadOnlyPVControl::onHighLimitPVValueChanged()
{
	if (highLimitPV_ && highLimitPV_->isInitialized() && allowHighLimitValuePVUpdates_)
		setHighLimitValue(highLimitPV_->getDouble());
}

 AMPVControl::~AMPVControl(){}
AMPVControl::AMPVControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& stopPVname, QObject* parent, double tolerance, double completionTimeoutSeconds, int stopValue, const QString &description)
	: AMReadOnlyPVControl(name, readPVname, parent, description)
{
	setTolerance(tolerance);
	allowsMovesWhileMoving_ = true;

	//not moving yet:
	moveInProgress_ = false;

	// not connected:
	wasConnected_ = false;

	// setpoint is initialized:
	setpoint_ = 0;

	// this is what to use for our timeout:
	completionTimeout_ = completionTimeoutSeconds;

	// connect the timer to the timeout handler:
	connect(&completionTimer_, SIGNAL(timeout()), this, SLOT(onCompletionTimeout()));

	// process variable:
	writePV_ = new AMProcessVariable(writePVname, true, this);
	// instead of connected(), use writeRead: connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)))
	connect(writePV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onWritePVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeoutOccurred()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
	connect(writePV_, SIGNAL(valueChanged(double)), this, SLOT(onSetpointChanged(double)));
	connect(writePV_, SIGNAL(initialized()), this, SLOT(onWritePVInitialized()));

	// We now need to monitor the feedback position ourselves, to see if we get where we want to go:
	connect(readPV_, SIGNAL(valueChanged(double)), this, SLOT(onNewFeedbackValue(double)));

	// Do we have a stopPV?
	noStopPV_ = stopPVname.isEmpty();
	if(noStopPV_) {
		stopPV_ = 0;
	}
	else {
		stopPV_ = new AMProcessVariable(stopPVname, false, this);
		connect(stopPV_, SIGNAL(error(int)), this, SLOT(onReadPVError(int)));	/// \todo Does this need separate error handling? What if the stop write fails? That's really important.
	}
	stopValue_ = stopValue;


	// If any PVs are already connected [possible if they're sharing an existing connection]:
	wasConnected_ = (readPV_->readReady() && writePV_->writeReady());	// equivalent to isConnected(), but we cannot call virtual functions inside a constructor, that will break subclasses.
	if(writePV_->isInitialized())
		onWritePVInitialized();
}

void AMPVControl::enableLimitMonitoring()
{
	// PVControl uses the control limits (ie DRVL and DRVH)
	lowLimitPV_ = new AMProcessVariable(QString("%1.DRVL").arg(writePV_->pvName()), true, this);
	highLimitPV_ = new AMProcessVariable(QString("%1.DRVH").arg(writePV_->pvName()), true, this);

	connect(lowLimitPV_, SIGNAL(initialized()), this, SLOT(onLowLimitPVValueChanged()));
	connect(lowLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onLowLimitPVValueChanged()));

	connect(highLimitPV_, SIGNAL(initialized()), this, SLOT(onHighLimitPVValueChanged()));
	connect(highLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onHighLimitPVValueChanged()));
}

void AMPVControl::onSetpointChanged(double newVal)
{
	setpoint_ = newVal;
	emit setpointChanged(setpoint_);
}

void AMPVControl::onReadPVInitialized()
{
	// Same as read only PV control's except we don't get grab the limits from here.
	setUnits(readPV_->units());	// copy over the new unit string
	setEnumStates(readPV_->enumStrings());
	setDisplayPrecision(readPV_->displayPrecision());
}

void AMPVControl::onWritePVInitialized() {
	if (allowLowLimitValuePVUpdates_)
		setLowLimitValue(writePV_->lowerControlLimit());

	if (allowHighLimitValuePVUpdates_)
		setHighLimitValue(writePV_->upperControlLimit());

	setMoveEnumStates(writePV_->enumStrings());
}

// Start a move to the value setpoint:
AMControl::FailureExplanation AMPVControl::move(double setpoint) {

	if(isMoving()) {
		if(!allowsMovesWhileMoving()) {
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING, QString("AMPVControl: Could not move %1 (%2) to %3, because the control is already moving.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return AlreadyMovingFailure;
		}

		// assuming this control can accept mid-move updates. We just need to update our setpoint and send it.

		if(!canMove()) {	// this would be rare: a past move worked, but now we're no longer connected?
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_BASED_ON_CANMOVE, QString("AMPVControl: Could not move %1 (%2) to %3.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return NotConnectedFailure;
		}

		// Set the move target.

		setpoint_ = setpoint;

		// Set the move progress values.

		updateMoveProgressMinimum();
		updateMoveProgressValue();
		updateMoveProgressMaximum();

		// Initiate move.

		writePV_->setValue(setpoint_);

		// Start the completion timer.

		completionTimer_.start(int(completionTimeout_*1000.0)); // restart the completion timer... Since this might be another move, give it some more time.

		// re-targetted moves will emit moveReTargetted(), although no moveSucceeded()/moveFailed() will be issued for the first move.
		emit moveReTargetted();

		// check for done:
		if(inPosition()) {
			completionTimer_.stop();
			emit movingChanged(moveInProgress_ = false);
			emit moveSucceeded();
		}
	}

	// Regular case: start of a new move.
	else {
		// kill any old countdowns:
		completionTimer_.stop();

		if(!canMove()) {
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_BASED_ON_CANMOVE, QString("AMPVControl: Could not move %1 (%2) to %3.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return NotConnectedFailure;
		}

		// new move target:
		setpoint_ = setpoint;

		// Set the move progress values.

		updateMoveProgressMinimum();
		updateMoveProgressValue();
		updateMoveProgressMaximum();

		// Issue the move, check on attemptMoveWhenWithinTolerance
		if(!attemptMoveWhenWithinTolerance_ && inPosition()){
			emit moveSucceeded();
		}
		else{
			writePV_->setValue(setpoint_);

			// We're now moving! Let's hope this control makes it... (No way to actually check.)
			emit movingChanged(moveInProgress_ = true);
			// emit the signal that we started:
			emit moveStarted();

			// Are we in-position? [With the default tolerance of AMCONTROL_TOLERANCE_DONT_CARE, we will always be in-position, and moves will complete right away, that's the intended behaviour, because we have no other way of knowing when they'll finish.]
			if(inPosition()) {
				emit movingChanged(moveInProgress_ = false);
				emit moveSucceeded();
			}
			else {
				// start the countdown to see if we get there in time or stall out: (completionTimeout_ is in seconds)
				completionTimer_.start(int(completionTimeout_*1000.0));
			}
		}
	}
	return NoFailure;
}

// Start a move to the value setpoint:
AMControl::FailureExplanation AMPVControl::move(const QString &stringSetPoint)
{
	writePV_->setValue(stringSetPoint);
	return NoFailure;
}

// This is used to check every new value, to see if we entered tolerance:
void AMPVControl::onNewFeedbackValue(double) {

	// If we're not in the middle of a move, don't really care about changing values.
	if(!moveInProgress_)
		return;

	// Did we make it?
	if( inPosition() ) {
		// move is now done:
		emit movingChanged(moveInProgress_ = false);
		// disable the timer, so it doesn't trigger an error later
		completionTimer_.stop();
		// let everyone know we succeeded:
		emit moveSucceeded();

	}

	// Update the move progress value.

	updateMoveProgressValue();
}

// This is used to handle errors from the write pv
void AMPVControl::onWritePVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if(errorCode == AMPROCESSVARIABLE_CANNOT_WRITE && shouldMove() ){
			emit error(AMControl::CannotWriteError);
			AMErrorMon::debug(this, AMPVCONTROL_WRITE_PROCESS_VARIABLE_ERROR, QString("AMPVControl: Write Process Variable Error %1: code %2.").arg(source->pvName()).arg(errorCode));
		}
	}
}

// This is used to handle the timeout of a move:
void AMPVControl::onCompletionTimeout() {

	// if we weren't moving, this shouldn't have happened. someone forgot to shutoff the timer?
	// todo: this is only included for state testing debugging... can remove if never happens
	if(!moveInProgress_) {
		return;
	}

	// No matter what, this move is over:
	emit movingChanged(moveInProgress_ = false);
	completionTimer_.stop();

	// Did we make it?
	if( inPosition() ) {
		emit moveSucceeded();
	}
	// Didn't make it into position:
	else {
		emit moveFailed(AMControl::TimeoutFailure);
	}
}


// Class AMSinglePVControl
///////////////////////////////////////

 AMSinglePVControl::~AMSinglePVControl(){}
AMSinglePVControl::AMSinglePVControl(const QString &name, const QString &PVname, QObject *parent, double tolerance, double completionTimeoutSeconds, const QString &description)
	: AMPVControl(name, PVname, PVname, QString(), parent, tolerance, completionTimeoutSeconds, 1, description)
{
}


// Class AMReadOnlyPVwStatusControl
///////////////////////////////////////

AMReadOnlyPVwStatusControl::AMReadOnlyPVwStatusControl(const QString& name, const QString& readPVname, const QString& movingPVname, QObject* parent, AMAbstractControlStatusChecker* statusChecker, const QString &description)
	: AMReadOnlyPVControl(name, readPVname, parent, description)
{
	// Initializing:
	statusChecker_ = statusChecker;
	wasMoving_ = false;

	// Create the movingPV and hook it up:
	movingPV_ = new AMProcessVariable(movingPVname, true, this);
	connect(movingPV_, SIGNAL(valueChanged(int)), this, SLOT(onMovingChanged(int)));
	connect(movingPV_, SIGNAL(readReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(movingPV_, SIGNAL(error(int)), this, SLOT(onStatusPVError(int)));
	connect(movingPV_, SIGNAL(connectionTimeout()), this, SIGNAL(movingConnectionTimeoutOccurred()));
	connect(movingPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	// If any PVs were already connected on creation [possible if sharing an existing connection]:
	wasConnected_ = (readPV_->readReady() && movingPV_->readReady());	// equivalent to isConnected(), but we cannot call virtual functions inside a constructor, potentially breaks subclasses.
	if(movingPV_->readReady())
		wasMoving_ = (*statusChecker_)((int)movingPV_->lastValue());

}


// This is used to handle errors from the status pv
void AMReadOnlyPVwStatusControl::onStatusPVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if(errorCode == AMPROCESSVARIABLE_CANNOT_READ){
			emit error(AMControl::CannotGetStatusError);
			AMErrorMon::debug(this, AMPVCONTROL_STATUS_PROCESS_VARIABLE_ERROR, QString("AMReadOnlyPVwStatusControl: Status Process Variable error %1: code %2.").arg(source->pvName()).arg(errorCode));
		}
	}
}

// This is called whenever there is an update from the move status PV
// In it we simply do change detection of the moving status, and emit signals if it has changed.
void AMReadOnlyPVwStatusControl::onMovingChanged(int movingValue) {

	bool nowMoving = (*statusChecker_)(movingValue);

	if(wasMoving_ != nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);
}

 AMPVwStatusControl::~AMPVwStatusControl(){}
AMPVwStatusControl::AMPVwStatusControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname, const QString& stopPVname, QObject* parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker* statusChecker, int stopValue, const QString &description)
	: AMReadOnlyPVwStatusControl(name, readPVname, movingPVname, parent, statusChecker, description) {

	// Initialize:
	moveInProgress_ = false;
	stopInProgress_ = false;
	startInProgress_ = false;
	settlingInProgress_ = false;
	settlingTime_ = 0.0;	/// \todo Once tested, this should maybe be enabled by default. All systems with separate status and feedback PVs will need it. How much time?
	setTolerance(tolerance);
	setpoint_ = 0;
	moveStartTimeout_ = moveStartTimeoutSeconds;
	hardwareWasMoving_ = false;

	// create new setpoint PV. Monitor it, in case someone else changes it
	writePV_ = new AMProcessVariable(writePVname, true, this);

	// connect:
	// use writeReadyChanged() instead of connected() here: connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)))
	connect(writePV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onWritePVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeoutOccurred()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
	connect(writePV_, SIGNAL(valueChanged(double)), this, SLOT(onSetpointChanged(double)));
	connect(writePV_, SIGNAL(initialized()), this, SLOT(onWritePVInitialized()));

	// connect the timer to the timeout handler:
	connect(&moveStartTimer_, SIGNAL(timeout()), this, SLOT(onMoveStartTimeout()));
	connect(&settlingTimer_, SIGNAL(timeout()), this, SLOT(onSettlingTimeFinished()));

	// Do we have a stopPV?
	noStopPV_ = stopPVname.isEmpty();
	if(noStopPV_) {
		stopPV_ = 0;
	}
	else {
		stopPV_ = new AMProcessVariable(stopPVname, false, this);
		connect(stopPV_, SIGNAL(error(int)), this, SLOT(onReadPVError(int)));	/// \todo Does this need separate error handling? What if the stop write fails? That's really important.
	}
	stopValue_ = stopValue;


	// If any PVs were already connected on creation [possible if sharing an existing connection]:
	wasConnected_ = (readPV_->readReady() && writePV_->writeReady() && movingPV_->readReady());	// equivalent to isConnected(), but we cannot call virtual functions from the constructor, potentially breaks subclasses.
	if(writePV_->isInitialized())
		setMoveEnumStates(writePV_->enumStrings());
	if(movingPV_->readReady())
		hardwareWasMoving_ = (*statusChecker_)((int)movingPV_->lastValue());

}

void AMPVwStatusControl::enableLimitMonitoring()
{
	// AMPVwStatusControl uses the control limits (ie DRVL and DRVH)
	lowLimitPV_ = new AMProcessVariable(QString("%1.DRVL").arg(writePV_->pvName()), true, this);
	highLimitPV_ = new AMProcessVariable(QString("%1.DRVH").arg(writePV_->pvName()), true, this);

	connect(lowLimitPV_, SIGNAL(initialized()), this, SLOT(onLowLimitPVValueChanged()));
	connect(lowLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onLowLimitPVValueChanged()));

	connect(highLimitPV_, SIGNAL(initialized()), this, SLOT(onHighLimitPVValueChanged()));
	connect(highLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onHighLimitPVValueChanged()));
}

void AMPVwStatusControl::onSetpointChanged(double newVal)
{
	setpoint_ = newVal;
	emit setpointChanged(setpoint_);
}

void AMPVwStatusControl::onReadPVInitialized()
{
	// Same as read only PV control's except we don't get grab the limits from here.
	setUnits(readPV_->units());	// copy over the new unit string
	setEnumStates(readPV_->enumStrings());
	setDisplayPrecision(readPV_->displayPrecision());
}

void AMPVwStatusControl::onWritePVInitialized() {
	if (allowLowLimitValuePVUpdates_)
		setLowLimitValue(writePV_->lowerControlLimit());

	if (allowLowLimitValuePVUpdates_)
		setHighLimitValue(writePV_->upperControlLimit());

	setMoveEnumStates(writePV_->enumStrings());
}

// Start a move to the value setpoint:
AMControl::FailureExplanation AMPVwStatusControl::move(double Setpoint) {

	if(isMoving()) {
		if(!allowsMovesWhileMoving()) {
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING, QString("AMPVwStatusControl: Could not move %1 (%2) to %3, because the control is already moving.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return AlreadyMovingFailure;
		}

		if(!moveInProgress()) {
			// the control is already moving, but it's not one of our moves. In this situation, there is no way that we can start a move and be assured that we'll be notified when OUR move finishes.
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING_EXTERNAL, QString("AMPVwStatusControl: Could not move %1 (%2) to %3, because the control is already moving.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return AlreadyMovingFailure;
		}

		// Otherwise: This control supports mid-move updates, and we're already moving. We just need to update the setpoint and send it.
		setpoint_ = Setpoint;

		// Update the move progress values.
		updateMoveProgressMinimum();
		updateMoveProgressValue();
		updateMoveProgressMaximum();

		writePV_->setValue(setpoint_);

		// since the settling phase is considered part of a move, it's OK to be here while settling... But for Acquaman purposes, this will be considered a single re-targetted move, even though the hardware will see two.  If we're settling, disable the settling timer, because we only want to respond to the end of the second move.
		if(settlingInProgress_) {
			settlingInProgress_ = false;
			settlingTimer_.stop();
		}

		emit moveReTargetted(); // re-targetted moves will emit moveReTargetted(), although no moveSucceeded()/moveFailed() will be issued for the initial move.
	}

	else {
		settlingInProgress_ = false;
		stopInProgress_ = false;
		moveInProgress_ = false;
		// Flag that "our" move started:
		startInProgress_ = true;

		// This is our new target:
		setpoint_ = Setpoint;

		// Update the move progress values.
		updateMoveProgressMinimum();
		updateMoveProgressValue();
		updateMoveProgressMaximum();

		// Normal move:
		// Issue the move command, check on attemptMoveWhenWithinTolerance
		if(!attemptMoveWhenWithinTolerance_ && inPosition())
			emit moveSucceeded();
		else{
			writePV_->setValue(setpoint_);
			// start the timer to check if our move failed to start:
			moveStartTimer_.start(int(moveStartTimeout_*1000.0));
		}
	}

	return NoFailure;
}

// Tell the motor to stop.  (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
bool AMPVwStatusControl::stop() {
	if(!canStop())
		return false;

	stopPV_->setValue(stopValue_);
	stopInProgress_ = true;	// flag that a stop is "in progress" -- we've issued the stop command.
	moveInProgress_ = false;	// one of "our" moves is no longer in progress.
	startInProgress_ = false;
	settlingInProgress_ = false;
	return true;
}

void AMPVwStatusControl::setSettlingTime(double seconds)
{
	if (settlingTime_ != seconds) {
		settlingTime_ = seconds;
		emit settlingTimeChanged(settlingTime_);
	}
}

// This is used to handle errors from the status pv
void AMPVwStatusControl::onWritePVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if(errorCode == AMPROCESSVARIABLE_CANNOT_WRITE && shouldMove()){
			emit error(AMControl::CannotWriteError);
			AMErrorMon::debug(this, AMPVCONTROL_STATUS_WRITE_PROCESS_VARIABLE_ERROR, QString("AMPVwStatusControl: Write Process Variable error %1: %2.").arg(source->pvName()).arg(errorCode));
		}
	}
}

// This is used to handle the timeout of a move start:
void AMPVwStatusControl::onMoveStartTimeout() {

	moveStartTimer_.stop();

	// This is only meaningful if one of our moves is in progress.
	if(startInProgress_) {
		// give up on this move:
		startInProgress_ = false;

		// The move didn't start within our allowed start period. That counts as a move failed.
		emit moveFailed(AMControl::TimeoutFailure);
	}
}

// Re-implemented from AMReadOnlyPVwStatusControl:
void AMPVwStatusControl::onMovingChanged(int isMovingValue) {

	bool nowMoving = (*statusChecker_)(isMovingValue);	// according to the hardware.  For checking moveSucceeded/moveStarted/moveFailed, use the value delivered in the signal argument, instead of re-checking the PV, in case we respond late and the hardware has already changed again.

	// In case the hardware is being silly and sending multiple MOVE ACTIVE, MOVE ACTIVE, MOVE ACTIVE states in a row, or MOVE DONE, MOVE DONE, MOVE DONE states in a row: only act on changes. [Edge detection]
	if(nowMoving == hardwareWasMoving_)
		return;

	hardwareWasMoving_ = nowMoving;

	// moveStarted, moveFailed, moveSucceeded, or transition to settling:
	///////////////////////////////////////////////////////////////////////

	// if we requested one of our moves, and moving just started:
	if(startInProgress_ && nowMoving) {
		moveInProgress_ = true;
		startInProgress_ = false;
		// This is great... the device started moving within the timeout:

		// disable the moveStartTimer, we don't need it anymore
		moveStartTimer_.stop();

		emit moveStarted();
	}

	// If one of our moves was running, and we stopped moving:
	if(moveInProgress_ && !nowMoving) {

		// Mode 1: No settling:
		if( settlingTime_ == 0.0) {
			// That's the end of our move
			moveInProgress_ = false;

			// Check if we succeeded...
			if(inPosition()) {
				emit moveSucceeded();

			} else {
				emit moveFailed(AMControl::ToleranceFailure);

			}
		}
		// Mode 2: allow settling
		else {
			if(!settlingInProgress_) {
				settlingInProgress_ = true;
				settlingTimer_.start(int(settlingTime_*1000)); // QTimer uses millisecond time intervals.
			}
		}
	}

	// "sucessfully" stopped due to a stop() command.
	if(stopInProgress_ && !nowMoving) {
		stopInProgress_ = false;
		// but the move itself has failed, due to a stop() intervention.
		emit moveFailed(AMControl::WasStoppedFailure);
	}



	// Emitting movingChanged().
	/////////////////////////////////////

	// For external purposes, isMoving() depends on whether the hardware says we're moving, or we're in the settling phase.
	nowMoving = isMoving();

	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);
}


void AMPVwStatusControl::onSettlingTimeFinished()
{
	settlingTimer_.stop();

	if(!settlingInProgress_) {
		// temporary, for settling state testing:
		qWarning() << "AMPVwStatusControl:" << name() << ": Settling timeout while settlingInProgress is not true.  This should never happen; please report this bug.";
		return;
	}

	settlingInProgress_ = false;

	if(moveInProgress_) {
		moveInProgress_ = false;
		// Check if we succeeded...
		if(inPosition()) {
			emit moveSucceeded();
		}
		else {
			emit moveFailed(AMControl::ToleranceFailure);
		}
	}
	else {
		// temporary, for settling state testing:
		qWarning() << "AMPVwStatusControl: " << name() << ": Settling time reached while moveInProgress_  == false. This should never happen; please report this bug.";
	}

	// possible that a change in settlingInProgress_ caused isMoving() to change. Emit signal if necessary:
	bool nowMoving = isMoving();
	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);
}

/// ========================= implementation of AMPVwStatusAndUnitConversionControl =================
AMPVwStatusAndUnitConversionControl::AMPVwStatusAndUnitConversionControl(const QString &name, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &stopPVname, AMAbstractUnitConverter *readUnitConverter, AMAbstractUnitConverter *writeUnitConverter, QObject *parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description) :
	AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{
	readConverter_ = readUnitConverter;
	writeConverter_ = writeUnitConverter;

	disconnect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(readPV_, SIGNAL(valueChanged(double)), this, SLOT(onReadPVValueChanged(double)));

	disconnect(writePV_, SIGNAL(valueChanged(double)), this, SIGNAL(setpointChanged(double)));
	connect(writePV_, SIGNAL(valueChanged(double)), this, SLOT(onWritePVValueChanged(double)));

	disconnect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));	// don't allow AMReadOnlyPV to change our units or enum names.

	setUnits(readConverter_->units());

}

void AMPVwStatusAndUnitConversionControl::onReadPVValueChanged(double newValue)
{
	emit valueChanged(readUnitConverter()->convertFromRaw(newValue));
}

void AMPVwStatusAndUnitConversionControl::onWritePVValueChanged(double newValue)
{
	emit setpointChanged(writeUnitConverter()->convertFromRaw(newValue));
}

void AMPVwStatusAndUnitConversionControl::setLowLimitValue(double newLowLimit)
{
	lowLimitValue_ = writeUnitConverter()->convertFromRaw(newLowLimit);
	emit minimumValueChanged(lowLimitValue_);
}

void AMPVwStatusAndUnitConversionControl::setHighLimitValue(double newHighLimit)
{
	highLimitValue_ = writeUnitConverter()->convertFromRaw(newHighLimit);
	emit maximumValueChanged(highLimitValue_);
}

void AMPVwStatusAndUnitConversionControl::enableLimitMonitoring()
{
	// AMPVwStatusControl uses the control limits (ie DRVL and DRVH)
	lowLimitPV_ = new AMProcessVariable(QString("%1.DRVL").arg(writePV_->pvName()), true, this);
	highLimitPV_ = new AMProcessVariable(QString("%1.DRVH").arg(writePV_->pvName()), true, this);

	connect(lowLimitPV_, SIGNAL(initialized()), this, SLOT(onLowLimitPVValueChanged()));
	connect(lowLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onLowLimitPVValueChanged()));

	connect(highLimitPV_, SIGNAL(initialized()), this, SLOT(onHighLimitPVValueChanged()));
	connect(highLimitPV_, SIGNAL(valueChanged(double)), this, SLOT(onHighLimitPVValueChanged()));
}

void AMPVwStatusAndUnitConversionControl::setUnitConverters(AMAbstractUnitConverter *readUnitConverter, AMAbstractUnitConverter* writeUnitConverter)
{
	double oldValue = value();
	double oldSetpoint = setpoint();

	delete readConverter_;
	readConverter_ = readUnitConverter;
	if(writeConverter_)
		delete writeConverter_;
	writeConverter_ = writeUnitConverter;

	double newValue = value();
	double newSetpoint = setpoint();

	setUnits(readConverter_->units());

	if(newValue != oldValue)
		emit valueChanged(newValue);
	if(newSetpoint != oldSetpoint)
		emit setpointChanged(newSetpoint);
}

/// ========================= implementation of AMReadOnlyWaveformBinningSinglePVControl =================
AMReadOnlyWaveformBinningPVControl::~AMReadOnlyWaveformBinningPVControl(){}
AMReadOnlyWaveformBinningPVControl::AMReadOnlyWaveformBinningPVControl(const QString &name, const QString &readPVname, int lowIndex, int highIndex, QObject *parent, const QString &description) :
	AMReadOnlyPVControl(name, readPVname, parent, description)
{
	disconnect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	attemptDouble_ = false;
	setBinParameters(lowIndex, highIndex);
	connect(readPV_, SIGNAL(valueChanged()), this, SLOT(onReadPVValueChanged()));
}

double AMReadOnlyWaveformBinningPVControl::value() const{
	if(attemptDouble_)
		return readPV_->binFloatingPointValues(lowIndex_, highIndex_);
	return readPV_->binIntegerValues(lowIndex_, highIndex_);
}

void AMReadOnlyWaveformBinningPVControl::setBinParameters(int lowIndex, int highIndex){
	lowIndex_ = lowIndex;
	highIndex_ = highIndex;
}

void AMReadOnlyWaveformBinningPVControl::setAttemptDouble(bool attemptDouble){
	attemptDouble_ = attemptDouble;
}

void AMReadOnlyWaveformBinningPVControl::onReadPVValueChanged(){
	emit valueChanged(value());
}

/// ========================= implementation of AMWaveformBinningSinglePVControl =================
AMWaveformBinningSinglePVControl::AMWaveformBinningSinglePVControl(const QString &name, const QString &PVName, int lowIndex, int highIndex, QObject *parent, const QString &description) :
	AMSinglePVControl(name, PVName, parent)
{
	setDescription(description);
	setAttemptDouble(false);
	setBinParameters(lowIndex, highIndex);

	// reconnect the valueChanged signal for readPV_
	disconnect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(readPV_, SIGNAL(valueChanged()), this, SLOT(onReadPVValueChanged()));
}

double AMWaveformBinningSinglePVControl::value() const{
	if(attemptDouble_)
		return readPV_->binFloatingPointValues(lowIndex_, highIndex_);
	return readPV_->binIntegerValues(lowIndex_, highIndex_);
}

bool AMWaveformBinningSinglePVControl::values(int size, int *outputValues) const{
	if(attemptDouble_)
		return false;
	QVector<int> integerValues = readPV_->lastIntegerValues();
	if(integerValues.count() < size)
		return false;
	memcpy(outputValues, integerValues.constData(), size*sizeof(int));
	return true;
}

bool AMWaveformBinningSinglePVControl::values(int size, double *outputValues) const{
	if(!attemptDouble_)
		return false;
	QVector<double> doubleValues = readPV_->lastFloatingPointValues();
	if(doubleValues.count() < size)
		return false;
	memcpy(outputValues, doubleValues.constData(), size*sizeof(double));
	return true;
}

void AMWaveformBinningSinglePVControl::setBinParameters(int lowIndex, int highIndex){
	lowIndex_ = lowIndex;
	highIndex_ = highIndex;
}

void AMWaveformBinningSinglePVControl::setAttemptDouble(bool attemptDouble){
	attemptDouble_ = attemptDouble;
}

void AMWaveformBinningSinglePVControl::setValues(const QVector<int> &values){
//	qDebug() << "Attempting QVector<int>";
	if(attemptDouble_)
		return;

//	qDebug() << "Doing QVector<int> on " << writePV_->pvName();

	writePV_->setValues(((dbr_long_t*)(values.data())), values.count());
}

void AMWaveformBinningSinglePVControl::setValues(const QVector<double> &values){
//	qDebug() << "Attempting QVector<double>";
	if(!attemptDouble_)
		return;

//	qDebug() << "Doing QVector<double> on " << writePV_->pvName();

	writePV_->setValues(((dbr_double_t*)(values.data())), values.count());
}

void AMWaveformBinningSinglePVControl::onReadPVValueChanged(){
	emit valueChanged(value());
}





