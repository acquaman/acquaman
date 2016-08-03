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


#ifndef AMACTION3_H
#define AMACTION3_H

#include <QObject>
#include "actions3/AMActionInfo3.h"

#define AMACTION3_CANNOT_CANCEL_NOT_IN_FINAL_STATE 270701
#define AMACTION3_CANNOT_PAUSE_ACCORDING_TO_ACTION 270702
#define AMACTION3_CANNOT_PAUSE_NOT_IN_PAUSABLE_STATE 270703
#define AMACTION3_CANNOT_RESUME_NOT_CURRENTLY_PAUSED 270704
#define AMACTION3_NOTIFIED_STARTED_BUT_NOT_TOLD_TO_START 270705
#define AMACTION3_NOTIFIED_SUCCEEDED_BUT_NOT_YET_POSSIBLE 270706
#define AMACTION3_NOTIFIED_FAILED_BUT_NOT_YET_POSSIBLE 270707
#define AMACTION3_NOTIFIED_PAUSED_BUT_NOT_CURRENTLY_POSSIBLE 270708
#define AMACTION3_NOTIFIED_RESUMED_BUT_NOT_CURRENTLY_POSSIBLE 270709
#define AMACTION3_NOTIFIED_CANCELLED_BUT_NOT_YET_POSSIBLE 270710
#define AMACTION3_CANNOT_SKIP_NOT_POSSIBLE 270711
#define AMACTION3_CANNOT_SKIP_NOT_CURRENTLY_RUNNING 270712

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

<b>Skipping</b>

Actions also have the ability to "skip" or stop before they have finished as originally setup.  This is meant to be distinctly different from cancelling due to the fact that
cancelling has an inherent meaning of "failed" or "not quite right".  Therefore, the skip quality is meant to provide the idea of "good enough".  Whether this is a scan that
already has enough good data and you want to save time or you are doing a set of actions in a loop and figure it's good enough at 7 of 10, you can now successfully complete that
action with the skip option.

To use skip, actions must provide a set of "skip options" that are available and then implement what each of those options do in the skip() method.  The skip method takes a
"command" which is a string - which obviously must match a skip option.  For generality, skip will do nothing so that if the action does not support skipping and the method
is evoked regardless, nothing will break.

 */
class AMAction3 : public QObject
{
	Q_OBJECT
public:
	// enum types defined
	/////////////////////////

	/// This enum describes the states that an action can be in.
	enum State {
			 Constructed = 0,
			 Starting,
			 Running,
			 Pausing,
			 Paused,
			 Resuming,
			 Cancelling,
			 Cancelled,
			 Succeeded,
			 Failed,
			 Skipping
		   };
	/// When running inside AMActionRunner, or as a sub-action in AMListAction, this enum specifies what to do if the action fails
	enum FailureResponse { MoveOnResponse = 0,
				   AttemptAnotherCopyResponse,
				   PromptUserResponse
				 };
	/// ActionValidity says whether a potential workflow action is valid based on beamline knowledge
	enum ActionValidity{
		ActionNeverValid = 0,			///< NeverValid refers to actions which are entirely unsupported (possibly depricated or not yet implemented)
		ActionCurrentlyValid = 1,		///< CurrentlyValid refers to actions which can be run at the moment
		ActionNotCurrentlyValid = 2		///< NotCurrentlyValid refers to actions that cannot be run at the moment, but might be able to be run later. For example, a scan requiring a certain detector may not be runnable at the moment but could be runnable if the detector becomes available before the scan is actually run in the workflow.
	};

	// Constructor and life-cycle management
	/////////////////

	/// Constructor: create an action to run the specified AMActionInfo.
	AMAction3(AMActionInfo3* info, QObject *parent = 0);

	/// Copy constructor. Takes care of making copies of the info and prerequisites. NOTE that the state() is reset to Constructed: the copy should look like a new action that hasn't been run yet.
	AMAction3(const AMAction3& other, QObject *parent = 0);

	/// This virtual function takes the role of a virtual copy constructor. This implementation always returns 0 therefore, all actions MUST RE-IMPLEMENT to be able to create and return a an independent copy of themselves. (This allows us to get a detailed subclass copy without knowing the type of the action.) NOTE that the returned instance should be a perfect copy except for the state() -- which will be reset to Constructed -- and any other subclass-specific state information: the copy should look like a new action that hasn't been run yet.
	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly.*/
	virtual AMAction3* createCopy() const { return 0; }

	/// Destructor: deletes the info and prerequisites
	virtual ~AMAction3();

	/// Returns whether or not this action is currently scheduled for deletion
	bool isScheduledForDeletion() const;

