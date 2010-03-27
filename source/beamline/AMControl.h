#ifndef ACQMAN_CONTROL_H
#define ACQMAN_CONTROL_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
#include "ProcessVariable.h"
#include <QDebug>

#include <cmath>

/// A "AMControl" is an abstract representation of all basic scientific quantities that can be adjusted (controlled) or measured.
/*!
  A "AMControl" is an abstract representation of all basic scientific quantities that can be adjusted (controlled) or measured.  Fundamentally, a Control can be "measured" and/or "moved".  All controls have the following abilities:

  actions:
  ========

  void move(double setpoint): set a Control to a requested target value

  properties:
  ==========

  double value(): the readout/value/position at this instant in time

  QString name(): A descriptive (and hopefully unique) name for this Control

  bool isConnected(): indicates that the control is hooked up and operating correctly; ready for use.
  signal: connectedChanged(bool isConnected): emitted for changes in isConnected()

  bool shouldMeasure(): this control should be capable of reporting it's current value, assuming that it's connected.
  bool shouldMove(): this control should be capable of being adjusted, assuming that it's connected.
  bool canMeasure(), canMove(): indicate that, at this instant in time, the control is capable of being measured or moved

  QPair<double, double> range:
  double minimumValue():
  double maximumValue(): the limits of this control's range

  double setpoint(): For Controls that can be moved, this is the most recently requested target (desired value)
  double tolerance(): This "deadband" or "epsilon" represents how close a Control must get to it's target value for it to count as inPosition (or for the move to have succeeded).  It represents the level of accuracy the scientist requires for this quantity; ie: the maximum acceptable error between measure() and setpoint().  The default value is CONTROL_TOLERANCE_DONT_CARE -- a very large number.  Use setTolerance() to change it.
  bool inPosition(): indicates the Control is within tolerance() of it's requested target.

  There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems like in a synchrotron.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

One concept of moving simply reflects what you would see in the physical world: is the value of the Control currently changing? (either triggered by the user, or by external inputs)
In other cases, what you really want to know is whether the Control started or stopped moving \emph{in response to your instructions}.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

The Control abstraction provides two different properties (and associated signals) for these two concepts. The first one simply indicates motion, regardless of the cause:

  bool isMoving(): indicates that the Control's measurement is transient / on its way toward a steady-state value.
  signal: movingChanged(bool isMoving): provides notification of changes in isMoving()
  signal: valueChanged(double newValue): emits a signal whenever a new sample (measured value) is obtained.

The second category only reflects motion due to instructions sent by \emph{this} software:

  bool moveInProgress(): A move that you requested is currently happening
  signal: moveStarted(): emitted when moveInProgress first goes to true
  signal: moveSucceeded(): emitted when a move completes successfully, and the final measure()d value is within tolerance of the setpoint.
  signal: moveFailed(int explanation): emitted when a move is not able to complete for some reason. Possible explanation codes are:
	- 1: Could not start the move because the control's not connected / responding
	- 2: The move finished, but failed to achieve the tolerance requested
	- 3 and higher: an error code defined by the specific control implementation
	-

One additional feature of Controls is the ability to logically group sets of sub-controls together.  (For example, a Monochromator control could consist of a Grating angle control, exit slit position control, and grating selector.)
Every Control therefore has a list of subcontrols:

actions:
========
void addChild(AMControl* newChild): Insert a subcontrol into this group

properties:
==========
QList<AMControl*> children(): returns the subcontrols grouped under this control.
int numChildren(): number of subcontrols in the group
AMControl* child(int index): return a single subcontrol

One powerful ability of the control grouping is being able to generate a list of the current state of a tree of subcontrols, and then restoring the controls to the same state later:
QMap<QString, double> currentState() const; TODOTODOTODO implement this
bool setState(QMap<QString, double> namesAndValues);

These properties are defined as the general characteristics that all Controls are expected to have, regardless of the actual hardware or connection method. (For example, they should be equally useful for representing Process Variables in an Epics-based control system, devices connected directly via TCP-IP or serial lines, or off-site hoofadinkuses interfaced using smoke signals or bongo drums.)

The Control interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections:

Class Name					Description
===============================================================================
AMControl					Interface definition. Can be instantiated, but not particularly useful.
AMControlGroup				Empty control that doesn't move or measure, but reports the combined moving status and connected status of its subcontrols
AMReadOnlyPVControl			Measures a single process variable.  Cannot be used to set values.
AMPVControl					Measures a feedback process variable, and moves a setpoint PV.
AMReadOnlyPVwStatusControl	Measures a single process variable, and monitors a second PV for moving status
AMPVwStatusControl			Measures a feedback PV, moves a setpoint PV, and monitors a status PV

Examples of where to use these:

Class Name					Usage example
===============================================================================
AMControl
AMControlGroup				A "Beamline" group that shows whether anything on the beamline is moving or disconnected
AMReadOnlyPVControl			Observing the storage ring current ("isMoving()" is hard to define here, so its meaningless)
AMPVControl					High-voltage bias settings, with a setpoint and feedback value
AMReadOnlyPVwStatusControl	Another beamline's energy motor feedback, with moving/not-moving indicators
AMPVwStatusControl			Your beamline's energy motor setpoint and feedback


We need to carefully specify the behavior for all of these:

Class Name					isConnected() means:
===============================================================================
AMControl					[is always false]
AMControlGroup				all children are connected
AMReadOnlyPVControl			feedback PV is connected
AMPVControl					feedback and setpoint PVs are connected
AMReadOnlyPVwStatusControl	feedback and motion-status PVs are connected
AMPVwStatusControl			feedback, setpoint, and motion-status PVs are all connected

Class Name					moving() means:
===============================================================================
AMControl					[is always false]
AMControlGroup				any child is moving
AMReadOnlyPVControl			[is always false: no good way to determine this]
AMPVControl					one of your moves is executing (moveInProgress = true)
AMReadOnlyPVwStatusControl	motion-status PV indicates that the control is moving
AMPVwStatusControl			motion-status PV indicates that the control is moving

Class Name					moveSucceeded() means:
===============================================================================
AMControl					[never happens]
AMControlGroup				[never happens]
AMReadOnlyPVControl			[never happens]
AMPVControl					after starting a move, the measure()d value came within tolerance() of setpoint() before completionTimeout()
AMReadOnlyPVwStatusControl	[never happens]
AMPVwStatusControl			motion-status PV indicates that the control stopped moving, and the value was not within tolerance of setpoint()

Class Name					moveFailed() means:
===============================================================================
AMControl					[any move]
AMControlGroup				[any move]
AMReadOnlyPVControl			[any move]
AMPVControl					after starting a move, the completionTimeout() occured before measure()d value came within tolerance() of setpoint()
AMReadOnlyPVwStatusControl	[any move]
AMPVwStatusControl			motion-status PV indicated that the control stopped moving, and the value was not within tolerance() of setpoint.  OR... The PV did not start isMoving() within startTimeout().







  */

