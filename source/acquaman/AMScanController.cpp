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

#include <QDateTime>
#include "AMScanController.h"

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalConfig_ = cfg;
	scan_ = 0;

	state_ = AMScanController::Constructed;
	skipCommand_ = "";
}

AMScanController::~AMScanController()
{

}

QString AMScanController::stateString(ScanState scanControllerState) const
{
	QString controllerString = "Scan is ";

	switch(scanControllerState){

	case AMScanController::Constructed:
		controllerString.append("constructed.");
		break;

	case AMScanController::Initializing:
		controllerString.append("initializing.");
		break;

	case AMScanController::Initialized:
		controllerString.append("initialized.");
		break;

	case AMScanController::Starting:
		controllerString.append("starting.");
		break;

	case AMScanController::Running:
		controllerString.append("running.");
		break;

	case AMScanController::Pausing:
		controllerString.append("pausing.");
		break;

	case AMScanController::Paused:
		controllerString.append("paused.");
		break;

	case AMScanController::Resuming:
		controllerString.append("resuming.");
		break;

	case AMScanController::Stopping:
		controllerString.append("stopping");
		break;

	case AMScanController::Cleaning:
		controllerString.append("cleaning.");
		break;

	case AMScanController::Cancelling:
		controllerString.append("cancelling.");
		break;

	case AMScanController::Cancelled:
		controllerString.append("cancelled.");
		break;

	case AMScanController::Finished:
		controllerString.append("finished.");
		break;

	case AMScanController::Failed:
		controllerString.append("failed.");
		break;
	}

	return controllerString;
}

bool AMScanController::initialize()
{
	if(canChangeState(AMScanController::Initializing)) {
		setState(AMScanController::Initializing);
		return initializeImplementation();
	}

	return false;
}

bool AMScanController::start()
{
	if(canChangeState(AMScanController::Starting)) {
		setState(AMScanController::Starting);
		return startImplementation();
	}

	return false;
}

bool AMScanController::pause()
{
	if(canChangeState(AMScanController::Pausing)){
		setState(AMScanController::Pausing);
		pauseImplementation();
		return true;
	}

	return false;
}

bool AMScanController::resume()
{
	if(canChangeState(AMScanController::Resuming)){
		setState(AMScanController::Resuming);
		resumeImplementation();
		return true;
	}

	return false;
}

void AMScanController::cancel()
{
	if(canChangeState(AMScanController::Cancelling)) {
		setState(AMScanController::Cancelling);
		cancelImplementation();
	}
}

void AMScanController::stop(const QString &command)
{
	if(canChangeState(AMScanController::Stopping)) {
		skipCommand_ = command;

		setState(AMScanController::Stopping);
		stopImplementation(command);
	}
}

void AMScanController::scheduleForDeletion()
{
	deleteLater();
}

/// ============== protected slots/ functions =======================

bool AMScanController::setInitialized()
{
	if(canChangeState(AMScanController::Initialized)){

		if(scan_)
			scan_->setScanController(this);

		setState(AMScanController::Initialized);
		emit initialized();

		return true;
	}

	return false;
}

bool AMScanController::setStarted()
{
	if(canChangeState(AMScanController::Running)){

		if(scan_)
			scan_->setScanController(this);

		setState(AMScanController::Running);
		emit started();

		return true;
	}

	return false;
}

bool AMScanController::setPaused()
{
	if(canChangeState(AMScanController::Paused)){
		setState(AMScanController::Paused);

		emit paused();
		return true;
	}

	return false;
}

bool AMScanController::setResumed()
{
	if(canChangeState(AMScanController::Running)){
		setState(AMScanController::Running);
		emit resumed();
		return true;
	}

	return false;
}

void AMScanController::setCleaning()
{
	if(canChangeState(AMScanController::Cleaning)) {
		setState(AMScanController::Cleaning);
	}
}

void AMScanController::setCancelled()
{
	if(canChangeState(AMScanController::Cancelled)) {

		if(scan_){
			scan_->setEndDateTime(QDateTime::currentDateTime());
			scan_->setScanController(0);
		}

		setState(AMScanController::Cancelled);
		emit cancelled();
	}
}

bool AMScanController::setFinished()
{
	if(canChangeState(AMScanController::Finished)){

		if(scan_){
			scan_->setEndDateTime(QDateTime::currentDateTime());
			scan_->setScanController(0);
		}

		setState(AMScanController::Finished);
		emit finished();

		return true;
	}

	return false;
}

void AMScanController::setFailed()
{
	if(canChangeState(AMScanController::Failed)) {

		if(scan_) {
			scan_->setEndDateTime(QDateTime::currentDateTime());
			scan_->setScanController(0);
		}

		setState(AMScanController::Failed);
		emit failed();
	}
}

bool AMScanController::canChangeState(AMScanController::ScanState newState)
{
	bool canTransition = false;

	// Check the permissible transitions
	switch (newState) {

	case AMScanController::Constructed :
		break;

	case AMScanController::Initializing :
		if(state_ == AMScanController::Constructed)
			canTransition = true;
		break;

	case AMScanController::Initialized :
		if(state_ == AMScanController::Initializing)
			canTransition = true;
		break;

	case AMScanController::Starting :
		if(state_ == AMScanController::Initialized)
			canTransition = true;
		break;

	case AMScanController::Running :
		if(state_ == AMScanController::Starting || state_ == AMScanController::Resuming)
			canTransition = true;
		break;

	// Only support pausing from the running state right now
	case AMScanController::Pausing :
		if(canPause() && isRunning())
			canTransition = true;
		break;

	case AMScanController::Paused :
		if(state_ == AMScanController::Pausing)
			canTransition = true;
		break;

	case AMScanController::Resuming :
		if(isPaused())
			canTransition = true;
		break;

	case AMScanController::Stopping :
		if (isRunning())
			canTransition = true;
		break;

	case AMScanController::Cleaning :
		if (state_ == Initializing || isRunning() || isStopping() || state_ == Cancelling)
			canTransition = true;
		break;

	case AMScanController::Cancelling :
		if (state_ != Cancelling && state_ != Cancelled)
			canTransition = true;
		break;

	case AMScanController::Cancelled :
		if (state_ == Cancelling || state_ == Cleaning)
			canTransition = true;
		break;

	case AMScanController::Finished :
		if(isRunning() || isStopping() || state_ == Cleaning)
			canTransition = true;
		break;

	case AMScanController::Failed :
		canTransition = true;
		break;
	}

	return canTransition;
}

void AMScanController::setState(ScanState newState)
{
	if(canChangeState(newState)) {

		AMScanController::ScanState lastState = state_;
		state_= newState;

		emit stateChanged(lastState, newState);
	}
}
