/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

// Class AMReadOnlyPVControl
///////////////////////////////////////

AMReadOnlyPVControl::AMReadOnlyPVControl(const QString& name, const QString& readPVname, QObject* parent, const QString description)
	: AMControl(name, "?", parent, description)  {

	readPV_ = new AMProcessVariable(readPVname, true, this);

	connect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	//connect(readPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(readPV_, SIGNAL(readReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SIGNAL(readConnectionTimeoutOccurred()));
	connect(readPV_, SIGNAL(error(int)), this, SLOT(onReadPVError(int)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	connect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));

}

// This is written out fully (rather than simply connected SIGNALs to SIGNALs
// So that we can override it in other AMPVControls with multiple PVs.
void AMReadOnlyPVControl::onPVConnected(bool) {

	// For the read-only control, just having the readPV is enough to be connected or disconnected.
	emit connected( isConnected() );

	// don't need to do anything else here.  isConnected() and canRead() come dynamically from the PV state canRead().
}

void AMReadOnlyPVControl::onReadPVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if( (errorCode == AMPROCESSVARIABLE_CANNOT_READ) && shouldMeasure() ){
			emit error(AMControl::CannotReadError);
			qWarning() << QString("AMReadOnlyPVControl: Read Process Variable error %1: code %2.").arg(source->pvName()).arg(errorCode);
		}
	}
}

void AMReadOnlyPVControl::onReadPVInitialized() {
	setUnits(readPV_->units());	// copy over the new unit string
	setEnumStates(readPV_->enumStrings());	// todo: for subclasses, what to do if readPV and writePV have different number of enum states? Protect against invalid access somehow...
}


AMPVControl::AMPVControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& stopPVname, QObject* parent, double tolerance, double completionTimeoutSeconds, int stopValue, const QString &description)
	: AMReadOnlyPVControl(name, readPVname, parent, description)
{
	setTolerance(tolerance);

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
	// instead of connected(), use writeRead: connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onWritePVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeoutOccurred()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
	connect(writePV_, SIGNAL(valueChanged(double)), this, SIGNAL(setpointChanged(double)));

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
}

AMSinglePVControl::AMSinglePVControl(const QString &name, const QString &PVname, QObject *parent, double tolerance, double completionTimeoutSeconds, const QString &description)
	: AMPVControl(name, PVname, PVname, QString(), parent, tolerance, completionTimeoutSeconds, 1, description)
{
}

// Start a move to the value setpoint:
/// \todo: figure out if dave and tom want handling for already-moving... (practical example: HV supply)
void AMPVControl::move(double setpoint) {

	// new move target:
	setpoint_ = setpoint;

	// kill any old countdowns:
	completionTimer_.stop();

	if( canMove() ) {
		// Issue the move
		writePV_->setValue(setpoint_);

		// We're now moving! Let's hope this hoofedinkus makes it... (No way to actually check.)
		emit movingChanged(moveInProgress_ = true);

		// emit the signal that we started:
		emit moveStarted();

		// Special case added:
		// =======================
		// If we're already in-position (ie: moving to where we are already). In this situation, you won't see any feedback values change (onNewFeedbackValue() won't be called), but probably want to get the moveSucceeded() signal right away -- ie: instead of waiting for the move timeout.
		// This check is only possible if you've actually specified a non-default, appropriate timeout:
		if(tolerance() != AMCONTROL_TOLERANCE_DONT_CARE && inPosition()) {
			emit movingChanged(moveInProgress_ = false);
			emit moveSucceeded();
		}
		else {
			// start the countdown to see if we get there in time or stall out: (completionTimeout_ is in seconds)
			completionTimer_.start(int(completionTimeout_*1000.0));
		}
	}
	else {
		qWarning() << QString("AMPVControl: Could not move %1 to %2").arg(writePV_->pvName()).arg(setpoint_);

		// Notify the failure right away:
		emit moveFailed(AMControl::NotConnectedFailure);
		// And we're definitely not moving.
		if(moveInProgress_)
			emit movingChanged(moveInProgress_ = false);
	}

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
}

// This is used to handle errors from the write pv
void AMPVControl::onWritePVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if(errorCode == AMPROCESSVARIABLE_CANNOT_WRITE && shouldMove() ){
			emit error(AMControl::CannotWriteError);
			qWarning() << QString("AMPVControl: Write Process Variable Error %1: code %2.").arg(source->pvName()).arg(errorCode);
		}
	}
}