#include <float.h>
#define AMCONTROL_TOLERANCE_DONT_CARE DBL_MAX

// Every Control can have it's own value, as well as act as a parent for other controls.
// This interface class should be re-implemented for anything useful; it's not really helpful on its own.
class AMControl : public QObject {

	Q_OBJECT
public:

	/// This enum type is used to describe the reason for a move failure:
	enum FailureExplanation { NotConnectedFailure = 1, ToleranceFailure, OtherFailure };

	AMControl(const QString& name, const QString& units = "n/a", double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, QObject* parent = 0) : QObject(parent), units_(units) {
		setObjectName(name);
		wasConnected_ = false;
		tolerance_ = tolerance;
	}

	/// One additional feature of Controls is the ability to logically group sets of sub-controls together.  (For example, a Monochromator control could consist of a Grating angle control, exit slit position control, and grating selector.)  Every Control therefore has a list of subcontrols:
	QList<AMControl*> children() const { return children_; }
	/// The number of subcontrols in this group:
	int numChildren() const { return children_.count(); }
	/// Retreive a specific child control from the list by index
	AMControl* child(size_t index) const{
		if(index < (size_t)children_.count())
			return children_.at(index);
		return NULL;
	}
	/// Add a subcontrol to the control group. Subclasses can reimplement this if they need to connect to children's signals, etc.
	virtual void addChild(AMControl* control) { children_ << control; }

	/// A descriptive and hopefully-unique name for this control:
	QString name() const { return objectName(); }

	/// This value defines how close the final measure()d position must be to the setpoint(), for the move to have succeeded.
	double tolerance() const { return tolerance_; }

	/// This represents the current value/position of the control. Must reimplement for actual controls.
	virtual double value() const { return -1; }

	/// this is the "setpoint": the last place the control was told to go:
	virtual double setpoint() const { return -1; }

	/// this indicates whether a control is "in position" (ie: its value is within tolerance of the setpoint)
	virtual bool inPosition() const { return fabs(value()-setpoint()) < tolerance(); }

