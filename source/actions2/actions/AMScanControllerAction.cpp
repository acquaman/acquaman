#include "AMScanControllerAction.h"

#include "acquaman/AMScanConfiguration.h"
#include "acquaman/AMScanController.h"
#include "actions2/actions/AMScanControllerActionInfo.h"

AMScanControllerAction::AMScanControllerAction(AMScanControllerActionInfo* info, QObject *parent) :
	AMAction(info, parent)
{
	controller_ = 0;
	shouldCancelASAP_ = false;
}

AMScanControllerAction::AMScanControllerAction(const AMScanControllerAction& other) :
	AMAction(other)
{
	controller_ = 0;
	shouldCancelASAP_ = false;
}

AMScanControllerAction::~AMScanControllerAction() {
	if(controller_)
		delete controller_;
}

bool AMScanControllerAction::canPause() const
{
	if(controller_)
		return controller_->controllerCanPause();
	else
		return false;
}

const AMScanControllerActionInfo * AMScanControllerAction::scanControllerInfo() const
{
	return qobject_cast<const AMScanControllerActionInfo*>(info());
}

AMScanControllerActionInfo * AMScanControllerAction::scanControllerInfo()
{
	return qobject_cast<AMScanControllerActionInfo*>(info());
}

void AMScanControllerAction::startImplementation()
{
	AMScanConfiguration* config = scanControllerInfo()->scanConfig();
	if(!config) {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -13,
										 "There was no valid scan configuration provided. Please report this problem to the Acquaman developers."));
		notifyFailed();
		return;
	}

	controller_ = config->createController();
	if(!controller_) {
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -1,
										 "Could not create the scan controller. Please report this problem to the Acquaman developers."));
		notifyFailed();
		return;
	}

	connect(controller_, SIGNAL(stateChanged(int,int)), this, SLOT(onScanControllerStateChanged(int,int)));
	connect(controller_, SIGNAL(progress(double,double)), this, SLOT(setProgress(double,double)));
	connect(controller_, SIGNAL(timeRemaining(double)), this, SLOT(onScanControllerTimeRemaining(double)));

	// all future status text updates will come automatically from AMAction state changes. This is the only one that doesn't have a corresponding AMAction state change.
	setStatusText("Initializing");

	if(!controller_->initialize()){
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Alert,
										 -5,
										 "There was a problem initializing the scan controller. The preceeding error messages might give more information. Please report this problem to the beamline staff."));
		disconnect(controller_, 0, this, 0);
		notifyFailed();
		return;
	}
}

void AMScanControllerAction::pauseImplementation()
{
	// will only be called from the running state, so we can assume there is a controller_
	controller_->pause();
}

void AMScanControllerAction::resumeImplementation()
{
	controller_->resume();
}

void AMScanControllerAction::cancelImplementation()
{
	if(!controller_) {
		notifyCancelled();
		return;
	}

	// if we're in a transient startup state, we might not be able to cancel right now... Not sure about the behaviour of the different scan controllers; from observations at SGM, looks like the AMDacqScanController doesn't cancel well while initializing.
	if(controller_->state() == AMScanController::Initializing || controller_->state() == AMScanController::Starting) {
		shouldCancelASAP_ = true;
	}
	else {
		controller_->cancel();
	}
}

// #include "dataman/AMScan.h"

void AMScanControllerAction::onScanControllerStateChanged(int previousState, int state)
{
	// on initialized(): Need to start it
	if(state == AMScanController::Initialized && previousState == AMScanController::Initializing) {
		if(shouldCancelASAP_) {
			disconnect(controller_, 0, this, 0);
			notifyCancelled();
		}
		else if(!controller_->start()) {
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -2,
											 "Could not start the scan controller. Please report this bug to the Acquaman developers."));
			disconnect(controller_, 0, this, 0);
			notifyFailed();
		}
	}


	// on started(): need to save the scan
	if(state == AMScanController::Running && previousState == AMScanController::Starting) {
		if(shouldCancelASAP_) {
			controller_->cancel();
		}
		else {
			if(!controller_->scan()->storeToDb(AMDatabase::database("user")))
				AMErrorMon::report(AMErrorReport(this,
												 AMErrorReport::Alert,
												 -3,
												 "Could not save the scan to the database. Please report this bug to the Acquaman developers."));
			notifyStarted();
		}
	}


	// on cancelled():
	if(state == AMScanController::Cancelled) {
		disconnect(controller_, 0, this, 0);
		notifyCancelled();
	}


	// on failed():
	if(state == AMScanController::Failed) {
		disconnect(controller_, 0, this, 0);
		notifyFailed();
	}

	// on succeeded(): save the scan once again...
	if(state == AMScanController::Finished) {
		disconnect(controller_, 0, this, 0);
		if(!controller_->scan()->storeToDb(AMDatabase::database("user"))) {
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -4,
											 "Could not save the scan to the database. Please report this bug to the Acquaman developers."));
		}
		/// \todo ... incorporate auto-export here.
		notifySucceeded();
	}

	// on paused():
	if(state == AMScanController::Paused && previousState == AMScanController::Pausing) {
		notifyPaused();
	}
	// on resumed():
	if(state == AMScanController::Running && previousState == AMScanController::Resuming) {
		notifyResumed();
	}
}

void AMScanControllerAction::onScanControllerTimeRemaining(double seconds)
{
	setExpectedDuration(seconds + runningTime());
}
