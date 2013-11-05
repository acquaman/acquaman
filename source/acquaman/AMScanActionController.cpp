#include "AMScanActionController.h"

#include "actions3/AMActionRunner3.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/AMBeamline.h"

AMScanActionController::AMScanActionController(AMScanConfiguration *configuration, QObject *parent) :
	AMScanController(configuration, parent)
{
	scanningActions_ = 0;
	initializationActions_ = 0;
	cleanupActions_ = 0;
	scanningActionsSucceeded_ = false;

	connect(this, SIGNAL(stateChanged(int,int)), this, SLOT(onStateChanged(int,int)));
}

AMScanActionController::~AMScanActionController()
{
	if (initializationActions_)
		delete initializationActions_;

	if (cleanupActions_)
		delete cleanupActions_;
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

		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				AMSCANACTIONCONTROLLER_CANNOT_INTIALIZE,
				"Error, SGM XAS Scan Action Controller failed to start (SGM is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning."));

		return false;
	}

	AMActionRunner3::scanActionRunner()->addActionToQueue(scanningActions_);
	connect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	connect(scanningActions_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));
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

void AMScanActionController::onActionsTreeSucceeded()
{
	scanningActionsSucceeded_ = true;

	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	disconnect(scanningActions_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));

	if (cleanupActions_){

		connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
		connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

		AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
		AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	}

	else
		setFinished();
}

void AMScanActionController::onActionsTreeFailed()
{
	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onActionsTreeSucceeded()));
	disconnect(scanningActions_, SIGNAL(failed()), this, SLOT(onActionsTreeFailed()));

	if (cleanupActions_){

		connect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
		connect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

		AMActionRunner3::scanActionRunner()->addActionToQueue(cleanupActions_);
		AMActionRunner3::scanActionRunner()->setQueuePaused(false);
	}

	else
		setFailed();
}

void AMScanActionController::onCleanupActionsListSucceeded()
{
	disconnect(cleanupActions_, SIGNAL(succeeded()), this, SLOT(onCleanupActionsListSucceeded()));
	disconnect(cleanupActions_, SIGNAL(failed()), this, SLOT(onCleanupActionsListFailed()));

	if(scanningActionsSucceeded_)
		setFinished();

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