	/// Returns whether or not this action has finished logging (it may have never starting logging and that's okay because the default value is true)
	bool isLoggingFinished() const;


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
	/// Returns the total number of seconds since this action was start()ed, or 0 if it hasn't started running yet. As long as the action has started, this will be equivalent to QDateTime::currentDateTime() - startDateTime().
	/*! \note This will include all the time spent in the Paused state as well as WaitingForPrereqs. To get just the actual running time, \see runningTime(). */
	double elapsedTime() const;
	/// Returns the number of seconds that this action has been in the Paused and Pausing states for.
	double pausedTime() const;
	/// Returns the number of seconds that this action has actually been running for, <i>excluding</i> time spent in the Paused (and Pausing) and WaitingForPrereqs states.
	double runningTime() const;

	/// Returns a description of the action's status, for example, "Waiting for the beamline energy to reach the setpoint".  Implementations should update this while the action is running using setStatusText().
	/*! Even if you don't ever call this, the base class implementation will at at least call it on every state change with the name of the state. (For example: when the state changes to running, the status text will change to "Running".) You can always call it again after the state change/in-between state changes to provide more details to the user.*/
	QString statusText() const { return statusText_; }

	/// Returns an ActionValidity enum for whether this action is valid. Default is AMAction3::ActionCurrentlyValid
	virtual AMAction3::ActionValidity isValid() { return AMAction3::ActionCurrentlyValid; }
	virtual QString validationWarningMessage() { return ""; }


	// States
	///////////////////

	/// Returns the current state of the action
	State state() const { return state_; }
	/// Returns a string describing the given \c state
	QString stateDescription(State state);

	/// Returns whether the action is in a final state (Succeeded, Failed, or Cancelled). All cleanup should be done before entering these states, so it should be OK to delete an action once it is in a final state.
	bool inFinalState() const { return state_ == Succeeded || state_ == Failed || state_ == Cancelled; }

	/// Returns whether the action is running or not
	bool isRunning() const { return state_ == Running; }

	/// Returns whether the action is paused or not
	bool isPaused() const { return state_ == Paused; }
	/// This virtual function can be re-implemented to specify whether the action has the capability to pause. By default, it returns false (ie: cannot pause).
	virtual bool canPause() const { return false; }

	/// This virtual function can be reimplemented to specify whether the action can be placed inside a parallel list.  By default, it returns true (eg: can be parallelized).
	virtual bool canParallelize() const { return true; }

	/// This virtual method returns whether the action supports skipping. By default, it returns false (ie: cannot skip).
	virtual bool canSkip() const { return false; }
	/// Returns the skip options that this action will support.
	virtual QStringList skipOptions() const { return skipOptions_; }


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

	QString skipCommand() const { return skipCommand_; }


public slots:
	// External requests to change the state: start(), cancel(), pause(), and resume().
	//////////////////
	// All of these requests to change the state return false if not allowed from the current state.

	/// Start running the action. Allowed from Constructed. State will change to WaitingForPrereqs or Starting.
	bool start();

	/// Explicitly cancel the action. Allowed from anything except Cancelling, Succeeded, or Failed. The state will change Cancelling. The action could take a while to finish cancelling itself, the state will change to Cancelled when that finally happens.
	bool cancel();

	/// For actions that support pausing, request to pause the action. Allowed from WaitingForPrereqs or Running, if canPause() is true. The state will change to Paused or Pausing (respectively).
	bool pause();

	/// For actions that support pausing, request to pause the action. Allowed from Paused only. The state will change to Resuming.
	bool resume();

	/// For actions that support skipping, uses \param command to successfully end the action perscribed by the command.  Returns true if it was successful.
	bool skip(const QString &command);

	/// Setting to true will cause the action to generate  messages with AMAgnositicDataAPI if it is capable of doing so.
	void setGenerateScanActionMessage(bool generateScanActionMessages) { generateScanActionMessages_ = generateScanActionMessages; }

	/// Called to let this action delete itself at the appropriate time. If called more than once, subsequent calls will do nothing. Subclasses may be much more complicated and can use scheduleForDeletionImplementation() to do the fancy work.
	void scheduleForDeletion();

	/// Called to set the isLoggingFinishedFlag for logged actions
	void setIsLoggingFinished(bool isLoggingFinished);

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

	/// Specific action types should check this flag to see if they need to send out messages with AMAgnositicDataAPI. If true, messages should be generated.
	bool generateScanActionMessages() const { return generateScanActionMessages_; }

	/// The message that is displayed to detail reasons why an action failed, or the settings of the action.
	const QString failureMessage() const { return failureMessage_; }

signals:

	/// Emitted whenever the state() of the action changes
	void stateChanged(int fromState, int toState);

	// convenience synonyms for state changed to a final state.
	//////////////////////////
	/// Emitted when the state changes to Paused.  Synonym for stateChanged(Pausing, Paused).
	void paused();
	/// Emitted when the state changes to Resumed.  Synonym for stateChanged(Resuming, Running).
	void resumed();
	/// Emitted when the state changes to Running.  Synonym for stateChanged(Constructed, Running).
	void started();
	/// Emitted when the state changes to Succeeded.
	void succeeded();
	/// Emitted when the state changes to Failed.
	void failed();
	/// Emitted when the state changes to Cancelled.
	void cancelled();


