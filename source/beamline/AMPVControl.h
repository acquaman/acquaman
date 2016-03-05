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

#ifndef AMPVCONTROL_H
#define AMPVCONTROL_H

#include "beamline/AMControl.h"
#include "beamline/AMProcessVariable.h"

#define AMPVCONTROL_READ_PROCESS_VARIABLE_ERROR 280401
#define AMPVCONTROL_COULD_NOT_MOVE_BASED_ON_CANMOVE 280402
#define AMPVCONTROL_WRITE_PROCESS_VARIABLE_ERROR 280403
#define AMPVCONTROL_MOVE_TIMEOUT_OCCURED_NOT_DURING_MOVE 280404
#define AMPVCONTROL_STATUS_PROCESS_VARIABLE_ERROR 280405
#define AMPVCONTROL_STATUS_WRITE_PROCESS_VARIABLE_ERROR 280406
#define AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING 280407
#define AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING_EXTERNAL 280408

/**
 * \defgroup control Beamline Control with AMControl and AMProcessVariable
 @{
 */

/// This class provides an AMControl with measure-only capability, based on an Epics Process Variable implementation.
/*!
The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using a single Process Variable.  It cannot be used to set values.  An example usage would be measuring the storage ring current.  There is no way to decide whether the control is "moving" or not (How do you define if the ring current is "moving"?), so this behavior is undefined.

The unique behavior is defined as:

<table>
<tr><td>isConnected means:		<td>the feedback process variable is connected.
<tr><td>isMoving() means:		<td>[always false; no good way to determine this]
<tr><td>moveSucceeded() means:	<td>[never happens]
<tr><td>moveFailed() means:		<td>[happens on any attempted move; can't do that!]
</table>

Most useful members for using this class:

- value()
- name()
- isConnected()
- valueChanged()

*/
class AMReadOnlyPVControl : public AMControl {

	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique description of this control
  \param readPVname The EPICS channel-access name for this Process Variable
  \param parent QObject parent class
  */
	virtual ~AMReadOnlyPVControl();
	AMReadOnlyPVControl(const QString& name, const QString& readPVname, QObject* parent = 0, const QString decription = "");

	/// Ensures that the PV will monitor limits within the standard field names (HOPR and LOPR or DRVH and DRVL)
	virtual void enableLimitMonitoring();
	/// \name Reimplemented Public Functions:
	//@{
	/// most recent value of this measurement
	virtual double value() const { return readPV_->lastValue(); }
	/// Indicates that the Control is ready for use.  In this case, means that the readPV is connected and readable.
	virtual bool isConnected() const { return readPV_->readReady(); }
	/// Indicates whether the readPV is readable.
	virtual bool canMeasure() const { return readPV_->canRead(); }
	/// Indicates that we \em should be able to measure from this Control.  Always true.
	virtual bool shouldMeasure() const { return true; }

	/// Minimum value taken from the readPV's lower graphing limit within EPICS.
	virtual double minimumValue() const { return lowLimitValue_; }
	/// Maximum value taken from the readPV's upper graphing limit within EPICS.
	virtual double maximumValue() const { return highLimitValue_; }

	/// Returns the alarm severity for the readPV:
	virtual int alarmSeverity() const { return readPV_->alarmSeverity(); }
	/// Returns the alarm status for the readPV:
	virtual int alarmStatus() const { return readPV_->alarmStatus(); }
	//@}

	/// \name Additional public functions:
	//@{
	/// The EPICS channel-access name of the underlying readPV.
	QString readPVName() const { return readPV_->pvName(); }
	/// Read-only access to the underlying readPV.
	const AMProcessVariable* readPV() const { return readPV_; }
	//@}



signals:
	/// Reports that the readPV failed to connect to the EPICS server within the timeout.

	///  This extra signal is specialized to report on PV channel connection status.  You should be free to ignore it and use the signals defined in AMControl.
	void readConnectionTimeoutOccurred();

protected:

	/// Pointer to ProcessVariable used to read feedback value
	AMProcessVariable* readPV_;
	/// Pointer to ProcessVariable used to minitor the lower limit, if hasLimits is specified.
	AMProcessVariable* lowLimitPV_;
	/// Pointer to ProcessVariable used to monitor the uper limit, if hasLimits is specified.
	AMProcessVariable* highLimitPV_;
	/// Used for change-detection of isConnected() for the connected() signal
	bool wasConnected_;
	/// Cached low limit value, to ensure that the correct value is returned when monitoring.
	double lowLimitValue_;
	/// Cached high limit value, to ensure that the correct value is returned when monitoring.
	double highLimitValue_;


protected slots:
	/// Handles the low limit PV changing. Converts the limit before forwarding the signal on.
	virtual void setLowLimitValue(double newLowLimit);
	/// Handles the high limit PV changing. Converts the limit before forwarding the signal on.
	virtual void setHighLimitValue(double newHighLimit);

