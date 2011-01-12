#ifndef AMACTION_H
#define AMACTION_H

#include <QObject>
#include "actions2/AMActionInfo.h"

/// You can subclass this to define a prerequisite for AMActions: a condition that must be satisfied before the action can start. It should return satisfied() and have a signal satisfiedChanged() to notify whenever that changes.
class AMActionPrereq : public QObject {
	Q_OBJECT
public:
	/// Returns whether this prereq is satisfied
	virtual bool satisfied() const = 0;
	/// Returns an explanation that will make sense to users when we tell them why we can't proceed with their action
	virtual QString explanation() const = 0;
	/// This function is used as a virtual copy constructor. It should return an independent copy of the prereq instance.
	virtual AMActionPrereq* createCopy() const = 0;

signals:
	void satisfiedChanged(bool isSatisfied);
};


class AMAction : public QObject
{
	Q_OBJECT
public:
	// enum types defined
	/////////////////////////

	/// This enum describes the states that an action can be in.
	enum State { Constructed = 0,
				 WaitingForPrereqs,
				 Starting,
				 Running,
				 Pausing,
				 Paused,
				 Resuming,
				 Cancelling,
				 Cancelled,
				 Succeeded,
				 Failed
			   };
	/// When running inside AMActionRunner, or as a sub-action in AMListAction, this enum specifies what to do if the action fails
	enum FailureResponse { MoveOnResponse = 0,
						   AttemptAnotherCopyResponse,
						   PromptUserResponse
						 };
	/// When there are pre-requisites for the action, this specifies what to do if any pre-reqs aren't satisfied
	enum PrereqBehaviour { WaitPrereqBehaviour = 0,
						   CancelActionPrereqBehaviour,
						   FailActionPrereqBehaviour,
						   PromptUserPrereqBehaviour
						 };



	// Constructor and life-cycle management
	/////////////////

	/// Constructor: create an action to run the specified AMActionInfo.
	AMAction(AMActionInfo* info, QObject *parent = 0);

	/// Copy constructor. Takes care of making copies of the info and prerequisites. NOTE that the state() is reset to Constructed: the copy should look like a new action that hasn't been run yet.
	AMAction(const AMAction& other);

	/// This virtual function takes the role of a virtual copy constructor. All actions MUST RE-IMPLEMENT to be able to create and return a an independent copy of themselves. (This allows us to get a detailed subclass copy without knowing the type of the action.) NOTE that the returned instance should be a perfect copy except for the state() -- which will be reset to Constructed -- and any other subclass-specific state information: the copy should look like a new action that hasn't been run yet.
	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly.*/
	virtual AMAction* createCopy() const { return new AMAction(*this); }

	/// Destructor: deletes the info and prerequisites
	virtual ~AMAction();


	// Info API
	//////////////////////

	/// Returns the AMActionInfo subclass describing this action. The Info subclasses are a way to separate the description/content of actions from their implementation. This allows storing and viewing future/past actions inside programs which shouldn't have the capability to run them
	/*! (For example: viewing a history of a user's SGM beamline actions inside their take-home Dataman program, or when on another beamline. Theoretically, this approach could even let someone plan and queue up beamline actions at home to run once they get to the beamline.)

You can use a generic AMActionInfo in an AMAction-subclass constructor, but if you want to be able to re-create live actions from historical ones, you should provide a unique info subclass with sufficient information to completely specify/re-create the action, and register your Info-Action pair with AMActionRegistry::registerInfoAndAction().*/
	const AMActionInfo* info() const { return info_; }
	// needed?
	// AMActionInfo* info() { return info_; }

	// Action Timing: start time, end time, cumulative elapsed time, etc.  All of this is managed by the base class for you.
	////////////////////////
	/// Returns the date and time when this action was started, or an invalid QDateTime if it hasn't started yet
	QDateTime startDateTime() const { return startDateTime_; }
	/// Returns the date and time when this action finished, or an invalid QDateTime if it hasn't finished yet.
	QDateTime endDateTime() const { return endDateTime_; }
	/// Returns the total number of seconds since this action was start()ed, or -1 if it hasn't started running yet. As long as the action has started, this will be equivalent to QDateTime::currentDateTime() - startDateTime().
	/*! \note This will include all the time spent in the Paused state as well as WaitingForPrereqs. To get just the actual running time, \see runningTime(). */
	double elapsedTime() const { if(!startDateTime_.isValid()) return -1.0; return double(startDateTime_.msecsTo(QDateTime::currentDateTime()))/1000.0; }
	/// Returns the number of seconds that this action has been in the Paused and Pausing states for.
	double pausedTime() const;
	/// Returns the number of seconds that this action has been in the WaitingForPrereqs state for
	double waitingForPrereqsTime() const;
	/// Returns the number of seconds that this action has actually been running for, <i>excluding</i> time spent in the Paused (and Pausing) and WaitingForPrereqs states.
	double runningTime() const { if(!startDateTime_.isValid()) return -1.0; return elapsedTime() - pausedTime() - waitingForPrereqsTime(); }

