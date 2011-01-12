#ifndef AMWAITACTION_H
#define AMWAITACTION_H

#include "actions2/AMAction.h"

#include <QTimer>
#include <QTime>

#include "actions2/actions/AMWaitActionInfo.h"

/// This implementation of an AMAction simply waits for a number of seconds (as specified in its corresponding AMWaitActionInfo), and then completes successfully.  It's a great example for learning to subclass AMAction.
class AMWaitAction : public AMAction
{
    Q_OBJECT
public:
	// Constructors and copying
	//////////////////////////////

	/// Constructor. Requires and takes ownership of an existing AMWaitActionInfo \c info.  For ex: \c info = new AMWaitActionInfo(60) will wait for 60 seconds.
	AMWaitAction(AMWaitActionInfo* info, QObject *parent = 0);
	/// This convenience constructor is indentical to AMWaitAction(new AMWaitActionInfo(seconds))
	AMWaitAction(double seconds, QObject* parent = 0);
	/// Copy constructor: must re-implement, but can simply use the AMAction copy constructor to make copies of the info and prereqs. We need to reset our internal state variables (secondsSpentPaused_, in this case) to make the copy a "like new" action - ie, not run yet.
	AMWaitAction(const AMWaitAction& other) : AMAction(other) { secondsSpentPaused_ = 0; }
	/// Virtual copy constructor
	virtual AMWaitAction* createCopy() const { return new AMWaitAction(*this); }

	// Re-implemented public functions
	///////////////////////////////

	/// Specify that we can pause
	virtual bool canPause() const { return true; }

signals:

public slots:

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

	// These virtual functions allow us to implement our unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation();

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation();

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

protected slots:
	/// Every second, we emit a progress update with setProgress()
	void onProgressTick();

	/// When the main timer times out, we're finished.
	void onFinished();

protected:

	// Internal variables:
	/// Timer used to implement our countdown
	QTimer timer_;
	/// Timer used to issue progress updates on a per-second basis
	QTimer progressTick_;
	/// Number of seconds spent in "paused" state, which we add to our expected duration
	double secondsSpentPaused_;
	/// The instant in time when we were last paused
	QTime pausedAtTime_;

	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMWaitActionInfo. This makes it easier to access.
	const AMWaitActionInfo* waitInfo() const;


};

#endif // AMWAITACTION_H