// This is used to handle the timeout of a move:
void AMPVControl::onCompletionTimeout() {

	// if we weren't moving, this shouldn't have happened. someone forgot to shutoff the timer?
	// todo: this is only included for state testing debugging... can remove if never happens
	if(!moveInProgress_) {
		qWarning() << "AMPVControl: timer timeout while move not in progress.  How did this happen?";
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


// This is called when a PV channel connects or disconnects
void AMPVControl::onPVConnected(bool) {

	// we'll receive this when any PV connects or disconnects.

	// We need both connected to count as connected. That's not hard to figure out.
	// But, we should do some change detection so that if we're already disconnected,
	// we don't re-emit disconnected. (Or, emit disconnected after the first PV connects and the second hasn't come up yet)

	bool nowConnected = isConnected();

	// Losing the connection:
	if(wasConnected_ && !nowConnected ) {
		emit connected(wasConnected_ = nowConnected);
	}

	// gaining the connection:
	if(!wasConnected_ && nowConnected ) {
		emit connected(wasConnected_ = nowConnected);
	}
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
	//connect(movingPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(movingPV_, SIGNAL(readReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(movingPV_, SIGNAL(error(int)), this, SLOT(onStatusPVError(int)));
	connect(movingPV_, SIGNAL(connectionTimeout()), this, SIGNAL(movingConnectionTimeoutOccurred()));
	connect(movingPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

}

// This is called when a PV channel connects or disconnects
void AMReadOnlyPVwStatusControl::onPVConnected(bool) {
	bool nowConnected = isConnected();

	// Due change detection so that we don't emit disconnected() when already disconnected()...
	// This could happen when the first PV connects but the move PV hasn't come up yet.

	// disconnected:
	if(wasConnected_ && !nowConnected)
		emit connected(wasConnected_ = nowConnected);

	// connection gained:
	if(!wasConnected_ && nowConnected)
		emit connected(wasConnected_ = nowConnected);
}

// This is used to handle errors from the status pv
void AMReadOnlyPVwStatusControl::onStatusPVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if(errorCode == AMPROCESSVARIABLE_CANNOT_READ){
			emit error(AMControl::CannotGetStatusError);
			qWarning() << QString("AMReadOnlyPVwStatusControl: Status Process Variable error %1: code %2.").arg(source->pvName()).arg(errorCode);
		}
	}
}

// This is called whenever there is an update from the move status PV
// In it we simply do change detection of the moving status, and emit signals if it has changed.
void AMReadOnlyPVwStatusControl::onMovingChanged(int movingValue) {

	bool nowMoving = (*statusChecker_)(movingValue);

	if(wasMoving_ && !nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);

	if(!wasMoving_ && nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);
}

AMPVwStatusControl::AMPVwStatusControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname, const QString& stopPVname, QObject* parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker* statusChecker, int stopValue, const QString &description)
	: AMReadOnlyPVwStatusControl(name, readPVname, movingPVname, parent, statusChecker, description) {

	// Initialize:
	moveInProgress_ = false;
	stopInProgress_ = false;
	startInProgress_ = false;
	setTolerance(tolerance);
	setpoint_ = 0;
	moveStartTimeout_ = moveStartTimeoutSeconds;

	// create new setpoint PV. Monitor it, in case someone else changes it
	writePV_ = new AMProcessVariable(writePVname, true, this);

	// connect:
	// use writeReadyChanged() instead of connected() here: connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onWritePVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeoutOccurred()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
	connect(writePV_, SIGNAL(valueChanged(double)), this, SIGNAL(setpointChanged(double)));

	// connect the timer to the timeout handler:
	connect(&moveStartTimer_, SIGNAL(timeout()), this, SLOT(onMoveStartTimeout()));

	// watch out for the changes in the device moving:
	connect(this, SIGNAL(movingChanged(bool)), this, SLOT(onIsMovingChanged(bool)));

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

}

// This is called when a PV channel connects or disconnects
void AMPVwStatusControl::onPVConnected(bool) {
	bool nowConnected = isConnected();

	// Due change detection so that we don't emit disconnected() when already disconnected()...
	// This could happen when the first PV connects but the move PV hasn't come up yet.

	// disconnected:
	if(wasConnected_ && !nowConnected)
		emit connected(wasConnected_ = nowConnected);

	// connection gained:
	if(!wasConnected_ && nowConnected)
		emit connected(wasConnected_ = nowConnected);
}

// Start a move to the value setpoint:
void AMPVwStatusControl::move(double setpoint) {

	stopInProgress_ = false;
	moveInProgress_ = false;
	// Flag that "our" move started:
	startInProgress_ = true;

	// This is our new target:
	setpoint_ = setpoint;
	// Issue the move command:
	writePV_->setValue(setpoint_);

	// Special case added:
	// =======================
	// If we're already in-position (ie: moving to where we are already). In this situation, you won't see any feedback values change (onNewFeedbackValue() won't be called), but probably want to get the moveSucceeded() signal right away -- ie: instead of waiting for the move timeout.
	// This check is only possible if you've actually specified a non-default, appropriate tolerance:
	if(tolerance() != AMCONTROL_TOLERANCE_DONT_CARE && inPosition()) {
		startInProgress_ = false;
		moveInProgress_ = true;
		emit moveStarted();
		moveInProgress_ = false;
		emit moveSucceeded();
	}
	else {
		// start the timer to check if our move failed to start:
		moveStartTimer_.start(int(moveStartTimeout_*1000.0));
	}

}