	/// Returns a description of the action's status, for example, "Waiting for the beamline energy to reach the setpoint".  Implementations should update this while the action is running using setStatusText().
	QString statusText() const { return statusText_; }


	// States
	///////////////////

	/// Returns the current state of the action
	State state() const { return state_; }
	/// Returns a string describing the given \c state
	QString stateDescription(State state);
	/// Returns whether the action is in a final state (Succeeded, Failed, or Cancelled). All cleanup should be done before entering these states, so it should be OK to delete an action once it is in a final state.
	bool inFinalState() const { return state_ == Succeeded || state_ == Failed || state_ == Cancelled; }
	/// Returns the state the action was in before the current state
	State previousState() const { return previousState_; }

	/// This virtual function can be re-implemented to specify whether the action has the capability to pause. By default, it returns false (ie: cannot pause).
	virtual bool canPause() const { return false; }

	// External requests to change the state: start(), cancel(), pause(), and resume().
	//////////////////
	// All of these requests to change the state return false if not allowed from the current state.

	/// Start running the action. Allowed from Constructed. State will change to WaitingForPrereqs or Starting.
	bool start();

	/// Explicitly cancel the action. Allowed from anything except Cancelling, Succeeded, or Failed. The state will change Cancelling. The action could take a while to finish cancelling itself; the state will change to Cancelled when that finally happens.
	bool cancel();

	/// For actions that support pausing, request to pause the action. Allowed from WaitingForPrereqs or Running, if canPause() is true. The state will change to Paused or Pausing (respectively).
	bool pause();

	/// For actions that support pausing, request to pause the action. Allowed from Paused only. The state will change to Resuming.
	bool resume();

	// Progress API
	////////////////////////

	/// Returns the numerator and denominator of the progress. (ie: completed steps out of total steps). If the progress is unknown, this will return (0,0). If the action hasn't started yet, this will return (-1, -1).
	QPair<double, double> progress() const { return progress_; }
	/// Returns the total expected duration in seconds. If the time to completion is unknown, this will return -1.
	double expectedDuration() const { return info_->expectedDuration(); }


	// Prerequisite API
	/////////////////////////

	/// Returns the current list of prerequisites
	QList<AMActionPrereq*> prereqs() const { return prereqs_; }
	/// Returns the prereq behaviour: what to do when a prereq is not satisfied. The options are to wait for the prereq to be satisfied, cancel the action, fail the action, or prompt the user for what to do.
	PrereqBehaviour prereqBehaviour() const { return prereqBehaviour_; }

	/// Add a new prerequisite for this action. Prereqs (defined by a subclass of AMActionPrereq) specify whether the action can run right now or not. The action takes ownership of the prereq and will delete it when deleted. Returns false if the action is already running and it would be meaningless to add a prereq.
	bool addPrereq(AMActionPrereq* newPrereq);
	/// Specify what should be done in the event that a prereq is not satisfied. The options are to wait for the prereq to be satisfied, cancel the action, fail the action, or prompt the user for what to do. In the future, we might add an attempt to fix the situation causing the prereq.  The default is to wait for the prereq to be satistifed.
	/*! This function returns false if the action is already running and it would be meaningless to set the prereq behaviour.

   \note The difference between cancelling and failing the action has to do with the failure response... When cancelled, the failure response isn't invoked; when the action fails, it is.
*/
	bool setPrereqBehaviour(PrereqBehaviour prereqBehaviour);

	/// Returns true if all the prerequisites are satisfied; false if any aren't.
	bool prereqsSatisfied() const;


	// Recommended Response to failure:
	//////////////////////////

	/// When this action is run in a workflow using AMActionRunner, this specifies the recommended response if the action fails (ie: state() changes to Failed, due to some internal reason. Note that this is distinct from intentionally cancelled.) The possibilities are to give up and move on, or attempt to run the action again by generating a copy of it and running the copy -- before going on to any further actions.  As a third option, the user can be prompted to choose one of these options.  The default is to give up and move on.
	FailureResponse failureResponseInActionRunner() const { return failureResponseInActionRunner_; }
	/// When this action is run as a sub-action of another, this specifies the recommended response if the action fails (ie: state() changes to Failed, due to some internal reason. Note that this is distinct from intentionally cancelled.) The possibilities are to give up and move on, or to attempt to run the action again by generating a copy of it and running the copy -- before going on to any further actions.  As a third option, the user can be prompted to choose one of these options.  The default is to give up and move on.
	FailureResponse failureResponseAsSubAction() const { return failureResponseAsSubAction_; }

