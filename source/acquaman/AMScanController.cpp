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
	lastState_ = AMScanController::Constructed;
}

AMScanController::~AMScanController()
{

}

AMScanController::ScanState AMScanController::state() const
{
	return state_;
}

AMScanController::ScanState AMScanController::lastState() const
{
	return lastState_;
}

bool AMScanController::isInitializing() const
{
	return state_ == AMScanController::Initializing;
}

bool AMScanController::isInitialized() const
{
	return state_ == AMScanController::Initialized;
}

bool AMScanController::isStarting() const
{
	return state_ == AMScanController::Starting;
}

bool AMScanController::isRunning() const
{
	return state_ == AMScanController::Running;
}

bool AMScanController::isPausing() const
{
	return state_ == AMScanController::Pausing;
}

bool AMScanController::isPaused() const
{
	return state_ == AMScanController::Paused;
}

bool AMScanController::isResuming() const
{
	return state_ == AMScanController::Resuming;
}

bool AMScanController::isStopping() const
{
	return state_ == AMScanController::Stopping;
}

bool AMScanController::isCancelling() const
{
	return state_ == AMScanController::Cancelling;
}

bool AMScanController::isCancelled() const
{
	return state_ == AMScanController::Cancelled;
}

bool AMScanController::isFinished() const
{
	return state_ == AMScanController::Finished;
}

bool AMScanController::isFailed() const
{
	return state_ == AMScanController::Failed;
}

//bool AMScanController::isReadyForDeletion() const
//{
//	return true;
//}

bool AMScanController::initialize()
{
	if(changeState(AMScanController::Initializing))
		return initializeImplementation();

	return false;
}

bool AMScanController::start()
{
	if(changeState(AMScanController::Starting))
		return startImplementation();

	return false;
}

bool AMScanController::pause()
{
	if(changeState(AMScanController::Pausing)){

		pauseImplementation();
		return true;
	}

	return false;
}

bool AMScanController::resume()
{
	if(changeState(AMScanController::Resuming)){

		resumeImplementation();
		return true;
	}

	return false;
}

void AMScanController::cancel()
{
	if(changeState(AMScanController::Cancelling))
		cancelImplementation();
}

void AMScanController::stop(const QString &command)
{
	if (changeState(AMScanController::Stopping))
		stopImplementation(command);
}

void AMScanController::scheduleForDeletion()
{
	deleteLater();
}

bool AMScanController::setInitialized()
{
	if(canChangeStateTo(AMScanController::Initialized)){

		if(scan_)
			scan_->setScanController(this);

		changeState(AMScanController::Initialized);
		emit initialized();

		return true;
	}

	return false;
}

bool AMScanController::setStarted()
{
	if(canChangeStateTo(AMScanController::Running)){

		if(scan_)
			scan_->setScanController(this);

		changeState(AMScanController::Running);
		emit started();

		return true;
	}

	return false;
}

bool AMScanController::setPausing()
{
	if(changeState(AMScanController::Pausing)){

		return true;
	}

	return false;
}

bool AMScanController::setPaused()
{
	if(changeState(AMScanController::Paused)){

		emit paused();
		return true;
	}

	return false;
}

bool AMScanController::setResuming()
{
	if(changeState(AMScanController::Resuming)){

		return true;
	}

	return false;
}

bool AMScanController::setResumed()
{
	if(changeState(AMScanController::Running)){

		emit resumed();
		return true;
	}

	return false;
}

void AMScanController::setCancelled()
{
	if(canChangeStateTo(AMScanController::Cancelled)) {

		if(scan_){
			scan_->setEndDateTime(QDateTime::currentDateTime());
			scan_->setScanController(0);
		}

		changeState(AMScanController::Cancelled);
		emit cancelled();
	}
}

bool AMScanController::setFinished()
{
	if(canChangeStateTo(AMScanController::Finished)){

		if(scan_){
			scan_->setEndDateTime(QDateTime::currentDateTime());
			scan_->setScanController(0);
		}

		changeState(AMScanController::Finished);
		emit finished();

		return true;
	}

	return false;
}

void AMScanController::setFailed()
{
	if(canChangeStateTo(AMScanController::Failed)) {

		if(scan_) {
			scan_->setEndDateTime(QDateTime::currentDateTime());
			scan_->setScanController(0);
		}

		changeState(AMScanController::Failed);
		emit failed();
	}
}

bool AMScanController::canChangeStateTo(AMScanController::ScanState newState)
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
		if(isInitializing())
			canTransition = true;
		break;

	case AMScanController::Starting :
		if(isInitialized())
			canTransition = true;
		break;

	case AMScanController::Running :
		if(isStarting() || isResuming())
			canTransition = true;
		break;

	// Only support pausing from the running state right now
	case AMScanController::Pausing :
		if(canPause() && isRunning())
			canTransition = true;
		break;

	case AMScanController::Paused :
		if(isPausing())
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

	case AMScanController::Cancelling :
		canTransition = true;
		break;

	case AMScanController::Cancelled :
		canTransition = true;
		break;

	case AMScanController::Finished :
		if(isRunning() || isStopping())
			canTransition = true;
		break;

	case AMScanController::Failed :
		canTransition = true;
		break;
	}

	return canTransition;
}

#include <QDebug>
bool AMScanController::changeState(ScanState newState)
{
	if(canChangeStateTo(newState)) {
		qDebug() << " ==== AMScanController::changeState() " << newState;

		lastState_ = state_;
		state_= newState;
		emit stateChanged(lastState_, newState);

		return true;
	}
	qDebug() << " ==== AMScanController::changeState() failed to change to " << newState;

	return false;
}
