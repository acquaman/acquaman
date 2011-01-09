#include "AMAction.h"

// Constructor: create an action to run the specified AMActionInfo.
AMAction::AMAction(AMActionInfo* info, QObject *parent)
	: QObject(parent)
{
	info_ = info;
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = previousState_ = Constructed;
	statusText_ = "Not yet started";

	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));

	prereqBehaviour_ = WaitPrereqBehaviour;
	failureResponseInActionRunner_ = MoveOnResponse;
	failureResponseAsSubAction_ = MoveOnResponse;
}

// Copy constructor. Takes care of making copies of the info and prerequisites.
AMAction::AMAction(const AMAction& other)
	: QObject()
{
	progress_ = QPair<double,double>(-1.0, -1.0);
	state_ = previousState_ = Constructed;
	statusText_ = "Not yet started";


	prereqBehaviour_ = other.prereqBehaviour_;
	failureResponseInActionRunner_ = other.failureResponseInActionRunner_;
	failureResponseAsSubAction_ = other.failureResponseAsSubAction_;

	info_ = other.info()->createCopy();
	connect(info_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(expectedDurationChanged(double)));
	foreach(AMActionPrereq* prereq, other.prereqs_)
		prereqs_ << prereq->createCopy();
}


// Destructor: deletes the info and prerequisites
AMAction::~AMAction() {
	delete info_;
	while(!prereqs_.isEmpty())
		delete prereqs_.takeLast();
}



bool AMAction::start()
{
	if(state() != Constructed)
		return false;

	startDateTime_ = QDateTime::currentDateTime();

	// Do we have prerequisites that aren't satisfied?
	if(prereqs_.count() && !prereqsSatisfied()) {

		PrereqBehaviour p = prereqBehaviour();
		if(p == PromptUserPrereqBehaviour)
			p = promptUserForPrereqBehaviour();

		switch(p) {
		case WaitPrereqBehaviour:
			setState(WaitingForPrereqs);
			setProgress(0,0);
			// connect prereq changed signals to our slot, so we can detect when they are satisfied
			foreach(AMActionPrereq* prereq, prereqs_)
				connect(prereq, SIGNAL(satisfiedChanged(bool)), this, SLOT(internalOnPrereqChanged()));
			// the prereqs might have been satisfied in the time we were waiting for the user response. Check again...
			internalOnPrereqChanged();
			break;
		case CancelActionPrereqBehaviour:
			endDateTime_ = QDateTime::currentDateTime();
			setState(Cancelled);	// we don't need to go through Cancelling, because we haven't started yet.
			break;
		case FailActionPrereqBehaviour:
		default:
			setState(Failed);
			break;
		}
	}

	// No prereqs... start the action directly.
	else {
		setState(Starting);
		startImplementation();
	}
	return true;
}

bool AMAction::cancel()
{
	switch(state()) {

	case Cancelling:
	case Cancelled:
	case Succeeded:
	case Failed:
		return false;	// can't cancel from these states. Already cancelling, or in a final state.

	case Constructed:
		endDateTime_ = QDateTime::currentDateTime();
		setState(Cancelled);	// can cancel directly; haven't started yet
		return true;

	case WaitingForPrereqs:
		foreach(AMActionPrereq* prereq, prereqs_)
			disconnect(prereq, SIGNAL(satisfiedChanged(bool)), this, SLOT(internalOnPrereqChanged()));
		endDateTime_ = QDateTime::currentDateTime();
		setState(Cancelled);
		return true;

	case Starting:
	case Running:
	case Pausing:
	case Paused:
	case Resuming:
		setState(Cancelling);
		cancelImplementation();
		return true;
	default:
		return false;
	}
}

bool AMAction::pause()
{
	if(!canPause())
		return false;

	if(state() == WaitingForPrereqs) {
		setProgress(0,0	);
		setState(Paused);	// that's all we need... As long as the state is set to Paused, we won't advance if the prereqs become satisfied until we've been resumed.
		return true;
	}
	if(state() == Running) {
		setProgress(0,0);
		setState(Pausing);
		pauseImplementation();
		return true;
	}

	return false;	// any other state: cannot pause from there.
}