	/// this indicates a "fully-functional" control, ready for action
	virtual bool isConnected() const { return false; }

	/// These indicate the current abilities / connection statuses of this control. Unconnected controls can't do anything.

	/// Indicates that this control can currently take readings:
	virtual bool canMeasure() const { return false; }
	/// Indicates that this control type _should_ (assuming it's connected) be able to measure values:
	virtual bool shouldMeasure() const { return false; }
	/// Indicates thatthis control can (currently) move to setpoints:
	virtual bool canMove() const { return false; }
	/// Indicates that this control _should_ (assuming it's connected) be able to move to setpoints:
	virtual bool shouldMove() const { return false; }

	/// Indicates that the Control's measurement is transient / on its way toward a steady-state value
	/*! There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems like in a synchrotron.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

  One concept of moving simply reflects what you would see in the physical world: is the value of the Control currently changing? (either triggered by the user, or by external inputs)
  In other cases, what you really want to know is whether the Control started or stopped moving \emph{in response to your instructions}.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

  The Control abstraction provides two different properties (and associated signals) for these two concepts. The first one simply indicates motion, regardless of the cause:

	bool isMoving(): indicates that the Control's measurement is transient / on its way toward a steady-state value.
	signal: movingChanged(bool isMoving): provides notification of changes in isMoving()
	signal: valueChanged(double newValue): emits a signal whenever a new sample (measured value) is obtained.
	  */
	virtual bool isMoving() const { return false; }
	/*virtual bool isMoving() {
		bool isMoving = false;
		foreach(AMControl* c, children_) {
			isMoving |= c->isMoving();
		}
		return isMoving;
	}*/

	/// Indicates that a move() sent by this software is currently happening.
	/*! There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems like in a synchrotron.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

	Sometimes, what you really want to know is whether the Control started or stopped moving \emph{in response to your instructions}.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

The Control abstraction provides two different properties (and associated signals) for these two concepts. The second category only reflects motion due to instructions sent by \emph{this} software:

  bool moveInProgress(): A move that you requested is currently happening
  signal: moveStarted(): emitted when moveInProgress first goes to true
  signal: moveSucceeded(): emitted when a move completes successfully, and the final measure()d value is within tolerance of the setpoint.
  signal: moveFailed(int explanation): emitted when a move is not able to complete for some reason. Possible explanation codes are:
	AMControl::NotConnectedFailure: Could not start the move because the control's not connected or responding
	AMControl::ToleranceFailure: The move finished, but failed to achieve the tolerance requested
	AMControl::OtherFailure and higher: an error code defined by the specific control implementation
*/
	virtual bool moveInProgress() const { return false; }

	/// Indicates the units of the value of this control.
	virtual QString units() const { return units_; }

	/// These are the minimum and maximum allowed values for this control
	virtual QPair<double,double> range() const { return QPair<double,double>(minimumValue(), maximumValue()); }
	virtual double minimumValue() const { return -1; }
	virtual double maximumValue() const { return -1; }

public slots:
	/// This is used to set the control.  Must reimplement for actual controls
	virtual void move(double setpoint) {
		Q_UNUSED(setpoint);
		emit moveFailed(AMControl::NotConnectedFailure);  // The default implementation cannot move.
	}

	/// This is used to set the tolerance level:
	void setTolerance(double newTolerance) { tolerance_ = newTolerance; }

	/// This is used to cancel a move. Must reimplement for actual controls
	virtual void stop() { emit moveFailed(AMControl::NotConnectedFailure); }

	/// Move all of the AMControl's children (and grandchildren, etc) based on a QMap of Control Names and setpoint values
	bool setState(const QMap<QString, double> controlList, unsigned int errorLevel = 0);

signals:
	/// Announce changes of a moveInProgress(). These only apply to moves started by a local move() command... ie: they don't occur when another system causes the control to change its value.
	/// Emitted when moveInProgress() goes to true.
	void moveStarted();
	/// Emitted when a move successfully completes. (This is up to the implementation to define what counts as successful and compleed.)
	void moveSucceeded();
	/// Emitted when a move is not able to complete successfully for some reason.
	/*! Possible explanation codes are:
	AMControl::NotConnectedFailure: Could not start the move because the control's not connected or responding
	AMControl::ToleranceFailure: The move finished, but failed to achieve the tolerance requested
	AMControl::OtherFailure and higher: an error code defined by the specific control implementation
	*/
	void moveFailed(int explanation);

