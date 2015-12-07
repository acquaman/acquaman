#ifndef AMWAITACTION_H
#define AMWAITACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMWaitActionInfo.h"

#include <QTime>
#include <QTimer>

/// This class encapsulates an action that will simply just wait for a specified amount of time.
class AMWaitAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMWaitAction(AMWaitActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	AMWaitAction(const AMWaitAction &other);
	/// Destructor.
	virtual ~AMWaitAction() {}

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const;

	/// Specify that we can pause.
	virtual bool canPause() const { return true; }
	/// Specify that we can skip.
	virtual bool canSkip() const { return true; }

	/// Virtual function that denotes that this action has no children.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMControlMoveActionInfo. This makes it easier to access.
	const AMWaitActionInfo *waitActionInfo() const { return qobject_cast<const AMWaitActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMControlMoveActionInfo. This makes it easier to access.
	AMWaitActionInfo* waitActionInfo() { return qobject_cast<AMWaitActionInfo *>(info()); }

protected slots:
	/// Handles the time out of the timer.  Sets the succeeded flag.
	virtual void onWaitTimerTimeout();
	/// Handles propogating the progress timer.
	void onProgressTimerTimeout();

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// Pauses the action by stopping the timer.
	virtual void pauseImplementation();
	/// Resumes the action by resuming the timer.
	virtual void resumeImplementation();
	/// Handles cancelling the scan.
	virtual void cancelImplementation();
	/// Handles skipping the action.
	virtual void skipImplementation(const QString &command);

	/// The timer that is used for the timed wait.
	QTimer waitTimer_;
	/// The member that keeps track of time (if the scan is paused/resumed.)
	QTime waitTime_;
	/// This holds the elapsed time of the action.
	double elapsedTime_;
	/// The progress timer.
	QTimer progressTimer_;
};

#endif // AMWAITACTION_H