bool AMAction::resume()
{
	if(!canPause())
		return false;

	if(state() == Paused) {
		if(previousState() == WaitingForPrereqs) {
			setState(WaitingForPrereqs);
			internalOnPrereqChanged();	// review all prereqs to see if we should carry on.  They might have changed while paused.
			return true;
		}
		else {
			setState(Resuming);
			resumeImplementation();
			return true;
		}
	}

	return false;	// cannot resume from any other states.
}

bool AMAction::addPrereq(AMActionPrereq *newPrereq)
{
	if(state() != Constructed)
		return false;

	prereqs_ << newPrereq;
	return true;
}

bool AMAction::setPrereqBehaviour(AMAction::PrereqBehaviour prereqBehaviour)
{
	if(state() != Constructed)
		return false;

	prereqBehaviour_ = prereqBehaviour;
	return true;
}

void AMAction::notifyStarted()
{
	if(state() != Starting)
		return;

	setState(Running);
}

void AMAction::notifySucceeded()
{
	switch(state()) {

	case Starting:
	case Running:
	case Pausing:
	case Paused:
	case Resuming:
	case Cancelling:
		endDateTime_ = QDateTime::currentDateTime();
		setState(Succeeded);
		break;

	default:
		;
		// do nothing: can't succeed if we haven't started yet, or are already in a final state.
	}
}

void AMAction::notifyFailed()
{
	switch(state()) {

	case Starting:
	case Running:
	case Pausing:
	case Paused:
	case Resuming:
	case Cancelling:
		endDateTime_ = QDateTime::currentDateTime();
		setState(Failed);
		break;

	default:
		;// do nothing: can't fail if we haven't started yet, or are already in a final state. Why is the implementation telling us this?
	}
}

void AMAction::notifyPaused()
{
	if(state() != Pausing)
		return;
	setState(Paused);
}

void AMAction::notifyResumed()
{
	if(state() != Resuming)
		return;
	setState(Running);
}

void AMAction::notifyCancelled()
{
	// only called by implementation when finished cancelling.
	if(state() != Cancelling)
		return;
	endDateTime_ = QDateTime::currentDateTime();
	setState(Cancelled);
}

void AMAction::internalOnPrereqChanged()
{
	if(state() != WaitingForPrereqs)
		return;	// only check this and possibly move on if we're in the correct state. If we're paused while the prereqs are connected, we won't enter this function.

	if(prereqsSatisfied()) {
		// we're done here; disconnect prereqs and move on to the Starting state
		foreach(AMActionPrereq* prereq, prereqs_) {
			disconnect(prereq, SIGNAL(satisfiedChanged(bool)), this, SLOT(internalOnPrereqChanged()));
		}
		setState(Starting);	// move on to the next state
		startImplementation();
	}
}

bool AMAction::prereqsSatisfied() const
{
	bool allSatisfied = true;
	foreach(AMActionPrereq* prereq, prereqs_)
		allSatisfied &= prereq->satisfied();
	return allSatisfied;
}

#include <QMessageBox>
#include <QStringBuilder>
#include <QPushButton>

AMAction::PrereqBehaviour AMAction::promptUserForPrereqBehaviour()
{
	QMessageBox box;
	box.setWindowTitle("Prerequisites not ready");
	box.setText("There are some prerequisites for running this action that aren't ready yet. Would you like to wait for them to be ready, or cancel the action?");

	QStringList explanations;
	foreach(AMActionPrereq* prereq, prereqs_)
		if(!prereq->satisfied())
			explanations << prereq->explanation();

	box.setInformativeText("Action: " % info()->shortDescription() % "\n\n" % explanations.join("\n"));
	QPushButton* waitButton = new QPushButton("Wait for Prerequsites");
	box.addButton(waitButton, QMessageBox::AcceptRole);
	QPushButton* cancelButton = new QPushButton("Cancel Action");
	box.addButton(cancelButton, QMessageBox::RejectRole);

	box.exec();
	if(box.clickedButton() == waitButton)
		return WaitPrereqBehaviour;
	else
		return CancelActionPrereqBehaviour;
}
