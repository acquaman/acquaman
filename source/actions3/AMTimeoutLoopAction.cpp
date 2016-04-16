#include "AMTimeoutLoopAction.h"

AMTimeoutLoopAction::AMTimeoutLoopAction(AMTimeoutLoopActionInfo *info, QObject *parent)
	: AMLoopAction3(info, parent)
{
	waitingForTimeout_ = false;
	timeoutTimer_.setInterval(int(info->timeoutTime()*1000));
	connect(&timeoutTimer_, SIGNAL(timeout()), this, SLOT(internalDoNextIteration()));
	connect(this, SIGNAL(paused()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &timeoutTimer_, SLOT(start()));
	connect(this, SIGNAL(started()), &timeoutTimer_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(failed()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(succeeded()), &timeoutTimer_, SLOT(stop()));
}

AMTimeoutLoopAction::AMTimeoutLoopAction(int iterations, double timeoutTime, QObject *parent)
	: AMLoopAction3(new AMTimeoutLoopActionInfo(iterations, timeoutTime), parent)
{
	waitingForTimeout_ = false;
	timeoutTimer_.setInterval(int(timeoutTime*1000));
	connect(&timeoutTimer_, SIGNAL(timeout()), this, SLOT(internalDoNextIteration()));
	connect(this, SIGNAL(paused()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &timeoutTimer_, SLOT(start()));
	connect(this, SIGNAL(started()), &timeoutTimer_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(failed()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(succeeded()), &timeoutTimer_, SLOT(stop()));
}

AMTimeoutLoopAction::AMTimeoutLoopAction(const AMTimeoutLoopAction &original)
	: AMLoopAction3(original)
{
	waitingForTimeout_ = false;
	timeoutTimer_.setInterval(int(original.timeoutLoopInfo()->timeoutTime()*1000));
	connect(&timeoutTimer_, SIGNAL(timeout()), this, SLOT(internalDoNextIteration()));
	connect(this, SIGNAL(paused()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &timeoutTimer_, SLOT(start()));
	connect(this, SIGNAL(started()), &timeoutTimer_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(failed()), &timeoutTimer_, SLOT(stop()));
	connect(this, SIGNAL(succeeded()), &timeoutTimer_, SLOT(stop()));
}

AMTimeoutLoopAction::~AMTimeoutLoopAction()
{

}

void AMTimeoutLoopAction::internalDoNextAction()
{
	// did an action just finish completing?
	if(currentSubActionIndex_ >= 0) {

		internalDisconnectAction(currentSubAction_);
		// delete it later (since we might still be executing inside the action's functions).
		currentSubAction_->scheduleForDeletion();
	}

	// Check if we are stopping now.
	if (skipAfterCurrentAction_)
		setSkipped();

	// do we have a next action in this loop? [This will also trigger the start of the very first action]
	else if(currentSubActionIndex_ < subActionCount()-1) {
		emit currentSubActionChanged(++currentSubActionIndex_);

		currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
		internalConnectAction(currentSubAction_);

		if(state() == AMAction3::Pausing)
			setPaused();
		else if(state() == AMAction3::Running)
			currentSubAction_->start();
	}
	else {
		// done this loop.
		emit currentIterationChanged(++currentIteration_);

		// Are we stopping now that we are at the end of this iteration?
		if (skipAfterCurrentIteration_)
			setSkipped();

		// Is there a next one?
		else if(currentIteration_ < loopCount()) {

			waitingForTimeout_ = true;
		}
		// Nope, that's the end.
		else {
			setSucceeded();
		}
	}
}

void AMTimeoutLoopAction::internalDoNextIteration()
{
	if (waitingForTimeout_){

		waitingForTimeout_ = false;

		setStatusText(QString("Loop %1 of %2.").arg(currentIteration_+1).arg(loopCount()));
		emit currentSubActionChanged(currentSubActionIndex_ = 0);

		currentSubAction_ = subActions_.at(currentSubActionIndex_)->createCopy();
		internalConnectAction(currentSubAction_);

		timeoutTimer_.start(timeoutLoopInfo()->timeoutTime()*1000);

		if(state() == AMAction3::Pausing)
			setPaused();
		else if(state() == AMAction3::Running)
			currentSubAction_->start();
	}

	else
		setFailed("Timeout before loop iteration completed.");
}

void AMTimeoutLoopAction::onTimeoutTimeChanged()
{
	timeoutTimer_.setInterval(int(timeoutLoopInfo()->timeoutTime()*1000));
}