	/// Announce changes in "isMoving()". These happen whenever the control value is changing... whether we move()d it or not.
	void movingChanged(bool isMoving);

	/// Announces when the position/value of the control "value()" has changed.
	void valueChanged(double newValue);
	/// Announces when the unit string text has changed. Necessary because we might not know this until after a delayed control connects.
	void unitsChanged(const QString& units);

	/// Announces changes in isConnected().  Emitted (true) when full functionality is achieved. // Emitted (false) when full functionality is lost. Full functionality can be decided by the subclasses.
	void connectedChanged(bool);


protected:
	QList<AMControl*> children_;
	bool wasConnected_;


protected slots:
	/// This is used internally to set the unit text:
	virtual void setUnits(const QString& units) { units_ = units; emit unitsChanged(units_); }

	/// Used internally by setStateList, called recursively. MIGHT NEED TO BE VIRTUAL for reimplementation in child classes
	bool searchSetChildren(QMap<QString, double> *controlList, QMap<QString, AMControl*> *executeList, unsigned int errorLevel);

private: // subclasses should use the protected methods to access these, to ensure signal generation.
	bool tolerance_;
	QString units_;

};

/// This class provides an AMControl with measure-only capability, based on an Epics Process Variable implementation.
/*!
The Control interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using a single Process Variable.  It cannot be used to set values.  An example usage would be measuring the storage ring current.  There is no way to decide whether the control is "moving" or not (How do you define if the ring current is "moving"?), so this behavior is undefined.

The unique behavior is defined as:

isConnected means:		the feedback process variable is connected.
isMoving() means:		[always false; no good way to determine this]
moveSucceeded() means:	[never happens]
moveFailed() means:		[happens on any attempted move; can't do that!]

Most useful members for using this class:

value()
name()
isConnected()
valueChanged()

*/
class AMReadOnlyPVControl : public AMControl {

	Q_OBJECT

public:
	AMReadOnlyPVControl(const QString& name, const QString& readPVname, double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, QObject* parent = 0);

	/// Reimplemented Public Functions:
	virtual double value() const { return readPV_->lastValue(); }
	// reuse for wStatus: virtual bool isMoving() { if(movingPV_) return movingPV_->lastValue(); else return Control::isMoving(); }
	virtual bool isConnected() const { return canMeasure(); }
	virtual bool canMeasure() const { return readPV_->canRead(); }
	virtual bool shouldMeasure() const { return true; }

	virtual double minimumValue() const { return readPV_->lowerAlarmValue(); }
	virtual double maximumValue() const { return readPV_->upperAlarmValue(); }


	// Additional public functions:
	QString readPVName() const { return readPV_->pvName(); }
	// reuse for wStatus:QString movingPVName() { return (movingPV_) ? movingPV_->pvName() : QString(""); }



signals:	// These are specialized to report on PV channel connection status.  You should be free to ignore them and use the interface defined in Control::.
	void readConnectionTimeoutOccurred();

protected:
	DoubleProcessVariable* readPV_;
	// reuse for wStatus: IntProcessVariable* movingPV_;

	// This is used to detect changes in the moving/not-moving status
	// reuse for wStatus: bool wasMoving_;

protected slots:
	// This is called when the read from the unit string PV completes successfully.
	void onReadPVInitialized();

	// override this if you want custom handling if the readPV fails to connect.
	// You can also monitor the readConnectionTimeoutOccurred() signal.
	void onConnectionTimeout() { setUnits("?"); emit connected(false); }

	// This is called when a PV channel connects or disconnects
	void onPVConnected(bool connected);
	// This is called when there is a PV channel error:
	void onPVError(int error);

	// This is called whenever there is an update from the move status PV
	// reuse for wStatus: virtual void onMovingChanged(int isMoving);

};

/// This class provides an AMControl with measure and move capability, based on an Epics Process Variable implementation.
/*!
The Control interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections.

This class measures values using one Process Variable, and writes out a setpoint using another.  An example usage would be setting and monitoring a high-voltage bias power supply.  There is no explicit way to decide whether the control is "moving" or not, so we define it like this:


isConnected means:		the feedback process variable AND the setpoint process variable are connected.
isMoving() means:		a move() has been issued, the completionTimeout() hasn't occurred, and the value hasn't entered tolerance. same as moveInProgress()
moveSucceeded() means:	after a move() has been issued, the value entered tolerance before the completionTimeout() occurred
moveFailed() means:		after starting a move, the completionTimeout() occurred before the value reached tolerance

For these to make sense, make sure to use intelligent values for the tolerance and the timeout.  The default (AMCONTROL_TOLERANCE_DONT_CARE) will result in moves completing right away, regardless of where they get to.

Most useful members for using this class:

value()
name()
isConnected()
valueChanged()

move()
isMoving() or moveInProgress()

moveStarted()
moveSucceeded()
moveFailed(int)


*/
class AMPVControl : public AMReadOnlyPVControl {