	/// This is called when reading the PV's control information completes successfully.
	virtual void onReadPVInitialized();
	/// Handles updating the low limit value when the low limit pv is initialized.
	virtual void onLowLimitPVInitialized();
	/// Handles updating the high limit value when the high limit pv is initialized.
	virtual void onHighLimitPVInitialized();

	/// You can also monitor the readConnectionTimeoutOccurred() signal.
	void onConnectionTimeout() { setUnits("?"); emit connected(false); emit error(AMControl::CannotConnectError); }

	/// This is called when a PV channel connects or disconnects. Emits connected(bool) if isConnected() has changed compared to wasConnected_.
	void onPVConnected(bool connected);
	/// This is called when there is a Read PV channel error:
	void onReadPVError(int errorCode);

};

/// This class provides an AMControl with measure and move capability, based on an Epics Process Variable implementation.
/*!
The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using one Process Variable, and writes out a setpoint using another.  An example usage would be setting and monitoring a high-voltage bias power supply.  There is no explicit way to decide whether the control is "moving" or not, so we define it like this:

- isConnected means:		the feedback process variable AND the setpoint process variable are connected.
- isMoving() means:			a move() has been issued, the completionTimeout() hasn't occurred, and the value hasn't entered tolerance. same as moveInProgress()
- moveSucceeded() means:	after a move() has been issued, the value entered tolerance before the completionTimeout() occurred
- moveFailed() means:		after starting a move, the completionTimeout() occurred before the value reached tolerance

Since we don't have a way to determine if the control is actually moving, this class introduces a completionTimeout(). If the value doesn't reach within tolerance() of setpoint() within a certain time, the move will fail.  One limitation is that if the control overshoots the setpoint, we can't tell the difference between this, and a successful move that rises later.  See AMPVwStatusControl for more sophisticated move completion testing.

For these to make sense, make sure to use intelligent values for the tolerance and the timeout.  The default (AMCONTROL_TOLERANCE_DONT_CARE) will result in moves completing right away, regardless of where they get to.

Because we don't have a great idea of whether this control is actually moving or not, we set the default allowsMovesWhileMoving() to true... To make sure we never omit sending moves we actually should have, because we think we're moving.  Call setAllowsMovesWhileMoving(false) if your device does not support receiving move() commands while it's already in motion.

<b>Most useful members for using this class:</b>

- value()
- name()
- isConnected()
- valueChanged()

- move()
- isMoving() or moveInProgress()

- moveStarted()
- moveSucceeded()
- moveFailed(int)

- completionTimeout()
- setCompletionTimeout()


*/
class AMPVControl : public AMReadOnlyPVControl {

	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique description of this control
  \param readPVname The EPICS channel-access name for the feedback Process Variable
  \param writePVname The EPICS channel-access name for the setpoint Process Variable
  \param tolerance The accuracy required for a move() to count as having reached its setpoint() and emit moveSucceeded().
  \param completionTimeoutSeconds Maximum time allowed for the value() to get within tolerance() of the setpoint() after a move().
  \param parent QObject parent class
  \param stopPVname The EPICS channel-access name for the process variable to write to cancel a move in progress. If empty (default), shouldStop() and canStop() both return false, and calls to stop() will not work.
  */
	virtual ~AMPVControl();
	AMPVControl(const QString& name,
				const QString& readPVname,
				const QString& writePVname,
				const QString& stopPVname = QString(),
				QObject* parent = 0,
				double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
				double completionTimeoutSeconds = 10.0,
				int stopValue = 1,
				const QString &description = "");

	/// Ensures that the PV will monitor limits within the standard field names (HOPR and LOPR or DRVH and DRVL)
	virtual void enableLimitMonitoring();
	/// \name Reimplemented Public Functions:
	//@{
	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool isMoving() const{ return moveInProgress(); }
	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// Implies that we can read from the feedback PV and write to the setpoint PV.
	virtual bool isConnected() const { return readPV_->readReady() && writePV_->writeReady(); }
	/// Indicates that we can currently write to the setpoint PV.
	virtual bool canMove() const { return writePV_->canWrite(); }
	/// This Control class has the theoretical ability to move. Always true.
	virtual bool shouldMove() const { return true; }
	/// This Control class can currently stop() if it can write to the stop PV.
	virtual bool canStop() const { return !noStopPV_ && stopPV_->canWrite(); }
	/// This Control class has the theoretical ability to stop, if a stopPVname has been provided.
	virtual bool shouldStop() const { return !noStopPV_; }

	/// The target value for the setpoint PV
	virtual double setpoint() const { return setpoint_; }
	//@}

	/// \name Additional public functions:
	//@{
	/// The EPICS channel-access name of the writePV
	QString writePVName() const { return writePV_->pvName(); }
	/// The value of the writePV. This will match setpoint() unless someone else (another program or person in the facility) is changing the setpoint.
	double writePVValue() const { return writePV_->lastValue(); }
	/// Access to the writePV.  Using this to change the writePVs value by connecting to its slots is allowed but possible abberant behivour is possibles.
	AMProcessVariable* writePV() const { return writePV_; }
	/// Returns the number of seconds allowed for a move() to reach its target setpoint().
	double completionTimeout() const { return completionTimeout_; }
	/// Switches the writePV to use ca_put_callback() instead of ca_put(), if you want confirmation from the IOC when the put is actually processed, and the IOC can handle queuing instead of caching of PV puts.  The default uses ca_put().  \see AMProcessVariable::enablePutCallback().
	void enableWritePVPutCallback(bool putCallbackEnabled) { writePV_->enablePutCallback(putCallbackEnabled); }
	//@}

public slots:

	/// Start a move to the value setpoint (reimplemented)
	virtual FailureExplanation move(double setpoint);
	/// Start a move to the value setpoint (override implementation)
	virtual FailureExplanation move(const QString &stringSetPoint);

	/// Stop a move in progress (reimplemented)
	virtual bool stop() {
		if(!canStop())
			return false;
		stopPV_->setValue(stopValue_);

		// This move is over:
		moveInProgress_ = false;
		completionTimer_.stop();
		emit moveFailed(AMControl::WasStoppedFailure);

		return true;
	}

	/// set the completion timeout (new public slot)
	void setCompletionTimeout(double seconds) { completionTimeout_ = seconds; }

signals:
	/// Reports that the writePV failed to connect to the EPICS server within the timeout.
	/*! This extra signal is specialized to report on PV channel connection status.  You should be free to ignore it and use the signals defined in AMControl.*/
	void writeConnectionTimeoutOccurred();

	/// Signals changes in writePVValue().
	/*! Normally we expect that only this program is changing the setpoint and causing motion, using move().  If someone else changes the writePV (setpoint PV), this will signal you with the new value.*/
	void setpointChanged(double);

protected:
	/// Used for the setpoint
	AMProcessVariable* writePV_;
	/// Used for stopping
	AMProcessVariable* stopPV_;

	/// Used to detect completion timeouts:
	QTimer completionTimer_;
	/// Used to detect completion timeouts:
	double completionTimeout_;

	/// used internally to track whether we're moving:
	bool moveInProgress_;

	/// the target of our attempted move:
	double setpoint_;

	/// true if no stopPVname was provided... Means we can't stop(), and shouldStop() and canStop() are false.
	bool noStopPV_;
	/// The value written to the stopPV_ when attempting to stop().
	int stopValue_;

protected slots:

	/// (overridden) Handle a connection timeout from either the readPV_ or writePV_
	/*! The units come from the readPV, so if it's out, we don't know what the units are.
  In any case, if either one doesn't connected, we're not connected.*/
	void onConnectionTimeout() { if(sender() == readPV_) { setUnits("?"); } emit connected(false); emit error(AMControl::CannotConnectError); }


	/// This is called when there is a Write PV channel error:
	void onWritePVError(int errorCode);

	/// This is used to handle the timeout of a move
	void onCompletionTimeout();

	/// This is used to check every new value, to see if we entered tolerance
	void onNewFeedbackValue(double val);

	/// This is called when reading the PV's control information completes successfully.
	virtual void onReadPVInitialized();

	/// Called when the writePV is initialized(), calls setMoveEnumStates() if applicable.
	void onWritePVInitialized();
	/// Handles updating the setpoint member when the writePV updates.
	void onSetpointChanged(double newVal);



};

/// This class is a convenience construction class for AMPVControl.
/*!
  There are many controls that use AMPVControl that only talk to a single process variable.  The read and write PVs are the same and they have no stop PV.  Therefore,
  there is a lot of superfluous information in the constructor that can lead to confusion implementation for new user-programmers and is quite unintuitive.  In the end,
  this class behaves \em IDENTICALLY to AMPVControl.  It merely offers a more intuitive constructor.
  */

class AMSinglePVControl : public AMPVControl {

	Q_OBJECT

public:
	/// Constructor.
	/*! \param name A unique description of this control
  \param PVname The EPICS channel-access name for the setpoint AND feedback Process Variable
  \param tolerance The accuracy required for a move() to count as having reached its setpoint() and emit moveSucceeded().
  \param completionTimeoutSeconds Maximum time allowed for the value() to get within tolerance() of the setpoint() after a move().
  \param parent QObject parent class
  */
	virtual ~AMSinglePVControl();
	AMSinglePVControl(const QString& name,
					  const QString& PVname,
					  QObject* parent = 0,
					  double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
					  double completionTimeoutSeconds = 10.0,
					  const QString &description = "");
};

/// Subclass this to create an object that specifies how to interpret a control's status value.  With this mechanism, we can accept arbitrarily complex algorithms to determine if an AMPVwStatusControl's status value means that it's moving.  For example, if your motor driver returns 4, 5, or 6 to mean that it's moving, create a subclass whose operator()() function returns true in these three cases.
class AMAbstractControlStatusChecker {
public:
	virtual ~AMAbstractControlStatusChecker() {}

	/// Returns true if the \c statusValue indicates the control is moving, and false if it's not.
	virtual bool operator()(quint32 statusValue) = 0;
};