// Tell the motor to stop.  (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
bool AMPVwStatusControl::stop() {
	if(!canStop())
		return false;

	stopPV_->setValue(stopValue_);
	stopInProgress_ = true;	// flag that a stop is "in progress" -- we've issued the stop command.
	moveInProgress_ = false;	// one of "our" moves is no longer in progress.
	startInProgress_ = false;
	return true;
}

// This is used to handle errors from the status pv
void AMPVwStatusControl::onWritePVError(int errorCode) {
	// TODO: figure out how to handle this best.
	// For now, just report it.
	AMProcessVariable* source = qobject_cast<AMProcessVariable*>(sender());
	if(source){
		if(errorCode == AMPROCESSVARIABLE_CANNOT_WRITE && shouldMove()){
			emit error(AMControl::CannotWriteError);
			qWarning() << QString("AMPVwStatusControl: Write Process Variable error %1: %2.").arg(source->pvName()).arg(errorCode);
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

// This is used to add our own move tracking signals when isMoving() changes.
//This slot will only be accessed on _changes_ in isMoving()
void AMPVwStatusControl::onIsMovingChanged(bool isMoving) {

	// if we requested one of our moves, and moving just started:
	if(startInProgress_ && isMoving) {
		moveInProgress_ = true;
		startInProgress_ = false;
		// This is great... the device started moving within the timeout:

		// disable the moveStartTimer; we don't need it anymore
		moveStartTimer_.stop();

		emit moveStarted();
	}

	// If one of our moves was running, and we stopped moving:
	if(moveInProgress_ && !isMoving) {
		// That's the end of our move
		moveInProgress_ = false;

		// Check if we succeeded...
		if(inPosition()) {
			emit moveSucceeded();
		}
		else {
			emit moveFailed(AMControl::ToleranceFailure);
		}
	}

	// "sucessfully" stopped due to a stop() command.
	if(stopInProgress_ && !isMoving) {
		stopInProgress_ = false;
		// but the move itself has failed, due to a stop() intervention.
		emit moveFailed(AMControl::WasStoppedFailure);
	}

}


AMReadOnlyWaveformBinningPVControl::AMReadOnlyWaveformBinningPVControl(const QString &name, const QString &readPVname, int lowIndex, int highIndex, QObject *parent, const QString &description) :
	AMReadOnlyPVControl(name, readPVname, parent, description)
{
	disconnect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	setBinParameters(lowIndex, highIndex);
	connect(readPV_, SIGNAL(valueChanged()), this, SLOT(onReadPVValueChanged()));
}

double AMReadOnlyWaveformBinningPVControl::value() const{
	return readPV_->binIntegerValues(lowIndex_, highIndex_);
}

void AMReadOnlyWaveformBinningPVControl::setBinParameters(int lowIndex, int highIndex){
	lowIndex_ = lowIndex;
	highIndex_ = highIndex;
}

void AMReadOnlyWaveformBinningPVControl::onReadPVValueChanged(){
	emit valueChanged(value());
}







AMPVwStatusAndUnitConversionControl::AMPVwStatusAndUnitConversionControl(const QString &name, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &stopPVname, AMAbstractUnitConverter *readUnitConverter, AMAbstractUnitConverter *writeUnitConverter, QObject *parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description) :
	AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{
	readConverter_ = readUnitConverter;
	writeConverter_ = writeUnitConverter;

	disconnect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(readPV_, SIGNAL(valueChanged(double)), this, SLOT(onReadPVValueChanged(double)));

	disconnect(writePV_, SIGNAL(valueChanged(double)), this, SIGNAL(setpointChanged(double)));
	connect(writePV_, SIGNAL(valueChanged(double)), this, SLOT(onWritePVValueChanged(double)));

}

void AMPVwStatusAndUnitConversionControl::onReadPVValueChanged(double newValue)
{
	emit valueChanged(readUnitConverter()->convertFromRaw(newValue));
}

void AMPVwStatusAndUnitConversionControl::onWritePVValueChanged(double newValue)
{
	emit setpointChanged(writeUnitConverter()->convertFromRaw(newValue));
}

void AMPVwStatusAndUnitConversionControl::setUnitConverters(AMAbstractUnitConverter *readUnitConverter, AMAbstractUnitConverter* writeUnitConverter)
{
	QString oldUnits = units();
	double oldValue = value();
	double oldSetpoint = setpoint();

	delete readConverter_;
	readConverter_ = readUnitConverter;
	if(writeConverter_)
		delete writeConverter_;
	writeConverter_ = writeUnitConverter;

	double newValue = value();
	QString newUnits = units();
	double newSetpoint = setpoint();

	if(newUnits != oldUnits)
		emit unitsChanged(newUnits);
	if(newValue != oldValue)
		emit valueChanged(newValue);
	if(newSetpoint != oldSetpoint)
		emit setpointChanged(newSetpoint);
}


