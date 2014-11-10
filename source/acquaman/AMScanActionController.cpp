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


#include "AMScanActionController.h"

#include <QThread>

#include "actions3/AMActionRunner3.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/AMBeamline.h"

AMScanActionController::~AMScanActionController(){}

AMScanActionController::AMScanActionController(AMScanConfiguration *configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
	scanningActions_ = 0;
	initializationActions_ = 0;
	cleanupActions_ = 0;
	scanningActionsSucceeded_ = false;
	scanControllerStateMachineFinished_ = false;

	fileWriterThread_ = 0;
	fileWriterIsBusy_ = false;
	deleteFileWriterImmediately_ = false;

	connect(this, SIGNAL(stateChanged(int,int)), this, SLOT(onStateChanged(int,int)));
}

void AMScanActionController::scheduleForDeletion()
{
	if(fileWriterThread_){
		connect(fileWriterThread_, SIGNAL(destroyed()), this, SLOT(deleteLater()));

		if(readyForFinished())
			fileWriterThread_->deleteLater();
		else{
			deleteFileWriterImmediately_ = true;
			if(fileWriterIsBusy_)
				emit finishWritingToFile();
		}
	}
}

void AMScanActionController::onStateChanged(int oldState, int newState)
{
	Q_UNUSED(oldState)

	AMScanController::ScanState castNewState = (AMScanController::ScanState)newState;

	if( (castNewState == AMScanController::Finished) || (castNewState == AMScanController::Failed) || (castNewState == AMScanController::Cancelled) ){

		AMAgnosticDataMessageHandler *dataMessager = AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions");
		AMAgnosticDataMessageQEventHandler *scanActionMessager = qobject_cast<AMAgnosticDataMessageQEventHandler*>(dataMessager);

		if(scanActionMessager)
			scanActionMessager->removeReceiver(this);

		if(AMDatabase::database("scanActions")->transactionInProgress())
			AMDatabase::database("scanActions")->commitTransaction();
	}
}

bool AMScanActionController::initializeImplementation()
{
	initializationActions_ = createInitializationActions();
	cleanupActions_ = createCleanupActions();

	if (initializationActions_){

		connect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
		connect(initializationActions_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

		AMActionRunner3::scanActionRunner()->addActionToQueue(initializationActions_);
		AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	}

	else
		setInitialized();

	return true;
}

bool AMScanActionController::startImplementation()
{
	if(AMBeamline::bl()->isBeamlineScanning()){

		AMErrorMon::alert(this,
				AMSCANACTIONCONTROLLER_CANNOT_INTIALIZE,
				"Error, the Scan Action Controller failed to start (the beamline is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning.");

		return false;
	}

	AMActionRunner3::scanActionRunner()->addActionToQueue(scanningActions_);
	connect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onScanningActionsSucceeded()));
	connect(scanningActions_, SIGNAL(failed()), this, SLOT(onScanningActionsFailed()));
	AMAgnosticDataMessageHandler *dataMessager = AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions");
	AMAgnosticDataMessageQEventHandler *scanActionMessager = qobject_cast<AMAgnosticDataMessageQEventHandler*>(dataMessager);

	if(scanActionMessager)
		scanActionMessager->addReceiver(this);

	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	setStarted();

	return true;
}

bool AMScanActionController::canPause() const
{
	AMAction3 *currentAction = AMActionRunner3::scanActionRunner()->currentAction();

	if(currentAction)
		return currentAction->canPause();

	return false;
}

void AMScanActionController::pauseImplementation()
{
	if (AMActionRunner3::scanActionRunner()->pauseCurrentAction())
		setPaused();

	else
		AMErrorMon::alert(this, AMSCANACTIONCONTROLLER_CANNOT_PAUSE, "Was unable to pause the current action.");
}

void AMScanActionController::resumeImplementation()
{
	if (AMActionRunner3::scanActionRunner()->resumeCurrentAction())
		setResumed();

	else
		AMErrorMon::alert(this, AMSCANACTIONCONTROLLER_CANNOT_RESUME, "Was unable to resume the current action.");
}