/// This function object provides the default "isMoving" check for AMReadOnlyPVwStatusControl.  It will compare the current status value against the value provided in the constructor, and return true if equal.
class AMControlStatusCheckerDefault : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compared to \c isMovingValue, and return true if the status value is equal to isMovingValue.
	virtual ~AMControlStatusCheckerDefault() {}
	AMControlStatusCheckerDefault(quint32 isMovingValue) : isMovingValue_(isMovingValue) {}

	/// Returns true (moving) if the \c statusValue matches isMovingValue_
	virtual bool operator()(quint32 statusValue) { return statusValue == isMovingValue_; }

protected:
	quint32 isMovingValue_;
};

/// This function object provides the opposite of the default check for AMReadyOnlyPVwStatusControl (instead of providing the one value which represents "moving" you provide the one value which represents "stopped"). It will compare the current status against the value provided in the constuctor, and return true if not equal.
class AMControlStatusCheckerStopped : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compare to \c isStoppedValue, and return true if the status value is not equal to isStoppedValue (something that isn't stopped is moving)
	virtual ~AMControlStatusCheckerStopped() {}
	AMControlStatusCheckerStopped(quint32 isStoppedValue) : isStoppedValue_(isStoppedValue) {}

	/// Return true (moving) if the \c statusValue does not matche isStoppedValue_
	virtual bool operator()(quint32 statusValue) { return statusValue != isStoppedValue_; }

protected:
	quint32 isStoppedValue_;
};

/// This class provides an AMControl that can measure a feedback process variable, and monitor a second PV that provides a moving indicator.
/*!
The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using a single Process Variable.  It cannot be used to set values, but it watches another Process Variable to see if the control is moving.  An example usage would be monitoring the "beamline energy" PV that is common on most beamlines.

By default, isMoving() means that the (integer) numeric value of the \c isMovingPV is equal to 1.  If you need more detail than this, define an AMAbstractControlStatusChecker subclass where the bool operator()(int statusValue) returns true for moving and false for stopped... And then pass a new instance of it in the constructor.


<b>The unique behavior is defined as:</b>

- isConnected means:		the feedback process variable and moving-status process variable are connected.
- isMoving() means:		(moving-status PV's current value & isMovingMask) == isMovingValue
- moveSucceeded() means:	[never happens]
- moveFailed() means:		[happens on any attempted move; can't do that!]

<b>Most useful members for using this class:</b>

- value()
- name()
- isConnected()
- isMoving()
- valueChanged()

signals:
- movingChanged()

*/

class AMReadOnlyPVwStatusControl : public AMReadOnlyPVControl {

	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique description of this control
  \param readPVname The EPICS channel-access name for the feedback Process Variable
  \param movingPVname The EPICS channel-access name for the move-monitor Process Variable
  \param statusChecker An instance of an AMAbstractControlStatusChecker.  isMoving() is true when the movingPV's value passed into its operator()() function returns true.  The default status checker compares the movingPV's value to 1.  (Note: this class takes ownership of the statusChecker and deletes it when done.)
  \param parent QObject parent class
  */
	AMReadOnlyPVwStatusControl(const QString& name,
							   const QString& readPVname,
							   const QString& movingPVname,
							   QObject* parent = 0,
							   AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
							   const QString &description = "");

	/// Destructor
	virtual ~AMReadOnlyPVwStatusControl() { delete statusChecker_; }

	/// \name Reimplemented Public Functions:
	//@{
	/// Implies that we can read from both the feedback PV and move-status PV.
	virtual bool isConnected() const { return readPV_->readReady() && movingPV_->readReady(); }

	/// The movingPV now provides our moving status.
	virtual bool isMoving() const { return (*statusChecker_)(movingPV_->getInt()); }

	// Additional public functions:
	/// The EPICS channel-access name of the PV that provides the moving status.
	QString movingPVName() const { return movingPV_->pvName(); }
	/// The current value of the movingPV
	int movingPVValue() const { return movingPV_->getInt(); }
	/// Read-only access to the movingPV
	const AMProcessVariable* movingPV() const { return movingPV_; }
	//@}


signals:
	/// This is specialized to report on the isMovingPV's channel connection status.  You should be free to ignore it and use the signals defined in AMControl.
	void movingConnectionTimeoutOccurred();

protected:
	/// This PV is used to watch the moving status
	AMProcessVariable* movingPV_;

	/// An evaluator object used to determine if the status value indicates moving
	AMAbstractControlStatusChecker* statusChecker_;

	/// This is used to detect changes in the moving/not-moving status
	bool wasMoving_;

protected slots:

	/// override this if you want custom handling if any PV fails to connect.

	/// Since the units come from the read-PV, we need the readPV for that.
	/// All connection timeouts cause us to be not connected.
	void onConnectionTimeout() { if(sender() == readPV_) { setUnits("?"); } emit connected(false); emit error(AMControl::CannotConnectError); }

	/// This is called when there is a Status PV channel error:
	void onStatusPVError(int errorCode);

	/// This is called whenever there is an update from the move status PV
	virtual void onMovingChanged(int isMovingValue);

};