	/// Emitted when the progress changes. (\c numerator gives the amount done, relative to the total expected amount \c denominator. For example, \c numerator could be a percentage value, and \c denominator could be 100.)
	void progressChanged(double numerator, double denominator);
	/// If the action knows how long it will take, this signal is emitted with the total expected run time. If it doesn't know how long things will take, it could be emitted with (-1).
	void expectedDurationChanged(double expectedTotalDurationInSeconds);

	/// Emitted when the statusText() changes.
	void statusTextChanged(const QString& statusText);

	/// Emitted when the isLoggingFinished flag changes states
	void isLoggingFinishedChanged(bool isLoggingFinished);
	/// Emitted when the isLoggingFinished flag goes from false to true
	void loggingIsFinished();


protected slots:
	/// Implementations should call this to notify of their progress.   \c numerator gives the amount done, relative to the total expected amount \c denominator. For example, \c numerator could be a percentage value, and \c denominator could be 100.  If you don't know the level of progress, call this with (0,0).
	void setProgress(double numerator, double denominator);
	/// Implementations should call this to notify of the expected total duration (in seconds) of the action. If you don't know how long the action will take, call this with -1.
	void setExpectedDuration(double expectedTotalTimeInSeconds);
	/// Implementations should call this to describe the action's status while running, for example, "Moving motor X to 30.4mm"
	void setStatusText(const QString& statusText);

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic, they only need to fill in their pieces.

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

	/// Implementation method for skipping.  If the action supports skipping then this should do all the necessary actions for stopping the action.  This method is a bit of an exception in that setSkipped() is not called inside this method (not an absolute, but likely).  Therefore, the part of the action that DOES do the actual work must call setSkipped().
	virtual void skipImplementation(const QString &command) = 0;

	/// Implementation method for scheduleForDeletion. Default is simply to call deleteLater(), subclasses can implement more complex behaviors.
	virtual void scheduleForDeletionImplementation();



	// These functions should be called _by the implementation_ notify the base class when implementation-initiated state changes happen: Starting->Running, Running->Succeeded, Anything->Failed, Pausing->Paused, Cancelling->Cancelled.
	///////////////////////

	/// Call this after receiving startImplementation() to inform the base class that the action has started, and we should go from Starting to Running.
	void setStarted();
	/// Call this to inform the base class that the action has succeeded. It should be OK to delete the action after receiving this.
	void setSucceeded();
	/// Call this to inform the base class that the action has failed. It should be OK to delete the action after receiving this.
	void setFailed(const QString& = QString());
	/// Call this after receiving pauseImplementation() to inform the base class that the action has been paused, and we should go from Pausing to Paused.
	void setPaused();
	/// Call this after receiving resumeImplementation() to inform the base class that the action has been resumed, and we should go from Resuming to Running.
	void setResumed();
	/// Call this after receiving cancelImplementation() to inform the base class that the action has been cancelled, and we should go from Cancelling to Cancelled.
	void setCancelled();
	/// Call this after receiving skipImplementation() to inform the base class that the action has is now being skipped and we should go from Skipping to Succeeded.
	void setSkipped();

	/// Checks whether you can make the the transition to the new state.
	bool canChangeState(State newState) const;

protected:
	/// The list of skip options that the scan can perform.
	QStringList skipOptions_;

	/// Specific action types should check this flag to see if they need to send out messages with AMAgnositicDataAPI. If true, messages should be generated.
	bool generateScanActionMessages_;

	QString skipCommand_;

private:
	/// Changes states (if possible).
	void setState(State newState);

	/// The current state.
	State state_;
	/// The previous state.
	State previousState_;

	/// Failure response that should be shown in the action runner.
	FailureResponse failureResponseInActionRunner_;
	/// Failure response if the current action is a sub action.
	FailureResponse failureResponseAsSubAction_;

	/// The progress of the action. <elapsed, total>
	QPair<double,double> progress_;
	/// The time when the action was started.
	QDateTime startDateTime_;
	/// The time when the action was finished.
	QDateTime endDateTime_;
	/// The status text that is displayed in the current action view of the action runner view (ie: Workflow).
	QString statusText_;

	/// A pointer to the parent action of this action.  If this is not a list action this will likely always be 0.
	AMAction3 *parentAction_;

	/// A pointer to our associated AMActionInfo object
	AMActionInfo3 *info_;
	/// This variable tracks the number of seconds that the action has spent in the Paused or Pausing states, we use it to implement runningTime().
	/*! \note It is only updated _after_ the action has resumed().*/
	double secondsSpentPaused_;
	/// This variable stores the time at which we were last paused. It is set in pause().
	QDateTime lastPausedAt_;
	/// Optional failure string that Actions can use to report why they failed or their current settings
	QString failureMessage_;

	/// Flag for holding whether or not logging to a database is finished. Some actions never get logged, so the default value is true
	bool isLoggingFinished_;
	/// Flag for holding whether or not this action has been schdeduled for deletion
	bool isScheduledForDeletion_;
};

#endif // AMACTION3_H
