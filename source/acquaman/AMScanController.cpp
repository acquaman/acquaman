/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

#include <QDebug>
#include <QDateTime>
#include "AMScanController.h"

AMScanControllerSupervisor* AMScanControllerSupervisor::instance_ = 0;

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalCfg_ = cfg;
	// unused: _pCfg_ = & generalCfg_;
	generalScan_ = 0;
	// unused: _pScan_ = &generalScan_;

	state_ = AMScanController::Constructed;
	/*
	running_ = false;
	paused_ = false;
	initialized_ = false;
	failed_ = false;
	finished_ = false;
	*/
}

AMScanController::ScanState AMScanController::state() const {
	return state_;
}

bool AMScanController::isInitializing() const {
	return state_ == AMScanController::Initializing;
}

bool AMScanController::isInitialized() const {
	return state_ == AMScanController::Initialized;
}

bool AMScanController::isStarting() const {
	return state_ == AMScanController::Starting;
}

bool AMScanController::isRunning() const {
	return state_ == AMScanController::Running;
}

bool AMScanController::isPausing() const {
	return state_ == AMScanController::Pausing;
}

bool AMScanController::isPaused() const {
	return state_ == AMScanController::Paused;
}

bool AMScanController::isResuming() const {
	return state_ == AMScanController::Resuming;
}

bool AMScanController::isCancelling() const {
	return state_ == AMScanController::Cancelling;
}

bool AMScanController::isCancelled() const {
	return state_ == AMScanController::Cancelled;
}

bool AMScanController::isFinished() const {
	return state_ == AMScanController::Finished;
}

bool AMScanController::isFailed() const {
	return state_ == AMScanController::Failed;
}

bool AMScanController::initialize(){
	if(changeState(AMScanController::Initializing)){
		if(initializeImplementation())
			return true;
		else
			return false;
	}
	return false;
}

bool AMScanController::start(){
	if(changeState(AMScanController::Starting)){
		if(startImplementation())
			return true;
		else
			return false;
	}
	return false;
}

bool AMScanController::pause(){
	if(changeState(AMScanController::Pausing)){
		pauseImplementation();
		return true;
	}
	return false;
}

bool AMScanController::resume(){
	if(changeState(AMScanController::Resuming)){
		resumeImplementation();
		return true;
	}
	return false;
}

void AMScanController::cancel(){
	if(changeState(AMScanController::Cancelling))
		cancelImplementation();
}

bool AMScanController::setInitialized(){
	if(changeState(AMScanController::Initialized)){
		emit initialized();
		return true;
	}
	return false;
}

bool AMScanController::setStarted(){
	if(changeState(AMScanController::Running)){
		emit started();
		return true;
	}
	return false;
}

bool AMScanController::setPaused(){
	if(changeState(AMScanController::Paused)){
		emit paused();
		return true;
	}
	return false;
}

bool AMScanController::setResumed(){
	if(changeState(AMScanController::Running)){
		emit resumed();
		return true;
	}
	return false;
}

void AMScanController::setCancelled(){
	if(changeState(AMScanController::Cancelled))
		emit cancelled();
}

bool AMScanController::setFinished(){
	if(changeState(AMScanController::Finished)){
		emit finished();
		return true;
	}
	return false;
}

void AMScanController::setFailed(){
	if(changeState(AMScanController::Failed))
		emit failed();
}

bool AMScanController::changeState(ScanState newState){
	bool canTransition = false;
	// Check the permissible transitions
	switch (newState) {
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
	case AMScanController::Cancelling :
		canTransition = true;
		break;
	case AMScanController::Cancelled :
		canTransition = true;
		break;
	case AMScanController::Finished :
		if(isRunning())
			canTransition = true;
		break;
	case AMScanController::Failed :
		canTransition = true;
		break;
	}
	if(canTransition){
		ScanState oldState = state_;
		state_= newState;
		emit stateChanged(oldState, newState);
	}
	return canTransition;
}

AMScanControllerSupervisor::AMScanControllerSupervisor(QObject *parent) :
		QObject(parent)
{
	currentScanController_ = 0;
}


AMScanControllerSupervisor::~AMScanControllerSupervisor()
{
}

AMScanControllerSupervisor* AMScanControllerSupervisor::scanControllerSupervisor(){
	if(instance_ == 0)
		instance_ = new AMScanControllerSupervisor();
	return instance_;
}

void AMScanControllerSupervisor::releaseScanControllerSupervisor(){
	if(instance_){
		delete instance_;
		instance_ = 0;
	}
}

AMScanController* AMScanControllerSupervisor::currentScanController(){
	return currentScanController_;
}

bool AMScanControllerSupervisor::setCurrentScanController(AMScanController *newScanController){
	if(currentScanController_)
		return false;
	currentScanController_ = newScanController;
	if(!currentScanController_->scan())
		return false;
	connect(currentScanController_, SIGNAL(finished()), this, SLOT(onCurrentScanControllerFinished()));
	connect(currentScanController_, SIGNAL(started()), this, SIGNAL(currentScanControllerStarted()));
	connect(currentScanController_, SIGNAL(cancelled()), this, SLOT(onCurrentScanControllerFinished()));
	emit currentScanControllerCreated();
	return true;
}

bool AMScanControllerSupervisor::deleteCurrentScanController(){
	if(!currentScanController_)
		return false;
	onCurrentScanControllerFinished();
	return true;
}

void AMScanControllerSupervisor::onCurrentScanControllerFinished(){
	emit currentScanControllerDestroyed();
	disconnect(currentScanController_, SIGNAL(finished()), this, SLOT(onCurrentScanControllerFinished()));
	disconnect(currentScanController_, SIGNAL(started()), this, SIGNAL(currentScanControllerStarted()));
	disconnect(currentScanController_, SIGNAL(cancelled()), this, SLOT(onCurrentScanControllerFinished()));
	currentScanController_->deleteLater();
	currentScanController_ = 0;
}
