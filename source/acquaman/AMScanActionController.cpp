#include "AMScanActionController.h"

#include "actions3/AMActionRunner3.h"
#include "acquaman/AMAgnosticDataAPI.h"

AMScanActionController::AMScanActionController(AMScanConfiguration *configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
	connect(this, SIGNAL(stateChanged(int,int)), this, SLOT(onStateChanged(int,int)));
}

void AMScanActionController::skip(const QString &command)
{
	AMAction3 *currentAction = AMActionRunner3::scanActionRunner()->currentAction();

	if(currentAction)
		currentAction->skip(command);
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

		qDebug() << "Need to test the scanActions db to see if there's an open transaction when going from " << oldState << " to " << newState;

		if(AMDatabase::database("scanActions")->transactionInProgress()){

			qDebug() << "Detected a commitTransaction() required in AMScanActionScanController";
			AMDatabase::database("scanActions")->commitTransaction();
		}
	}
}

bool AMScanActionController::startImplementation()
{
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
	AMAction3 *currentAction = AMActionRunner3::scanActionRunner()->currentAction();

	// That's bad
	if(!currentAction)
		return;

	if(currentAction->state() == AMAction3::Running && currentAction->pause())
		setPaused();

	else{
		// Also bad
	}
}

void AMScanActionController::resumeImplementation()
{
	AMAction3 *currentAction = AMActionRunner3::scanActionRunner()->currentAction();

	// That's bad
	if(!currentAction)
		return;

	if(currentAction->state() == AMAction3::Paused && currentAction->resume()) {
		setResumed();
	}

	else{
		// Also bad
	}
}

void AMScanActionController::cancelImplementation()
{
	AMAction3 *currentAction = AMActionRunner3::scanActionRunner()->currentAction();

	if(currentAction){

		connect(currentAction, SIGNAL(cancelled()), this, SLOT(setCancelled()));
		currentAction->cancel();
	}
}
