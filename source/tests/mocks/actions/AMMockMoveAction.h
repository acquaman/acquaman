#ifndef AMMOCKMOVEACTION_H
#define AMMOCKMOVEACTION_H

#include "actions3/AMAction3.h"
#include "tests/mocks/beamline/AMMockControl.h"

#include <QTimer>

/*!
  * A class representing an action used internally by AMMockControls to simulate
  * their movement. This is performed by through a QTimer which operates on a tick
  * frequency of 0.1s, emitting moveValueChanged(double) each tick with the updated
  * position. AMMockControls can then listen to this signal in order to update their
  * value(). The mock action uses the timer frequency to make the value calculation,
  * producing a movement velocity of 1unit/s
  */
class AMMockMoveAction : public AMAction3
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of a mock move action, which will move the provided
	  * control to the provided setpoint.
	  */
    AMMockMoveAction(AMActionInfo3* info,
	                 AMMockControl* control,
	                 double setpoint,
	                 QObject* parent = 0);

	virtual ~AMMockMoveAction() {}

	/// The mock move action can not pause.
	virtual bool canPause() const { return false; }
	/// The mock move action can be parallelized.
	virtual bool canParallelize() const { return true; }
	/// The mock move action can skip.
	virtual bool canSkip() const { return true; }

	/// The mock move action has no children.
	virtual bool hasChildren() const { return false; }
	/// The mock move action has no children.
	virtual int numberOfChildren() const { return 0; }
signals:

	/// Signal which indicates value() change for the control being moved.
	void moveValueChanged(double value);
protected slots:

	/// Handles the move update timer timing out. Calculates the new position value
	/// and emits the moveValueChanged(double) signal.
	void onMoveUpdateTimerTimeout();

protected:
	void startImplementation();

	/// Does nothing. The action cannot be paused.
	virtual void pauseImplementation();

	/// Does nothing. The action cannot be paused.
	virtual void resumeImplementation();

	/// Stops the timer and sets the state to cancelled.
	virtual void cancelImplementation();

	/// Stops the timer and sets the state to succeeded.
	virtual void skipImplementation(const QString &command);

	AMMockControl* control_;
	double setpoint_;
	QTimer moveUpdateTimer_;

	double startPosition_;
};

#endif // AMMOCKMOVEACTION_H
