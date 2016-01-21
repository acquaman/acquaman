#ifndef AMMOCKMOVEACTION_H
#define AMMOCKMOVEACTION_H

#include "actions3/AMAction3.h"
#include "tests/mocks/beamline/AMMockControl.h"

#include <QTimer>

class AMMockMoveAction : public AMAction3
{
	Q_OBJECT
public:
    AMMockMoveAction(AMActionInfo3* info,
	                 AMMockControl* control,
	                 double setpoint,
	                 QObject* parent = 0);

	virtual ~AMMockMoveAction() {}

	/// This virtual function can be re-implemented to specify whether the action has the capability to pause.
	virtual bool canPause() const { return true; }
	/// This virtual function can be reimplemented to specify whether the action can be placed inside a parallel list.
	virtual bool canParallelize() const { return true; }
	/// This virtual method returns whether the action supports skipping.
	virtual bool canSkip() const { return true; }

	/// No children
	virtual bool hasChildren() const { return false; }
	/// No children
	virtual int numberOfChildren() const { return 0; }
signals:

	void moveValueChanged(double value);
protected slots:

	void onMoveUpdateTimerTimeout();

protected:
	void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation();

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation();

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Implementation method for skipping.  If the action supports skipping then this should do all the necessary actions for stopping the action.  This method is a bit of an exception in that setSkipped() is not called inside this method (not an absolute, but likely).  Therefore, the part of the action that DOES do the actual work must call setSkipped().
	virtual void skipImplementation(const QString &command);

	AMMockControl* control_;
	double setpoint_;
	QTimer moveUpdateTimer_;

	double startPosition_;
};

#endif // AMMOCKMOVEACTION_H