/// This class provides an AMControl that can measure a feedback process variable, move it using a setpoint process variable, and monitor a third PV that provides a "isMoving" indicator.
/*!
The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using one Process Variable (PV), and moves them to setpoints using a second PV.  It also watches third Process Variable to see if the control is moving.  An example usage would be controlling the "beamline energy" PV that is common on most beamlines.  Most importantly, it adds a working move() instruction, and reports shouldMove() = true.

isMoving() has the same meaning as for AMReadOnlyPVwStatusControl.  However, this class adds the moveInProgress() property to indicate of one of \em your moves -- a move triggered by this software -- is executing, as well as the moveStarted, moveSucceeded, and moveFailed(explanation) signals that accompany changes of moveInProgress().

Because we can monitor the moving status, it's possible to detect a situation where a stubborn device didn't respond to even our most polite move() instruction.  The moveStartTimeout() lets you set a time period withing which the device must start isMoving() after a move() instruction.  Failure to do so results in a moveFailed() signal.


The unique behavior is defined as:

- isConnected means:		the feedback process variable, setpoint PV, and moving-status PV are connected.
- isMoving() means:		(moving-status PV's current value & isMovingMask) == isMovingValue
- moveStarted() means:	after a move(), isMoving() became true withine moveStartTimeout()
- moveSucceeded() means:	after a move(), the isMoving() went back to stopped, and the value() was within tolerance() of setpoint()
- moveFailed() means:		after a move(), either
  -isMoving() did not start within moveStartTimeout(), or
  - when isMoving() went back to stopped, the required tolerance was not met.

<b>Most useful members for using this class:</b>

- value()
- name()
- isConnected()
- isMoving()
- valueChanged()

- move()
- moveInProgress()

- signals:
- movingChanged()
- moveStarted()
- moveFailed()
- moveSucceeded()

*/

class AMPVwStatusControl : public AMReadOnlyPVwStatusControl {

	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique representative name for this control
  \param readPVname The EPICS channel-access name for the feedback Process Variable
  \param writePVname The EPICS channel-access name for the setpoint Process Variable
  \param movingPVname The EPICS channel-access name for the move-monitor Process Variable
  \param tolerance The level of accuracy (max. distance between setpoint() and final value()) required for a move() to be successful
  \param moveStartTimeoutSeconds Time allowed after a move() for the Control to first start moving.  If it doesn't, we emit moveFailed(AMControl::TimeoutFailure).
  \param statusChecker An instance of an AMAbstractControlStatusChecker.  isMoving() is true when the movingPV's value passed into its operator()() function returns true.  The default status checker compares the movingPV's value to 1.  (Note: this class takes ownership of the statusChecker and deletes it when done.)
  \param stopPVname is the EPICS channel-access name for the Process Variable used to stop() a move in progress.
  \param stopValue is the value that will be written to the stopPV when stop() is called.
  \param parent QObject parent class
  */
	virtual ~AMPVwStatusControl();
	AMPVwStatusControl(const QString& name,
					   const QString& readPVname,
					   const QString& writePVname,
					   const QString& movingPVname,
					   const QString& stopPVname = QString(),
					   QObject* parent = 0,
					   double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
					   double moveStartTimeoutSeconds = 2.0,
					   AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
					   int stopValue = 1,
					   const QString &description = "");

	/// Ensures that the PV will monitor limits within the standard field names (HOPR and LOPR or DRVH and DRVL)
	virtual void enableLimitMonitoring();
	/// \name Reimplemented Public Functions:
	//@{
	/// Indicates that all three process variables are ready for action:
	virtual bool isConnected() const { return readPV_->readReady() && writePV_->writeReady() && movingPV_->readReady(); }
	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool moveInProgress() const { return moveInProgress_ || startInProgress_; }	// moveInProgress_ will be true as soon as move() is requested.  moveInProgress() isn't happening until the device starts moving as well.)
	/// Indicates that this control currently can cause moves:
	virtual bool canMove() const { return writePV_->canWrite(); }
	/// Theoretically, if we're connected, this control type should be able to move:
	virtual bool shouldMove() const { return true; }
	/// This Control class can currently stop() if it can write to the stop PV.
	virtual bool canStop() const { return !noStopPV_ && stopPV_->canWrite(); }
	/// This Control class has the theoretical ability to stop, if a stopPVname has been provided.
	virtual bool shouldStop() const { return !noStopPV_; }

	/// isMoving: according to the moving PV, and also while we're in the settling phase.
	virtual bool isMoving() const { return (*statusChecker_)(movingPV_->getInt()) || settlingInProgress_; }

	/// This is the target of the last requested move:
	virtual double setpoint() const { return setpoint_; }
	//@}

