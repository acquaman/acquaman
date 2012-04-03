#ifndef AMACTION3_H
#define AMACTION3_H

#include <QObject>
#include "actions3/AMActionInfo3.h"

/// AMAction defines the interface for "actions" which can be run asynchronously in Acquaman's Workflow system. Actions (especially AMListAction) can also be useful on their own, outside of the workflow environment, to (a) make a set of events happen in a defined order, and (b) receive notification when those events succeed or fail. They can be used to easily build up complicated behaviour when moving beamlines around or building scan controllers.
/*!
  <b>Running actions</b>

Actions can be run on their own (by simply calling start()), or within the workflow system by using AMActionRunner.

When using AMActionRunner, they can be queued up to run one-by-one with AMActionRunner::addQueuedAction(), or run immediately (in parallel with whatever else might be running) with AMActionRunner::runActionImmediately().  In both cases, the actions are logged in the database after running.

<b>Actions and States</b>
\todo detailed docs

<b>Actions and AMActionInfo</b>

Most of the time, an AMAction subclass has a corresponding AMActionInfo subclass that describes the content/details/specs of the action, but omits all the implementation code required to actually run the action. (Users should typically configure/set up actions by <i>configuring the info</i> inside a corresponding AMActionInfoView.)  This separation allows users to review/copy/manage the AMActionInfos from their completed actions inside Acquaman programs that do not (or should not) have access to the implementation code.  For example, a user might want to review some scan actions done on Beamline A, while working on Beamline B; the user could open and review a BeamlineAScanActionInfo even though their program doesn't have access to BeamlineAScanAction.  (A more typical example might be reviewing a log of completed actions from many beamlines inside the Dataman take-home program.)

<b><i>Action/Info Registry</i></b>

When this one-to-one relationship between AMActions and AMActionInfos is used, the programmer should register the pair in the AMAction/Info Registry (AMActionRegistry::registerInfoAndAction()). This allows the workflow system to recreate valid AMActions given a particular AMActionInfo -- allowing a user to re-instantiate and re-run actions from their history of stored AMActionInfos.

<b>AMActionInfo, views, and editors</b>

\todo detailed description

<b>Prerequisites</b>

AMActionPrereq defines an interface for "prerequisites" which must be satisfied before an action can run.  One example of this (for a scan action) could be to ensure that no other scans are currently running on the beamline.

When you add one or more prereqs to an action with addPrereq(), the action can do one of four things (depending on the chosen prereqBehavior()) if any of them are not satisfied:

- Wait patiently for all the prerequisites to be satisfied
- Automatically cancel the action (like calling cancel())
- Automatically fail the action  (like the action going into the Failed state)
- Ask the user for whether to wait or cancel the action


 */
class AMAction3 : public QObject
{
	Q_OBJECT
public:
	// enum types defined
	/////////////////////////

	/// This enum describes the states that an action can be in.
	enum State { Constructed = 0,
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

	// Constructor and life-cycle management
	/////////////////

	/// Constructor: create an action to run the specified AMActionInfo.
    AMAction3(AMActionInfo3* info, QObject *parent = 0);

	/// Copy constructor. Takes care of making copies of the info and prerequisites. NOTE that the state() is reset to Constructed: the copy should look like a new action that hasn't been run yet.
    AMAction3(const AMAction3& other);

    /// This virtual function takes the role of a virtual copy constructor. This implementation always returns 0; therefore, all actions MUST RE-IMPLEMENT to be able to create and return a an independent copy of themselves. (This allows us to get a detailed subclass copy without knowing the type of the action.) NOTE that the returned instance should be a perfect copy except for the state() -- which will be reset to Constructed -- and any other subclass-specific state information: the copy should look like a new action that hasn't been run yet.
	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly.*/
    virtual AMAction3* createCopy() const { return 0; }

	/// Destructor: deletes the info and prerequisites
    virtual ~AMAction3();


	// Info API
	//////////////////////

	/// Returns the AMActionInfo subclass describing this action. The Info subclasses are a way to separate the description/content of actions from their implementation. This allows storing and viewing future/past actions inside programs which shouldn't have the capability to run them
	/*! (For example: a user viewing a history of their SGM beamline actions inside their take-home Dataman program, or when on another beamline. Theoretically, this approach could even let someone plan and queue up beamline actions at home to run once they get to the beamline.)

You can use a generic AMActionInfo in an AMAction-subclass constructor, but if you want to be able to re-create live actions from historical ones, you should provide a unique info subclass with sufficient information to completely specify/re-create the action, and register your Info-Action pair with AMActionRegistry::registerInfoAndAction().*/
    const AMActionInfo3* info() const { return info_; }
    AMActionInfo3* info() { return info_; }

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
	/// Returns the number of seconds that this action has actually been running for, <i>excluding</i> time spent in the Paused (and Pausing) and WaitingForPrereqs states.
    double runningTime() const { if(!startDateTime_.isValid()) return -1.0; return elapsedTime() - pausedTime(); }

