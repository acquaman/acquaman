#ifndef AMTIMEOUTLOOPACTION_H
#define AMTIMEOUTLOOPACTION_H

#include "actions3/AMLoopAction3.h"
#include "actions3/AMTimeoutLoopActionInfo.h"

#include <QTimer>

/// Class extends the normal loop action to have a timeout before it starts the next set of actions.
class AMTimeoutLoopAction : public AMLoopAction3
{
	Q_OBJECT

public:
	/// Constructor.  Requires an AMTimoutLoopActionInfo.
	Q_INVOKABLE AMTimeoutLoopAction(AMTimeoutLoopActionInfo *info, QObject *parent = 0);
	/// This convenience constructor is a synonym for AMTimeoutLoopAction(new AMTimeoutLoopActionInfo(iterations, time)).
	AMTimeoutLoopAction(int iterations, double timeoutTime, QObject *parent = 0);
	/// Destructor.
	~AMTimeoutLoopAction();

	/// Copy constructor.
	AMTimeoutLoopAction(const AMTimeoutLoopAction &original);

	/// This virtual fucntion takes the role of a virtual copy constructor.
	virtual AMAction3 *createCopy() const { return new AMTimeoutLoopAction(*this); }

	/// Returns the timeout time.
	double timeoutTime() const { return timeoutLoopInfo()->timeoutTime(); }

signals:
	/// Notifier that the timeout time changed.
	void timeoutTimeChanged(double);

public slots:
	/// Sets the timeout time.
	void setTimeoutTime(double newTime) { timeoutLoopInfo()->setTimeoutTime(newTime); }

protected slots:
	/// Helper function that handles starting the next iteration.
	virtual void internalDoNextIteration();
	/// Updates the timer if the info timeout time changes.
	void onTimeoutTimeChanged();

protected:
	/// Helper function to manage action and loop iterations. Does everything we need to do to move onto the next action (either at the beginning, or after the last one completes).
	virtual void internalDoNextAction();

	/// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
	AMTimeoutLoopActionInfo *timeoutLoopInfo() { return qobject_cast<AMTimeoutLoopActionInfo*>(info()); }
	/// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
	const AMTimeoutLoopActionInfo *timeoutLoopInfo() const { return qobject_cast<const AMTimeoutLoopActionInfo*>(info()); }

	/// Flag for letting us know if we are waiting on the timeout.  If the timer goes off and we aren't ready we should fail.
	bool waitingForTimeout_;
	/// The timer used for the timeout.
	QTimer timeoutTimer_;
};

#endif // AMTIMEOUTLOOPACTION_H
