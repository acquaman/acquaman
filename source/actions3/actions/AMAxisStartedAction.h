#ifndef AMAXISSTARTEDACTION_H
#define AMAXISSTARTEDACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMAxisStartedActionInfo.h"

class AMAxisStartedAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor
	Q_INVOKABLE AMAxisStartedAction(AMAxisStartedActionInfo *info, QObject *parent = 0);
	/// Copy Constructor
	AMAxisStartedAction(const AMAxisStartedAction &other);
	/// Destructor.
	virtual ~AMAxisStartedAction();

	/// Virtual copy constructor
	virtual AMAction3* createCopy() const { return new AMAxisStartedAction(*this); }

	/// Specify that we cannot pause
	virtual bool canPause() const { return false; }
	/// Specify that we cannot skip
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected:
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { setPaused(); }

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { setResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Since this action does not support skipping, the method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

protected:
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMAxisStartedActionInfo. This makes it easier to access.
	const AMAxisStartedActionInfo* axisStartedInfo() const { return qobject_cast<const AMAxisStartedActionInfo*>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMAxisStartedActionInfo. This makes it easier to access.
	AMAxisStartedActionInfo* axisStartedInfo() { return qobject_cast<AMAxisStartedActionInfo*>(info()); }
};

#endif // AMAXISSTARTEDACTION_H