	/// \name Additional public functions:
	//@{
	/// The EPICS channel-access name of the setpoint PV
	QString writePVName() const { return writePV_->pvName(); }
	/// The value of the writePV. This will match setpoint() unless someone else (another program or person in the facility) is changing the setpoint.
	virtual double writePVValue() const { return writePV_->lastValue(); }
	/// Access to the writePV.  Using this to change the writePVs value by connecting to its slots is allowed but abberant behaviour could be the result.
	AMProcessVariable* writePV() const { return writePV_; }
	/// The maximum time allowed for the Control to start isMoving() after a move() is issued.
	double moveStartTimeout() { return moveStartTimeout_; }
	/// Switches the writePV to use ca_put_callback() instead of ca_put(), if you want confirmation from the IOC when the put is actually processed, and the IOC can handle queuing instead of caching of PV puts. The default uses ca_put().  \see AMProcessVariable::enablePutCallback().
	void enableWritePVPutCallback(bool putCallbackEnabled) { writePV_->enablePutCallback(putCallbackEnabled); }

	/// The settling time that is allowed after the hardware reports "move done", in seconds, before checking the feedback and tolerance.
	/*! EPICS channel access provides no guarantee on the order in which channel access monitors from different PVs are received. Therefore, it's highly likely that the status PV may receive the "Move Done" notification before the latest feedback value() is received from the read PV.  This can cause two problems: (1) depending on the tolerance and the monitoring rate, a physically-successful move may incorrectly report a tolerance failure, because the within-tolerance feedback value has not yet been received by the end of a move, and (2) the feedback value() read immediately after a move finishes may not actually be accurate. [In both cases, the true feedback value will probably be arriving a few ms later].

	  If a settling time is specified, the control will wait for that many seconds before checking the tolerance and reporting either moveFailed() or moveSucceeded(), as well as changing isMoving() and moveInProgress().

	  The default settling time is currently 0 [disabled].
	  */
	double settlingTime() const { return settlingTime_; }
	//@}

public slots:

	/// Start a move to the value setpoint (reimplemented)
	virtual FailureExplanation move(double setpoint);

	/// Tell the motor to stop.  (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
	virtual bool stop();

	/// set the completion timeout:
	void setMoveStartTimeout(double seconds) { moveStartTimeout_ = seconds; }

	/// Set the settling time that is allowed after the hardware reports "move done", in seconds. \see settlingTime().
	void setSettlingTime(double seconds);


signals:
	/// These are specialized to report on the writePV's channel connection status.  You should be free to ignore them and use the signals defined in AMControl.
	void writeConnectionTimeoutOccurred();
	/// Signals changes in writePVValue().
	/*! Normally we expect that only this program is changing the setpoint and causing motion, using move().  If someone else changes the writePV (setpoint PV), this will signal you with the new value.*/
	void setpointChanged(double);
	/// Notifier that the settling time has changed.
	void settlingTimeChanged(double seconds);

protected:
	/// This PV is used for the setpoint:
	AMProcessVariable* writePV_;
	/// Used for stopping
	AMProcessVariable* stopPV_;

	/// Used to detect moveStart timeouts:
	QTimer moveStartTimer_;
	/// Used to detect moveStart timeouts: timeout in seconds
	double moveStartTimeout_;

	/// used internally to track whether we're waiting for a physical control to actually start moving, after we've told it to.
	bool startInProgress_;
	/// used internally to track whether we're moving [ie: one of OUR moves is in progress]
	bool moveInProgress_;
	/// Used internally to indicate that we've issued a stop() command. (Stop in-progress)
	bool stopInProgress_;

	/// the target of our attempted move:
	double setpoint_;

	/// true if no stopPVname was provided... Means we can't stop(), and shouldStop() and canStop() are false.
	bool noStopPV_;
	/// The value written to the stopPV_ when attempting to stop().
	int stopValue_;

	/// If non-zero, the control waits this many seconds after receiving MOVE DONE notification from the status PV before checking tolerance and reporting the end of a move.
	double settlingTime_;
	/// Used to track that we are in the settling phase.  \see setSettlingTime().
	bool settlingInProgress_;
	/// Used to catch the end of settling
	QTimer settlingTimer_;

	/// Used for change detection of the hardware's moving state.  AMPVwStatus::wasMoving_ is used for change detection of our public isMoving() state, which includes settlingInProgress_.  Instead, this is the last raw moving state of the harware.
	bool hardwareWasMoving_;

protected slots:

	/// This is called when there is a Status PV channel error:
	void onWritePVError(int errorCode);

	/// Re-implemented: This is used to handle when the movingPV_ changes.
	virtual void onMovingChanged(int isMovingValue);

	/// This is used to handle the timeout of a move start:
	void onMoveStartTimeout();

	/// Called when the settling time expires
	void onSettlingTimeFinished();
	virtual void onReadPVInitialized();
	/// Called when the writePV is initialized(). Calls setMoveEnumStates() if applicable.
	void onWritePVInitialized();
	/// Handles updating the setpoint member when the writePV updates.
	void onSetpointChanged(double newVal);

};


/// Subclass this to create an object to define unit conversions from and to "raw" units. Instances of this are used by the AMPVwStatusAndUnitConversionControl.  A simple implementation that might do what you need is AMScaleAndOffsetUnitConverter.
class AMAbstractUnitConverter {
public:
	virtual ~AMAbstractUnitConverter() {}

