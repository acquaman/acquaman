#ifndef AMLOOPACTION3_H
#define AMLOOPACTION3_H

#include "actions3/AMNestedAction3.h"

#include "actions3/AMLoopActionInfo3.h"

/// An AMLoopAction contains a list of sub-actions that, when the loop action is run, will be executed a fixed number of times.  It implements the AMNestedAction interface so that the sub-actions are visible inside the AMActionRunner views, so that users can drag-and-drop existing actions into/out-of the loop.
/*! The sub-actions are effectively used as templates, because a new copy of each sub-action will be made every time it is executed. You can configure whether you want the entire loop action logged as one, or every sub-action to be logged individually, by calling setShouldLogSubActionsSeparately(). */
class AMLoopAction3 : public AMNestedAction3
{
    Q_OBJECT
public:
	/// Constructor. Requires an AMLoopActionInfo for the \c info.
    Q_INVOKABLE AMLoopAction3(AMLoopActionInfo3* info, QObject *parent = 0);
	/// This convenience constructor is a synonym for AMLoopAction(new AMLoopActionInfo(iterations)).
    AMLoopAction3(int iterations, QObject* parent = 0);

	/// Copy constructor, to make a new runnable action just like \c other. Here we make a copy of all the sub-actions.
    AMLoopAction3(const AMLoopAction3& other);

	/// This virtual function takes the role of a virtual copy constructor.
    virtual AMAction3* createCopy() const { return new AMLoopAction3(*this); }

	/// Destructor: Deletes all the subActions_
    virtual ~AMLoopAction3();

	// Public virtual functions: re-implemented:
	//////////////////////

    /// Pure virtual function that denotes that this action has children underneath it or not.
    bool hasChildren() const { return true; }
    /// Pure virtual function that returns the number of children for this action.
    int numberOfChildren() const { return -1; }

	/// Specifies whether, when logging actions that are run with AMActionRunner, to log the entire action, or log the individual sub-actions as they complete.
	virtual bool shouldLogSubActionsSeparately() { return logSubActionsSeparately_; }

	/// Returns the number of sub-actions inside the loop
	virtual int subActionCount() const { return subActions_.count(); }
	/// Returns the subAction at \c index within the loop.
    virtual const AMAction3* subActionAt(int index) const { if(index<0 || index>=subActions_.count()) return 0; return subActions_.at(index); }
	/// Returns the subAction at \c index within the loop.
    virtual AMAction3* subActionAt(int index) { if(index<0 || index>=subActions_.count()) return 0; return subActions_.at(index); }

	/// Returns the index of the currently-active subaction. Return -1 if it doesn't make sense to consider any as current.
	virtual int currentSubActionIndex() const { return currentSubActionIndex_; }

	/// Returns the current loop iteration (ie: loop counter).  Will be 0 before and while running the first loop, 1 during the second loop, and finally loopCount() after the last loop is done.
	int currentIteration() const { return currentIteration_; }

	// new public functions:
	/////////////////////////////

	/// Set whether each individual sub-action should be logged separately as soon as it completes, or if the whole loop action should be logged as one long single action (in the user's action history log).
	void setShouldLogSubActionsSeparately(bool logSeparately) { logSubActionsSeparately_ = logSeparately; }

	/// Returns the number of iterations to loop for:
	int loopCount() const { return loopInfo()->loopCount(); }


signals:
	// see AMNestedAction::currentSubActionChanged(int) for when the current sub-action changes.

	/// Emitted whenever a loop completes / the next loop starts. Contains the current loop index
	void currentIterationChanged(int currentIteration);

public slots:
	/// Set the number of iterations to loop for. If the action is running and this is smaller than the number already completed, it will finish at the end of the current loop.
	void setLoopCount(int newLoopCount) { loopInfo()->setLoopCount(newLoopCount); }

protected:
	int currentIteration_, currentSubActionIndex_;
    QList<AMAction3*> subActions_;
	bool logSubActionsSeparately_;
    AMAction3* currentSubAction_;


	// Protected virtual functions to re-implement
	/////////////////////////////////////

	// AMNestedAction api:
	///////////////////////

	/// Implement to insert a sub- \c action at \c index. Can be assured by the base class that \c index is valid and that we're not running yet.
    virtual void insertSubActionImplementation(AMAction3 *action, int index);
	/// Implement to remove and return the sub-action at \c index. Can be assured by the base class that \c index is valid and that we're not running yet.
    virtual AMAction3* takeSubActionImplementation(int index);


	// These virtual functions allow subclasses to implement their unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

//	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
    virtual void pauseImplementation() { setPaused(); }

//	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
    virtual void resumeImplementation() { setResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Helper function to manage action and loop iterations. Does everything we need to do to move onto the next action (either at the beginning, or after the last one completes).
	void internalDoNextAction();
	/// Helper function that returns true if we should log a sub-action ourselves when it finishes. True if: we're running inside AMActionRunner, we're supposed to log our sub-actions separately, AND \c action is not itself a nested action that is supposed to log its own sub-actions seperately.  (If \c action IS a nested action, but it's supposed to be logged as one unit, then we'll still log it ourselves.)
    bool internalShouldLogSubAction(AMAction3* action);

	/// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
    AMLoopActionInfo3* loopInfo() { return qobject_cast<AMLoopActionInfo3*>(info()); }
	/// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
    const AMLoopActionInfo3* loopInfo() const { return qobject_cast<const AMLoopActionInfo3*>(info()); }

protected slots:
	/// Called when the current action's state changes in any way. We do all of our decision making here on whether to succeed, fail, go on to the next sub-action or loop, etc.
	void internalOnCurrentActionStateChanged(int newState, int oldState);
	/// Called when the current action's progress is updated. We use it to update our own progress
	void internalOnCurrentActionProgressChanged(double numerator, double denominator);
	/// Callend when the current action updates its status text.
	void internalOnCurrentActionStatusTextChanged(const QString& statusText);

};

#endif // AMLOOPACTION_H