	/// Set the failure response that we would recommend when this action is run in a workflow using AMActionRunner.
	void setFailureResponseInActionRunner(FailureResponse fr) { failureResponseInActionRunner_ = fr; }
	/// Set the failure response that we would recommend when this action is run as a sub-action of another action.
	void setFailureResponseAsSubAction(FailureResponse fr) { failureResponseAsSubAction_ = fr; }

signals:

	/// Emitted whenever the state() of the action changes
	void stateChanged(int newActionState, int previousActionState);

	/// Emitted when the progress changes. (\c numerator gives the amount done, relative to the total expected amount \c denominator. For example, \c numerator could be a percentage value, and \c denominator could be 100.)
	void progressChanged(double numerator, double denominator);
	/// If the action knows how long it will take, this signal is emitted with the total expected run time. If it doesn't know how long things will take, it could be emitted with (-1).
	void expectedDurationChanged(double expectedTotalDurationInSeconds);

	/// Emitted when the statusText() changes.
	void statusTextChanged(const QString& statusText);


public slots:


protected slots:
	/// Implementations should call this to notify of their progress.   \c numerator gives the amount done, relative to the total expected amount \c denominator. For example, \c numerator could be a percentage value, and \c denominator could be 100.  If you don't know the level of progress, call this with (0,0).
	void setProgress(double numerator, double denominator) { progress_ = QPair<double,double>(numerator,denominator); emit progressChanged(numerator, denominator); }
	/// Implementations should call this to notify of the expected total duration (in seconds) of the action. If you don't know how long the action will take, call this with -1.
	void setExpectedDuration(double expectedTotalTimeInSeconds) { info_->setExpectedDuration(expectedTotalTimeInSeconds); }
	/// Implementations should call this to describe the action's status while running; for example, "Moving motor X to 30.4mm"
	void setStatusText(const QString& statusText) { emit statusTextChanged(statusText_ = statusText); }

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

	// These virtual functions allow subclasses to implement their unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation() { notifyStarted(); }

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { notifyPaused(); }

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { notifyResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation() { notifyCancelled(); }



	// These functions should be called _by the implementation_ notify the base class when implementation-initiated state changes happen: Starting->Running, Running->Succeeded, Anything->Failed, Pausing->Paused, Cancelling->Cancelled.
	///////////////////////

	/// Call this after receiving startImplementation() to inform the base class that the action has started, and we should go from Starting to Running.
	void notifyStarted();
	/// Call this to inform the base class that the action has succeeded. It should be OK to delete the action after receiving this.
	void notifySucceeded();
	/// Call this to inform the base class that the action has failed. It should be OK to delete the action after receiving this.
	void notifyFailed();
	/// Call this after receiving pauseImplementation() to inform the base class that the action has been paused, and we should go from Pausing to Paused.
	void notifyPaused();
	/// Call this after receiving resumeImplementation() to inform the base class that the action has been resumed, and we should go from Resuming to Running.
	void notifyResumed();
	/// Call this after receiving cancelImplementation() to inform the base class that the action has been cancelled, and we should go from Cancelling to Cancelled.
	void notifyCancelled();

private:
	State state_, previousState_;
	void setState(State newState);

	QList<AMActionPrereq*> prereqs_;
	PrereqBehaviour prereqBehaviour_;

	FailureResponse failureResponseInActionRunner_;
	FailureResponse failureResponseAsSubAction_;

	QPair<double,double> progress_;
	QDateTime startDateTime_, endDateTime_;
	QString statusText_;

private slots:
	// These slots are used to respond to events as the state machine runs.
	///////////////////
	void internalOnPrereqChanged();

private:
	/// Helper function to prompt the user for whether to cancel the action or wait for the prereqs to be satisfied
	PrereqBehaviour promptUserForPrereqBehaviour();

private:
	/// A pointer to our associated AMActionInfo object
	AMActionInfo* info_;
	/// This variable tracks the number of seconds that the action has spent in the Paused or Pausing states; we use it to implement runningTime().
	/*! \note It is only updated _after_ the action has resume()d.*/
	double secondsSpentPaused_;
	/// This variable tracks the number of seconds that the action spent in the WaitingForPrereqs state; we use it to implement runningTime(). It is not valid until we get past that state.
	double secondsSpentWaitingForPrereqs_;
	/// This variable stores the time at which we were last paused. It is set in pause().
	QDateTime lastPausedAt_;
};

#endif // AMACTION_H