	/// Returns the name of the output (non-raw) units
	virtual QString units() const = 0;
	/// Converts raw units to output units
	virtual double convertFromRaw(double rawValue) const = 0;
	/// Convert output units back to raw units
	virtual double convertToRaw(double value) const = 0;
};

/// This version of AMAbstractUnitConverter provides scaling and offset conversion, so that convertFromRaw(x) returns x*scale()+offset().
class AMScaleAndOffsetUnitConverter : public AMAbstractUnitConverter {
public:
	/// Constructor
	virtual ~AMScaleAndOffsetUnitConverter() {}
	AMScaleAndOffsetUnitConverter(const QString& units, double scale = 1.0, double offset = 0.0) :
		units_(units), scale_(scale), offset_(offset) {}
	/// Convert raw units to output units
	double convertFromRaw(double rawValue) const { return rawValue*scale_ + offset_; }
	/// Conver output units back to raw units
	double convertToRaw(double value) const { return (value-offset_)/scale_; }

	/// Returns the scaling factor
	double scale() const { return scale_; }
	/// Returns the offset
	double offset() const { return offset_; }
	/// Returns the name of the output units
	QString units() const { return units_; }

protected:
	QString units_;
	double scale_, offset_;
};

/// This class works exactly like AMPVwStatusControl, except that it offers a user-definable conversion from the raw process variable units to other units, using AMAbstractUnitConverter to specify the conversion.
class AMPVwStatusAndUnitConversionControl : public AMPVwStatusControl {

	Q_OBJECT
public:
	/// Constructor
	/*! \param name A unique representative name for this control
  \param readPVname The EPICS channel-access name for the feedback Process Variable
  \param writePVname The EPICS channel-access name for the setpoint Process Variable
  \param movingPVname The EPICS channel-access name for the move-monitor Process Variable
  \param tolerance The level of accuracy (max. distance between setpoint() and final value()) required for a move() to be successful. In this case, it should be specified in converted units (not raw units).
  \param moveStartTimeoutSeconds Time allowed after a move() for the Control to first start moving.  If it doesn't, we emit moveFailed(AMControl::TimeoutFailure).
  \param statusChecker An instance of an AMAbstractControlStatusChecker.  isMoving() is true when the movingPV's value passed into its operator()() function returns true.  The default status checker compares the movingPV's value to 1.  (Note: this class takes ownership of the statusChecker and deletes it when done.)
  \param stopPVname is the EPICS channel-access name for the Process Variable used to stop() a move in progress.
  \param stopValue is the value that will be written to the stopPV when stop() is called.
  \param parent QObject parent class
  \param description A human-readable description of what this control controls
  \param readUnitConverter An instance of an AMAbstractUnitConverter used by value() to convert from the readPV raw value to our desired units
  \param writeUnitConverter An instance of an AMAbstractUnitConverter used by move() and setpoint() to convert from our desired units to the raw writePV value. Note that it's important for \c readUnitConverter and \c writeUnitConverter to convert into the same consistent units so that value() and setpoint() can be compared together appropriately.  In many cases, if the underlying PVs have the same raw units, then the same converter can be used for both, and \c writeUnitConverter can be set as 0.
  */
	AMPVwStatusAndUnitConversionControl(const QString& name,
										const QString& readPVname,
										const QString& writePVname,
										const QString& movingPVname,
										const QString& stopPVname = QString(),
										AMAbstractUnitConverter* readUnitConverter = new AMScaleAndOffsetUnitConverter("myUnits"),
										AMAbstractUnitConverter* writeUnitConverter = 0,
										QObject* parent = 0,
										double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
										double moveStartTimeoutSeconds = 2.0,
										AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
										int stopValue = 1,
										const QString &description = "");

	/// Destructor: deletes the unit converter
	virtual ~AMPVwStatusAndUnitConversionControl() { delete readConverter_; delete writeConverter_; }
	/// Enables monitoring of the PVs limit fields
	void enableLimitMonitoring();
	/// Set the unit converters. This class takes ownership of the new converters and deletes the old ones. \c readUnitConverter must be a pointer to a valid object, writeUnitConverter can be 0 if the same conversion is appropriate for both the readPV and writePV.
	void setUnitConverters(AMAbstractUnitConverter* readUnitConverter, AMAbstractUnitConverter* writeUnitConverter = 0);
	/// Returns the unit converter currently in-use for the read (feedback) values
	AMAbstractUnitConverter* readUnitConverter() const { return readConverter_; }
	/// Returns the unit converter currently in-use for the write (setpoint, move) values
	AMAbstractUnitConverter* writeUnitConverter() const { return writeConverter_ ? writeConverter_ : readConverter_; }

	/// For units(), we return the units given by our unit converter. But you can still access the underlying "raw" units if you want:
	QString rawUnits() const { return AMPVwStatusControl::units(); }

	/// We overload value() to convert to our desired units
	virtual double value() const { return readUnitConverter()->convertFromRaw(AMPVwStatusControl::value()); }

	/// We overload move() to convert our units back to raw units for the writePV
	virtual FailureExplanation move(double setpoint) { return AMPVwStatusControl::move(writeUnitConverter()->convertToRaw(setpoint)); }