	Q_OBJECT

public:
	AMPVControl(const QString& name, const QString& readPVname, const QString& writePVname, double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, double completionTimeoutSeconds = 10.0, QObject* parent = 0);

	/// Reimplemented Public Functions:

	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool isMoving() const{ return moveInProgress(); }
	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool moveInProgress() const { return mip_; }

	virtual bool isConnected() const { return canMeasure() && canMove(); }
	virtual bool canMove() const { return writePV_->canWrite(); }
	virtual bool shouldMove() const { return true; }

	virtual double minimumValue() const { return writePV_->lowerControlLimit(); }
	virtual double maximumValue() const { return writePV_->upperControlLimit(); }

	virtual double setpoint() const { return setpoint_; }


	// Additional public functions:
	QString writePVName() const { return writePV_->pvName(); }

public slots:
	/// Reimplemented public slots:

	/// Start a move to the value setpoint:
	virtual void move(double setpoint);

	/// Stop a move in progress:
	virtual void stop() {}	// TODO TODO TODO




signals:
	/// These are specialized to report on PV channel connection status.  You should be free to ignore them and use the interface defined in Control::.
	void writeConnectionTimeoutOccurred();

protected:
	/// Used for the setpoint:
	DoubleProcessVariable* writePV_;

	/// Used to detect completion timeouts:
	QTimer completionTimer_;
	double completionTimeout_;

	/// used internally to track whether we're moving:
	bool mip_;

	/// the target of our attempted move:
	double setpoint_;

	/// used for change-detection of the connection state:
	bool wasConnected_;

protected slots:

	/// (overridden) Handle a connection timeout from either the readPV_ or writePV_
	// The units come from the readPV, so if it's out, we don't know.
	// In any case, if either one doesn't connected, we're not connected.
	void onConnectionTimeout() { if(sender() == readPV_) { setUnits("?"); } emit connected(false); }

	/// This is called when a PV channel connects or disconnects
	void onPVConnected(bool connected);

	/// This is called when there is a PV channel error:
	void onPVError(int error);

	/// This is used to handle the timeout of a move: [done]
	void onCompletionTimeout();

	/// This is used to check every new value, to see if we entered tolerance: [done]
	void onNewFeedbackValue(double val);


};


// This Control subclass uses one PV to write to a setpoint, and another PV to read the current value.
/* Most useful new functions:
		move()
		stop()
   and signals:
		moveStarted()
		moveEnded()
		moveFailed()
*/
/*
class PVControl : public ReadOnlyPVControl {

	Q_OBJECT

public:
	PVControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname = "", double tolerance = 0.01, QObject* parent = 0);

	// Reimplemented public functions:
	virtual double tolerance() { return tolerance_; }
	virtual bool isConnected() { return readPV_->canRead() && writePV_->canWrite(); }
	virtual double minimumValue() { return writePV_->lowerControlLimit(); }
	virtual double maximumValue() { return writePV_->upperControlLimit(); }
	bool valueOutOfRange(double value) { return (value > maximumValue() || value< minimumValue() ) ? TRUE : FALSE;}
	bool shouldMove() { return TRUE;}
	// this is the "setpoint": the last place the control was told to go:
	virtual double setpoint() { return writePV_->lastValue(); }

	// Reimplemented public slots:
public slots:
	virtual void move(double value);
	// This is used to cancel a move.
	virtual void stop();

	// New signals:
signals:
	void writeConnectionTimeout();

	// Adapts the "connected()", "disconnected()", etc. signals from ReadOnlyPVControl. For example, connected() indicates that both readPVConnected() and writePVConnected() happened. disconnected() is given if either one disconnects.
protected slots:

	// override this if you want custom handling if the readPV fails to connect.
	// You can also monitor the connectionTimeout() signal.
	virtual void onConnectionTimeout();

	// This is called when a PV channel connects or disconnects
	virtual void onPVConnected(bool connected);
	// This is called when there is a PV channel error:
	virtual void onPVError(int error);


protected:
	DoubleProcessVariable* writePV_;
	IntProcessVariable* stopPV_;
	double tolerance_;

};*/

#endif // CONTROL_H
