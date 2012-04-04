#ifndef AMLOOPACTION3_H
#define AMLOOPACTION3_H

#include "actions3/AMListAction3.h"

#include "actions3/AMLoopActionInfo3.h"

/// An AMLoopAction contains a list of sub-actions that, when the loop action is run, will be executed a fixed number of times.  It implements the AMNestedAction interface so that the sub-actions are visible inside the AMActionRunner views, so that users can drag-and-drop existing actions into/out-of the loop.
/*! The sub-actions are effectively used as templates, because a new copy of each sub-action will be made every time it is executed. You can configure whether you want the entire loop action logged as one, or every sub-action to be logged individually, by calling setShouldLogSubActionsSeparately(). */
class AMLoopAction3 : public AMListAction3
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

	/// Returns the current loop iteration (ie: loop counter).  Will be 0 before and while running the first loop, 1 during the second loop, and finally loopCount() after the last loop is done.
	int currentIteration() const { return currentIteration_; }

	// new public functions:
	/////////////////////////////

	/// Returns the number of iterations to loop for:
	int loopCount() const { return loopInfo()->loopCount(); }

    /// Public function to duplicate a set of sub-actions at \c indexesToCopy. The new sub-actions will all be inserted at the position after the last existing sub-action in \c indexesToCopy.  It uses insertSubAction() to add copies of the existing ones, and therefore will fail if the action is not in the AMAction::Constructed state.
    bool duplicateSubActions(const QList<int>& indexesToCopy);

signals:
	// see AMNestedAction::currentSubActionChanged(int) for when the current sub-action changes.

	/// Emitted whenever a loop completes / the next loop starts. Contains the current loop index
	void currentIterationChanged(int currentIteration);

public slots:
	/// Set the number of iterations to loop for. If the action is running and this is smaller than the number already completed, it will finish at the end of the current loop.
	void setLoopCount(int newLoopCount) { loopInfo()->setLoopCount(newLoopCount); }

protected slots:
	/// Called when the current action's state changes in any way. We do all of our decision making here on whether to succeed, fail, go on to the next sub-action or loop, etc.
    virtual void internalOnCurrentActionStateChanged(int newState, int oldState);
	/// Called when the current action's progress is updated. We use it to update our own progress
    virtual void internalOnCurrentActionProgressChanged(double numerator, double denominator);

protected:
    // Protected virtual functions to re-implement
    /////////////////////////////////////

    // These virtual functions allow subclasses to implement their unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
    /////////////////////////
    /// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
    virtual void startImplementation();
//	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
//    virtual void pauseImplementation() { setPaused(); }  // Using the AMListAction pauseImplementation().
//	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
//    virtual void resumeImplementation() { setResumed(); }  // Using the the AMListAction resumeImplementation().
    /// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
//    virtual void cancelImplementation();

    /// Helper function to manage action and loop iterations. Does everything we need to do to move onto the next action (either at the beginning, or after the last one completes).
    virtual void internalDoNextAction();

    /// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
    AMLoopActionInfo3* loopInfo() { return qobject_cast<AMLoopActionInfo3*>(info()); }
    /// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
    const AMLoopActionInfo3* loopInfo() const { return qobject_cast<const AMLoopActionInfo3*>(info()); }


    /// The iterator that keeps track of how far along the loop is.
    int currentIteration_;
    /// The current action that is being run in the loop.
    AMAction3 *currentSubAction_;
};

#endif // AMLOOPACTION_H