void AMScanActionController::cancelImplementation()
{
	if (AMActionRunner3::scanActionRunner()->cancelCurrentAction())
		setCancelled();

	else
		AMErrorMon::alert(this, AMSCANACTIONCONTROLLER_CANNOT_CANCEL, "Was unable to cancel the current action.");
}

void AMScanActionController::stopImplementation(const QString &command)
{
	AMAction3 *currentAction = AMActionRunner3::scanActionRunner()->currentAction();

	if(currentAction){

		connect(currentAction, SIGNAL(succeeded()), this, SLOT(onSkipCurrentActionSucceeded()));
		currentAction->skip(command);
	}
}

bool AMScanActionController::readyForFinished() const
{
	if(fileWriterThread_)
		return (!fileWriterIsBusy_ && fileWriterThread_->isFinished() && scanControllerStateMachineFinished_);
	else
		return (!fileWriterIsBusy_ && scanControllerStateMachineFinished_);
}

AMAction3* AMScanActionController::scanningActions()
{
	return scanningActions_;
}

void AMScanActionController::onInitializationActionsListSucceeded()
{
	disconnect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(initializationActions_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	setInitialized();
}

void AMScanActionController::onInitializationActionsListFailed()
{
	disconnect(initializationActions_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsListSucceeded()));
	disconnect(initializationActions_, SIGNAL(failed()), this, SLOT(onInitializationActionsListFailed()));

	connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

	AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
	AMActionRunner3::scanActionRunner()->setQueuePaused(false);
}

void AMScanActionController::onScanningActionsSucceeded()
{
	scanningActionsSucceeded_ = true;

	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onScanningActionsSucceeded()));
	disconnect(scanningActions_, SIGNAL(failed()), this, SLOT(onScanningActionsFailed()));

	if (cleanupActions_){

		connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
		connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

		emit cleaningActionsStarted();

		AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
		AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	}

	else{
		scanControllerStateMachineFinished_ = true;
		// Don't do anything else, if we're not ready for finished but we're succeeding then the file writing should finish soon and will check again
		if(readyForFinished())
			setFinished();
	}
}

void AMScanActionController::onScanningActionsFailed()
{
	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onScanningActionsSucceeded()));
	disconnect(scanningActions_, SIGNAL(failed()), this, SLOT(onScanningActionsFailed()));

	if (cleanupActions_){

		connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
		connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

		emit cleaningActionsStarted();

		AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
		AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	}

	else
		setFailed();
}

void AMScanActionController::onFileWriterIsBusy(bool isBusy)
{
	fileWriterIsBusy_ = isBusy;
	if(!fileWriterIsBusy_ && fileWriterThread_){
		connect(fileWriterThread_, SIGNAL(finished()), this, SLOT(onFileWriterThreadFinished()));
		fileWriterThread_->quit();
	}
}

void AMScanActionController::onFileWriterThreadFinished(){
	if(deleteFileWriterImmediately_)
		fileWriterThread_->deleteLater();

	if(readyForFinished())
		setFinished();
}

void AMScanActionController::onSkipCurrentActionSucceeded(){
	scanControllerStateMachineFinished_ = true;
	// This shouldn't be the case, because we're skipping so the file writer is probably waiting to be told to finish
	if(readyForFinished())
		setFinished();
	else if(fileWriterIsBusy_)
		emit finishWritingToFile();
}

void AMScanActionController::onCleanupActionsListSucceeded()
{
	disconnect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

	if(scanningActionsSucceeded_){
		scanControllerStateMachineFinished_ = true;
		// Don't do anything else, if we're not ready for finished but we're succeeding then the file writing should finish soon and will check again
		if(readyForFinished())
			setFinished();
	}
	else
		setFailed();
}

void AMScanActionController::onCleanupActionsListFailed()
{
	disconnect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

	setFailed();
}

void AMScanActionController::onScanningActionsGenerated(AMAction3 *actionTree)
{
	scanningActions_ = actionTree;

	AMAppControllerSupport::optimize(AMAppControllerSupport::principleOptimizersCopy(), scanningActions_);

	if(!AMAppControllerSupport::validate(AMAppControllerSupport::principleValidatorsCopy(), scanningActions_))
		return;
}