	/// We overload setpoint() to convert the units
	virtual double setpoint() const { return writeUnitConverter()->convertFromRaw(AMPVwStatusControl::setpoint()); }

	/// Overloaded to convert the units. The min and max values come from the specification in the writePV.
	virtual double minimumValue() const { return writeUnitConverter()->convertFromRaw(AMPVwStatusControl::minimumValue()); }
	/// Overloaded to convert the units The min and max values come from the specification in the writePV.
	virtual double maximumValue() const { return writeUnitConverter()->convertFromRaw(AMPVwStatusControl::maximumValue()); }
	/// Overloaded to convert the units.
	virtual double writePVValue() const { return writeUnitConverter()->convertFromRaw(AMPVwStatusControl::writePVValue()); }


protected slots:
	/// Instead of forwarding the readPV valueChanged() signal directly as valueChanged(), we need to do a conversion
	void onReadPVValueChanged(double newValue);
	/// Instead of forwarding the writePV valueChanged() signal directly as setpointChanged(), we need to do a conversion
	void onWritePVValueChanged(double newValue);
	/// Handles the low limit PV changing. Converts the limit before forwarding the signal on.
	virtual void setLowLimitValue(double newLowLimit);
	/// Handles the high limit PV changing. Converts the limit before forwarding the signal on.
	virtual void setHighLimitValue(double newHighLimit);

protected:

	AMAbstractUnitConverter* readConverter_, *writeConverter_;
};


/// This class provides an AMControl with measure-only capability, based on an Epics Process Variable Waveform implementation.
/*!
The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using a single Process Variable Waveform. This subclass can bin a waveform's values into a single value.  It cannot be used to set values.  An example usage would be measuring a spectral detector. The waveform represents each bin individually, but the ability to bin regions of interest is useful.  There is no way to decide whether the control is "moving" or not (How do you define if the ring current is "moving"?), so this behavior is undefined.

The unique behavior is defined as:

<table>
<tr><td>isConnected means:		<td>the feedback process variable is connected.
<tr><td>isMoving() means:		<td>[always false; no good way to determine this]
<tr><td>moveSucceeded() means:	<td>[never happens]
<tr><td>moveFailed() means:		<td>[happens on any attempted move; can't do that!]
</table>

Most useful members for using this class:

- value()
- name()
- setBinParameters(int, int)
- isConnected()
- valueChanged()

*/
class AMReadOnlyWaveformBinningPVControl : public AMReadOnlyPVControl {

	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique description of this control
  \param readPVname The EPICS channel-access name for this Process Variable
  \param parent QObject parent class
  */
	virtual ~AMReadOnlyWaveformBinningPVControl();
	AMReadOnlyWaveformBinningPVControl(const QString& name,
									   const QString& readPVname,
									   int lowIndex = 0,
									   int highIndex = 1,
									   QObject* parent = 0,
									   const QString &description = "");

	/// Does nothing, waveform records do not have limits.
	virtual void enableLimitMonitoring() {}
	/// \name Reimplemented Public Functions:
	//@{
	/// most recent value of this measurement
	virtual double value() const;

	//@}

	/// \name Additional public functions:
	//@{
	/// Set the parameters for binning. A low index and a high index.
	virtual void setBinParameters(int lowIndex, int highIndex);
	//@}

public slots:
	void setAttemptDouble(bool attemptDouble);

protected slots:
	/// This is called when the read PV has new values
	void onReadPVValueChanged();

protected:
	int lowIndex_;
	int highIndex_;

	bool attemptDouble_;
};

/// This class provides an AMControl, based on an Epics Process Variable Waveform implementation.
class AMWaveformBinningSinglePVControl : public AMSinglePVControl {

	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique description of this control
  \param readPVname The EPICS channel-access name for this Process Variable
  \param parent QObject parent class
  */
	AMWaveformBinningSinglePVControl(const QString& name,
									   const QString& PVName,
									   int lowIndex = 0,
									   int highIndex = 1,
									   QObject* parent = 0,
									   const QString &description = "");

	/// Does nothing, waveform records do not have limits.
	virtual void enableLimitMonitoring() {}
	/// \name Reimplemented Public Functions:
	//@{
	/// most recent value of this measurement
	virtual double value() const;
	virtual bool values(int size, int *outputValues) const;
	virtual bool values(int size, double *outputValues) const;

	//@}

	/// \name Additional public functions:
	//@{
	/// Set the parameters for binning. A low index and a high index.
	virtual void setBinParameters(int lowIndex, int highIndex);
	//@}

public slots:
	void setAttemptDouble(bool attemptDouble);

	void setValues(const QVector<int> &values);
	void setValues(const QVector<double> &values);

protected slots:
	/// This is called when the read PV has new values
	void onReadPVValueChanged();

protected:
	int lowIndex_;
	int highIndex_;

	bool attemptDouble_;
};
// End of doxygen group: control
/**
 @}
 */

#endif // AMPVCONTROL_H