	/// Returns a description of the action's status, for example, "Waiting for the beamline energy to reach the setpoint".  Implementations should update this while the action is running using setStatusText().
	/*! Even if you don't ever call this, the base class implementation will at at least call it on every state change with the name of the state. (For example: when the state changes to running, the status text will change to "Running".) You can always call it again after the state change/in-between state changes to provide more details to the user.*/
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


	// Nesting actions
	///////////////////////

	/// Sometimes, actions are formally bundled inside other actions using the AMNestedAction API. If this action happens to be found inside a nested action, this returns a pointer to the nested action. Normally, it returns 0.
    const AMAction3* parentAction() const { return parentAction_; }
	/// Sometimes, actions are formally bundled inside other actions using the AMNestedAction API. If this action happens to be found inside a nested action, this returns a pointer to the nested action. Normally, it returns 0.
    AMAction3* parentAction() { return parentAction_; }
	/// This function should not be considered part of the public interface. It is used by nested actions to set the parent action. NEVER CALL THIS FUNCTION.
    void setParentAction(AMAction3* parentAction) { parentAction_ = parentAction; }

    /// Pure virtual function that denotes that this action has children underneath it or not.
    virtual bool hasChildren() const = 0;
    /// Pure virtual function that returns the number of children for this action.
    virtual int numberOfChildren() const = 0;


public slots:
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

public:
	// Progress API
	////////////////////////

	/// Returns the numerator and denominator of the progress. (ie: completed steps out of total steps). If the progress is unknown, this will return (0,0). If the action hasn't started yet, this will return (-1, -1).
	QPair<double, double> progress() const { return progress_; }
	/// Returns the total expected duration in seconds. If the time to completion is unknown, this will return -1.
	double expectedDuration() const { return info_->expectedDuration(); }

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

	// convenience synonyms for state changed to a final state.
	//////////////////////////
	/// Emitted when the state changes to Succeeded
	void succeeded();
	/// Emitted when the state changes to Failed
	void failed();
	/// Emitted when the state changes to Cancelled
	void cancelled();


	/// Emitted when the progress changes. (\c numerator gives the amount done, relative to the total expected amount \c denominator. For example, \c numerator could be a percentage value, and \c denominator could be 100.)
	void progressChanged(double numerator, double denominator);
	/// If the action knows how long it will take, this signal is emitted with the total expected run time. If it doesn't know how long things will take, it could be emitted with (-1).
	void expectedDurationChanged(double expectedTotalDurationInSeconds);

	/// Emitted when the statusText() changes.
	void statusTextChanged(const QString& statusText);



protected slots:
	/// Implementations should call this to notify of their progress.   \c numerator gives the amount done, relative to the total expected amount \c denominator. For example, \c numerator could be a percentage value, and \c denominator could be 100.  If you don't know the level of progress, call this with (0,0).
	void setProgress(double numerator, double denominator) { progress_ = QPair<double,double>(numerator,denominator); emit progressChanged(numerator, denominator); }
	/// Implementations should call this to notify of the expected total duration (in seconds) of the action. If you don't know how long the action will take, call this with -1.
	void setExpectedDuration(double expectedTotalTimeInSeconds) { info_->setExpectedDuration(expectedTotalTimeInSeconds); }
	/// Implementations should call this to describe the action's status while running; for example, "Moving motor X to 30.4mm"
	void setStatusText(const QString& statusText) { emit statusTextChanged(statusText_ = statusText); }

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

    // These pure-virtual functions allow subclasses to implement their unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
    virtual void startImplementation() = 0;

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
    virtual void pauseImplementation() = 0;

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
    virtual void resumeImplementation() = 0;

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
    virtual void cancelImplementation() = 0;



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
    /// Changes states (if possible).
	void setState(State newState);
    /// Checks whether you can make the the transition to the new state.
    bool canChangeState(State newState) const;

	FailureResponse failureResponseInActionRunner_;
	FailureResponse failureResponseAsSubAction_;

	QPair<double,double> progress_;
	QDateTime startDateTime_, endDateTime_;
	QString statusText_;

    AMAction3* parentAction_;

private:
	/// A pointer to our associated AMActionInfo object
    AMActionInfo3* info_;
	/// This variable tracks the number of seconds that the action has spent in the Paused or Pausing states; we use it to implement runningTime().
	/*! \note It is only updated _after_ the action has resume()d.*/
	double secondsSpentPaused_;
	/// This variable stores the time at which we were last paused. It is set in pause().
	QDateTime lastPausedAt_;
};

#endif // AMACTION3_H
