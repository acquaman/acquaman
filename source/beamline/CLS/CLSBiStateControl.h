#ifndef CLSBISTATECONTROL_H
#define CLSBISTATECONTROL_H

#include "beamline/AMControl.h"

/*!
  This class implements AMControl to encapsulate the access convention to valves and shutters at the CLS.  Unlike most other controls/process variables, valves and shutters
  have three PVs: one for the current state of the valve/shutter, one to open it, and one to close it.
  */
class CLSBiStateControl : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.  Builds the bi-state control based on the following parameters.
	/*!
   \param name is the name of the control.
   \param descrtiption is a human readable description for the control.
   \param state is the PV name for the state PV.
   \param open is the PV name for the open PV.
   \param close is the PV name for the close PV.
   \param statusChecker is used to determine if the control is moving.
   \param parent provides a parent object.
   */
	CLSBiStateControl(const QString &name,
					  const QString &description,
					  const QString &state,
					  const QString &open,
					  const QString &close,
					  AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(2),
					  QObject *parent = 0);

	/// This represents the current value/position of the control. Returns one of three values: 0 (closed), 1 (open), and 2 (between).  Synonomous with state().
	virtual double value() const { return state(); }

	/// this is the "setpoint": the last place the control was told to go:
	virtual double setpoint() const { return setpoint_; }

	/// this indicates whether a control is "in position" (ie: its value is within tolerance of the setpoint)
	virtual bool inPosition() const { return fabs(value()-setpoint()) < tolerance(); }

	/// this indicates whether a contorl is "within tolerance" of a given target (ie: the target specified is the same as the current value within the set tolerance)
	virtual bool withinTolerance(double target) const { return fabs(value()-target) < tolerance(); }

	/// \name Capabilities
	/// These indicate the current cabilities and status of this control. Unconnected controls can't do anything.
	//@{
	/// Indicates a "fully-functional" control, ready for action
	virtual bool isConnected() const { return statePV_->readReady() && openPV_->writeReady() && closePV_->writeReady(); }
	/// Indicates that this control \em can currently take measurements.
	virtual bool canMeasure() const { return statePV_->readReady(); }
	/// Indicates that this control type \em should (assuming it's connected) be able to measure values.
	virtual bool shouldMeasure() const { return true; }
	/// Indicates thatthis control \em can (currently) move to setpoints:
	virtual bool canMove() const { return openPV_->writeReady() && closePV_->writeReady(); }
	/// Indicates that this control \em should (assuming it's connected) be able to move to setpoints:
	virtual bool shouldMove() const { return true; }
	/// Indicates that this control \em can (currently) issue stop() commands while moves are in progress.  Bi-state controls cannot be stopped.
	virtual bool canStop() const { return false; }
	/// Indicates that this control \em should (assuming it's connected) be able to issue stop() commands while moves are in progress.  Bi-state controls cannot be stopped.
	virtual bool shouldStop() const { return false; }
	/// Indicates that this control is currently moving.  For a bi-state control this is defined as the "Between".
	virtual bool isMoving() const { return (*statusChecker_)(statePV_->getInt()) || moveInProgress_; }

	/// Indicates that this control is moving (as a result of a move you specifically requested)
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// \name Information on the allowed range for this control:
	//@{
	/// These are the minimum and maximum allowed values for this control.
	virtual QPair<double,double> range() const { return QPair<double,double>(minimumValue(), maximumValue()); }
	/// Due to the binary nature of these controls, I enforce that the minimum must be zero.  This is because there is no guarantee that the PVs will be set or that there is any preconceived setup done for the controls.
	virtual double minimumValue() const { return 0; }
	/// Due to the binary nature of these controls, I enforce that the maximum must be one.  This is because there is no guarantee that the PVs will be set or that there is any preconceived setup done for the controls.
	virtual double maximumValue() const { return 1; }

	/// Get the current status of the control. 0 is closed, 1 is open, and 2 is between.
	int state() const
	{
		switch(statePV_->getInt()){
		case 1:
			return 1;
		case 4:
			return 0;
		default:
			return 2;
		}
	}
	/// Same as state() but returns a QString. Passes Open, Between, or Closed.
	QString stateString() const
	{

		switch(statePV_->getInt()){
		case 1:
			return QString("Open");
		case 4:
			return QString("Closed");
		default:
			return QString("Between");
		}
	}

signals:
	/// Notifies that the statePV_ has changed and passes on the state.
	void stateChanged(int);

public slots:
	/// This is used to move the control to a setpoint.  Must reimplement for actual controls
	virtual void move(double setpoint)
	{
		if ((int)setpoint == 1)
			open();
		else if ((int)setpoint == 0)
			close();
		else
			emit moveFailed(AMControl::OtherFailure);
	}
	/// Opens the control.  This activates the control and moves it to the "Open" state.  Synonomous with move(1).
	void open() {
		if(setpoint_ != 1) {
			moveInProgress_ = true;
			emit moveStarted();
		}
		setpoint_ = 1; openPV_->setValue(1);
	}
	/// Closes the control.  This deactivates the control and moves it to the "Closed" state.  Synonomous with move(0).
	void close() {
		if(setpoint_ != 0) {
			moveInProgress_ = true;
			emit moveStarted();
		}
		setpoint_ = 0; closePV_->setValue(1);
	}

	/// This is used to cancel or stop a move in progress. Does not do anything for this type of control.
	virtual bool stop() { return false; }

protected slots:
	/// Helper function to review the connection state and make sure that connected(bool) is emitted for all changes to isConnected().
	void onConnectionStateChanged();
	/// Called to emit stateChanged() and valueChanged() when the statePV_ changes.
	void onStateChanged();


protected:
	/// This is the process variable that holds the state of the control.
	AMProcessVariable *statePV_;
	/// This is the process variable that holds the open control for the the control.
	AMProcessVariable *openPV_;
	/// This is the process variable that holds the close control for the control.
	AMProcessVariable *closePV_;

	/// An evaluator object used to determine if the status value indicates moving
	AMAbstractControlStatusChecker* statusChecker_;

	/// Holds the setpoint for the control.
	int setpoint_;

	/// Holds the last value of isConnected()
	bool isConnected_;

	/// Holds whether a move (your move) is in progress.
	bool moveInProgress_;

	/// Holds the last value of isMoving_;
	bool isMoving_;
};

#endif // CLSBISTATECONTROL_H
