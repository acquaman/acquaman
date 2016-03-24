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


#ifndef AM_CONTROL_H
#define AM_CONTROL_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
#include <QTimer>
#include <cmath>
#include <float.h>

#include "dataman/info/AMControlInfo.h"
#include "actions3/AMAction3.h"
/**
 * \defgroup control Beamline Control with AMControl and AMProcessVariable
 @{
 */


/// Defines a tolerance level that indicates you don't care about the accuracy of this AMControl
#define AMCONTROL_TOLERANCE_DONT_CARE DBL_MAX


/// An AMControl is an abstract representation of all basic scientific quantities that can be adjusted (controlled) or measured.
/*!

  Fundamentally, a Control can be "measured" and/or "moved".  All controls have the following abilities:

  \b actions:

  - FailureExplanation move(double setpoint): set a Control to a requested target value. Returns a FailureExplanation if the move() cannot be accepted for any reason [ex: control not connected, a move is already in progress and allowsMovesWhileMoving() is false, etc.].  Returns NoFailure (0) on success.

  \b properties:

  - double value(): the readout/value/position at this instant in time
  - QString name(): A descriptive (and hopefully unique) name for this Control

  - isConnected(): indicates that the control is hooked up and operating correctly; ready for use.
  - signal: connected(bool isConnected): emitted for changes in isConnected()

  - bool shouldMeasure(): this control should be capable of reporting it's current value, assuming that it's connected.
  - bool shouldMove(): this control should be capable of being adjusted, assuming that it's connected.
  - bool canMeasure(), canMove(): indicate that, at this instant in time, the control is capable of being measured or moved

  - QPair<double, double> range()
  - double minimumValue():
  - double maximumValue(): the limits of this control's range

  - double setpoint(): For Controls that can be moved, this is the most recently requested target (desired value)
  - double tolerance(): This "deadband" or "epsilon" represents how close a AMControl must get to it's target value for it to count as inPosition() (or for the move to have succeeded).  It represents the level of accuracy the scientist requires for this quantity; ie: the maximum acceptable error between value() and setpoint().  The default value is CONTROL_TOLERANCE_DONT_CARE -- a very large number.  Use setTolerance() to change it.
  - bool inPosition(): indicates the AMControl is within tolerance() of it's requested target.
  - bool withinTolerance(double target): indicates the AMControl is within tolerance() of a specified target

  <b>Monitoring Motion</b>

  There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

One concept of moving simply reflects what you would see in the physical world: is the value of the Control currently changing? (either triggered by the user, or by external inputs)
In other cases, what you really want to know is whether the Control started or stopped moving <em>in response to your instructions</em>.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

The Control abstraction provides two different properties (and associated signals) for these two concepts. The first one simply indicates motion, regardless of the cause:

  - bool isMoving(): indicates that the Control's measurement is transient / on its way toward a steady-state value.
  - signal: movingChanged(bool isMoving): provides notification of changes in isMoving()
  - signal: valueChanged(double newValue): emits a signal whenever a new sample (measured value) is obtained.

The second category only reflects motion due to instructions sent by \em this software:

  - bool moveInProgress(): A move that you requested is currently happening
  - signal: moveStarted(): emitted when moveInProgress first goes to true
  - signal: moveSucceeded(): emitted when a move completes successfully, and the final value() is within tolerance of the setpoint.
  - signal: moveFailed(int explanation): emitted when a move is not able to complete for some reason. Possible explanation codes are:
 -# Could not start the move because the control's not connected / responding
 -# The move finished, but failed to achieve the tolerance requested
 -# The move did not complete within a timeout period specified
 -# an error code defined by the specific control implementation

<b>Re-targetting moves while already moving</b>
Some devices can accept new setpoints while in motion toward a first setpoint, while others cannot.  Use setAllowsMovesWhileMoving() to specify whether move() commands should be accepted while the device is already moving.

Defaults:
- AMControl: allowsMovesWhileMoving() == false
- AMPVControl: allowsMovesWhileMoving() == true, since we have no really reliable way to tell if these controls are actually moving.
- AMPVwStatusControl: allowsMovesWhileMoving() == false.

If moves while moving are not accepted, move() will return AlreadyMovingFailure and do nothing.  If they are accepted, moveReTargetted() will be emitted for each re-targetted move(); however, no moveSucceeded() or moveFailed() will be emitted for the initial move.  Instead, only one moveSucceeded() or moveFailed() will be emitted when motion finally stops.  This behaviour is chosen so that the moveSucceeded() or moveFailed() from the intial move would not be confused as a moveSucceeded() or moveFailed() for the subsequent moves.

<b>Normal move:</b> moveStarted().... ...[moveSucceeded() or moveFailed()]
<b>Re-targetted once:</b> moveStarted()... moveReTargetted()... ...[moveSucceeded() or moveFailed()]
<b>Re-targetted twice:</b> moveStarted()... moveReTargetted()... moveReTargetted()...[moveSucceeded() or moveFailed()]

<b>Grouping Controls</b>

One additional feature of Controls is the ability to logically group sets of sub-controls together.  (For example, a Monochromator control could consist of a Grating angle control, exit slit position control, and grating selector.)
Every Control therefore has a list of subcontrols:

\b Actions:

- void addChild(AMControl* newChild): Insert a subcontrol into this group

\b Properties:

- QList<AMControl*> children(): returns the subcontrols grouped under this control.
- int numChildren(): number of subcontrols in the group
- AMControl* child(int index): return a single subcontrol

One powerful ability of the control grouping is being able to generate a list of the current state of a tree of subcontrols, and then restoring the controls to the same state later:

- QMap<QString, double> currentState() const;
- bool setState(QMap<QString, double> namesAndValues);

\todo implement currentState()


<b> Implementing Real Controls</b>

These properties are defined as the general characteristics that all Controls are expected to have, regardless of the actual hardware or connection method. (For example, they should be equally useful for representing Process Variables in an Epics-based control system, devices connected directly via TCP-IP or serial lines, or off-site controls interfaced using smoke signals or bongo drums.)

The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections:

<b>Example Classes</b>
<table>
<tr><td>Class Name					<td>Description
<tr><td>AMControl					<td>Interface definition. Can be instantiated, but not particularly useful.
<tr><td>AMCompositeControl			<td>Empty control that doesn't move or measure, but reports the combined moving status and connected status of its subcontrols
<tr><td>AMReadOnlyPVControl			<td>Measures a single process variable.  Cannot be used to set values.
<tr><td>AMPVControl					<td>Measures a feedback process variable, and moves a setpoint PV.
<tr><td>AMReadOnlyPVwStatusControl	<td>Measures a single process variable, and monitors a second PV for moving status
<tr><td>AMPVwStatusControl			<td>Measures a feedback PV, moves a setpoint PV, and monitors a status PV
</table>


<b>Examples of where to use these</b>
<table>
<tr><td>Class Name					<td>Usage example
<tr><td>AMControl					<td>
<tr><td>AMCompositeControl			<td>A "Beamline" group that shows whether anything on the beamline is moving or disconnected
<tr><td>AMReadOnlyPVControl			<td>Observing the storage ring current ("isMoving()" is hard to define here, so its meaningless)
<tr><td>AMPVControl					<td>High-voltage bias settings, with a setpoint and feedback value
<tr><td>AMReadOnlyPVwStatusControl	<td>Another beamline's energy motor feedback, with moving/not-moving indicators
<tr><td>AMPVwStatusControl			<td>Your beamline's energy motor setpoint and feedback
</table>

We need to carefully specify the behavior for all of these implementation-defined situations:

<b>isConnected() means:</b>

<table>
<tr><td>AMControl					<td>[is always false]
<tr><td>AMCompositeControl			<td>all children are connected
<tr><td>AMReadOnlyPVControl			<td>feedback PV is connected
<tr><td>AMPVControl					<td>feedback and setpoint PVs are connected
<tr><td>AMReadOnlyPVwStatusControl	<td>feedback and motion-status PVs are connected
<tr><td>AMPVwStatusControl			<td>feedback, setpoint, and motion-status PVs are all connected
</table>

<b>isMoving() means:</b>

<table>
<tr><td>AMControl					<td>[is always false]
<tr><td>AMCompositeControl			<td>any child is moving
<tr><td>AMReadOnlyPVControl			<td>[is always false: no good way to determine this]
<tr><td>AMPVControl					<td>one of your moves is executing (moveInProgress = true)
<tr><td>AMReadOnlyPVwStatusControl	<td>motion-status PV indicates that the control is moving
<tr><td>AMPVwStatusControl			<td>motion-status PV indicates that the control is moving
</table>

<b>moveSucceeded() means:</b>

<table>
<tr><td>AMControl					<td>[never happens]
<tr><td>AMCompositeControl			<td>[never happens]
<tr><td>AMReadOnlyPVControl			<td>[never happens]
<tr><td>AMPVControl					<td>after starting a move, the value() came within tolerance() of setpoint() before completionTimeout()
<tr><td>AMReadOnlyPVwStatusControl	<td>[never happens]
<tr><td>AMPVwStatusControl			<td>motion-status PV indicates that the control stopped moving, and the value was not within tolerance of setpoint()
</table>

<b>moveFailed() means:</b>

<table>
<tr><td>AMControl					<td>[never happens]
<tr><td>AMCompositeControl			<td>[never happens]
<tr><td>AMReadOnlyPVControl			<td>[never happens]
<tr><td>AMPVControl					<td>after starting a move, the completionTimeout() occured before value() came within tolerance() of setpoint()
<tr><td>AMReadOnlyPVControl			<td>[never happens]
<tr><td>AMPVwStatusControl			<td>The motion-status PV indicated that the control stopped moving, and the value was not within tolerance() of setpoint.  OR... The PV did not start isMoving() within startTimeout().
</table>


  */


class AMControl : public QObject {

	Q_OBJECT
public:

	/// This enum type is used to describe the reason for a move failure.
	/*! Possible explanation codes are:
 */
	enum FailureExplanation {
		NoFailure = 0,
		NotConnectedFailure, ///< Could not start the move because the control's not connected / responding
		ToleranceFailure,		///< The move finished, but failed to achieve the tolerance requested
		TimeoutFailure,			///< The move did not complete within a timeout period specified
		WasStoppedFailure,		///< The move was prematurely stopped with the stop() command.
		AlreadyMovingFailure,	///< The move could not be started because the control was already moving, and it does not allowsMovesWhileMoving().
		RedirectedFailure,		///< Never emitted by AMControl classes. Can be used to represent that a move was redirected while already moving.
		LimitFailure,			///< Emitted by subclasses that may have information about movements that cannot proceed based on limits
		OtherFailure 			///< An error code defined by the specific control implementation
	};

	/// An english-language version of FailureExplanation
	static QString failureExplanation(FailureExplanation f) {
		QString explanation;
		switch(f) {
		case AMControl::NoFailure:				explanation = "There was no actual failure."; break;	// Nobody should ever see this.
		case AMControl::NotConnectedFailure:	explanation = "The control was not connected."; break;
		case AMControl::ToleranceFailure:		explanation = "The required tolerance was not met."; break;
		case AMControl::TimeoutFailure:			explanation = "The move timed out without starting or reaching its destination."; break;
		case AMControl::WasStoppedFailure:		explanation = "The move was manually interrupted or stopped."; break;
		case AMControl::AlreadyMovingFailure:	explanation = "The control was already moving."; break;
		case AMControl::RedirectedFailure:		explanation = "The move was externally re-directed to another destination."; break;
		case AMControl::LimitFailure:			explanation = "The move requested is not possible because it exceeds a hardware limit."; break;
		case AMControl::OtherFailure:
		default:
			explanation = "An undocumented failure happened."; break;
		}
		return explanation;
	}
	/// An english-language version of FailureExplanation
	static QString failureExplanation(int f) { return failureExplanation(FailureExplanation(f)); }

	/// This enum type is used to describe the different types of relative moves that can be done
	/*! Possible relative move types are:
	*/
	enum RelativeMoveType {
		RelativeMoveFromValue = 0,	///< Relative move from the current value (if the control is has a setpoint of 100.00 and a feedback of 99.99, this type of relative move will set the setpoint to 100.99 on a relative move of +1.00)
		RelativeMoveFromSetpoint = 1,	///< Relative move from the current setpoint (if the control is has a setpoint of 100.00 and a feedback of 99.99, this type of relative move will set the setpoint to 101.00 on a relative move of +1.00)
		RelativeMoveInvalid = 2		///< Invalid enum holder
	};

	/// This enum type is used to describe problematic states the control can be in
	/*! Possible explanation codes are:
   */
	enum ErrorExplanation{
		CannotConnectError = 1, ///< Cannot connect to part of the control
		CannotReadError,	///< Cannot read when we are connected and expect to be able to
		CannotWriteError,	///< Cannot write when we are connected and expect to be able to
		CannotGetStatusError	///< Cannot read the status when we are connected
	};

	/// Alarm levels defined for controls. These correspond to traditional EPICS alarm severity levels, but can be used by other control systems as well.
	enum AlarmLevel {
		NoAlarm = 0,
		MinorAlarm,
		MajorAlarm,
		InvalidAlarm
	};

	/// Base Class Constructor
	/*! \param name A unique descriptive name for this control.
  \param units The default unit description.
  \param parent QObject parent for memory management
  */
 	virtual ~AMControl();
	AMControl(const QString& name, const QString& units = "", QObject* parent = 0, const QString description = "");

	/// \name Control info
	/// One feature of a control is that it can create a snapshot of its current state and pass it on as an AMControlInfo.
	AMControlInfo toInfo() { return AMControlInfo(name(), value(), minimumValue(), maximumValue(), units(), tolerance(), description(), contextKnownDescription(), isEnum() ? enumNameAt(value()) : QString() ); }

	/// Returns a string representation of the control.
	virtual QString toString() const;

	/// \name Accessing childControls() subcontrols:
	/// One additional feature of Controls is the ability to logically group sets of sub-controls together. (For example, a Monochromator control could consist of a Grating angle control, exit slit position control, and grating selector.)  Every Control therefore has a list of subcontrols.
	//@{

	/// The list of sub-controls associated with this control.

	/// \return A list of pointers to all sub-controls.
	QList<AMControl*> childControls() const { return children_; }
	/// Returns the number of subcontrols in this group.
	int childControlCount() const { return children_.count(); }
	/// Retreive a specific child control from the list by index
	AMControl* childControlAt(unsigned index) const{
		if(index < (unsigned)children_.count())
			return children_.at(index);
		return NULL;
	}
	/// Returns true if this control has children and if one of them (or one of their children) matches the given control. Returns false otherwise.
	bool hasChildControl(AMControl *control) const;
	/// Add a subcontrol to the control group. Subclasses can reimplement this if they need to connect to the child's signals, etc.
	virtual void addChildControl(AMControl* control) { children_ << control; }
	/// Removes a subcontrol from the control group.
	virtual void removeChildControl(AMControl *control);
	/// Clears the child controls.
	virtual void clearChildControls();
	//@}

	/// Returns a descriptive and hopefully-unique name for this control:
	QString name() const { return objectName(); }

	/// Returns a human-readable description for this control
	QString description() const { return description_;}

	/// Returns a short human-readable description for when the context is known or is implicit
	QString contextKnownDescription() const { return contextKnownDescription_;}

	/// This value defines how close the final value() must be to the setpoint(), for the move() to have succeeded.
	double tolerance() const { return tolerance_; }

	/// This flag determines if moves happen when already within tolerance. If true (default) then a control will attempt a move no matter what. If that control never switches to the moving state, then the move will likely timeout even though it is within tolerance.
	bool attemptMoveWhenWithinTolerance() const { return attemptMoveWhenWithinTolerance_; }

	/// This represents the current value/position of the control. Must reimplement for actual controls.
	virtual double value() const { return -1; }

	/// this is the "setpoint": the last place the control was told to go:
	virtual double setpoint() const { return -1; }

	/// this indicates whether a control is "in position" (ie: its value is within tolerance of the setpoint)
	virtual bool inPosition() const { return fabs(value()-setpoint()) < tolerance(); }

	/// this indicates whether a control is "within tolerance" of a given target (ie: the target specified is the same as the current value within the set tolerance)
	virtual bool withinTolerance(double target) const { return fabs(value()-target) < tolerance(); }

	/// \name Capabilities
	/// These indicate the current cabilities and status of this control. Unconnected controls can't do anything.
	//@{
	/// Indicates a "fully-functional" control, ready for action
	virtual bool isConnected() const { return false; }
	/// Indicates that this control \em can currently take measurements.
	virtual bool canMeasure() const { return false; }
	/// Indicates that this control type \em should (assuming it's connected) be able to measure values.
	virtual bool shouldMeasure() const { return false; }
	/// Indicates thatthis control \em can (currently) move to setpoints:
	virtual bool canMove() const { return false; }
	/// Indicates that this control \em should (assuming it's connected) be able to move to setpoints:
	virtual bool shouldMove() const { return false; }
	/// Indicates that this control \em can (currently) issue stop() commands while moves are in progress.
	virtual bool canStop() const { return false; }
	/// Indicates that this control \em shoule (assuming it's connected) be able to issue stop() commands while moves are in progress.
	virtual bool shouldStop() const { return false; }
	/// Indicates that this control \em can currently be calibrated.
	virtual bool canCalibrate() const { return false; }
	/// Indicates that this control \em should (assuming it's connected) be calibrated.
	virtual bool shouldCalibrate() const { return false; }
	/// Indicates that this control \em should (assuming it's connected) be calibrated.
	virtual bool shouldPerformCoordinatedMovement() const { return false; }
	/// Indicates that this control \em should (assuming it's connected) be able to perform continuous movements.
	virtual bool canPerformCoordinatedMovement() const { return false; }
	/// Indicates that this control should accept move() requests while it is already isMoving(). Some hardware can handle this. If this is false, move() requests will be ignored when the control is already in motion.
	bool allowsMovesWhileMoving() const { return allowsMovesWhileMoving_; }
	//@}

	/// Indicates that the Control's measurement is transient (ie: on its way toward a steady-state value)
	/*! There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems like in a synchrotron.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

  One concept of moving simply reflects what you would see in the physical world: is the value of the Control currently changing? (either triggered by the user, or by external inputs)

In other cases, what you really want to know is whether the Control started or stopped moving <em>in response to your instructions</em>.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

The Control abstraction provides two different properties (and associated signals) for these two concepts. The first one simply indicates motion, regardless of the cause:

- bool isMoving(): indicates that the Control's measurement is transient / on its way toward a steady-state value.
- signal: movingChanged(bool isMoving): provides notification of changes in isMoving()
- signal: valueChanged(double newValue): emits a signal whenever a new sample (measured value) is obtained.

   */
	virtual bool isMoving() const { return false; }


	/// Indicates that a move() sent by this software is currently happening.
	/*! There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems like in a synchrotron.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

 Sometimes, what you really want to know is whether the Control started or stopped moving <em>in response to your instructions</em>.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

 The Control abstraction provides two different properties (and associated signals) for these two concepts. The second category only reflects motion due to instructions sent by \em this software:

  - bool moveInProgress(): A move that you requested is currently happening
  - signal: moveStarted(): emitted when moveInProgress first goes to true
  - signal: moveReTargetted(): emitted when allowsMovesWhileMoving() is true, and a move is re-directed to a different setpoint while already moving.
  - signal: moveSucceeded(): emitted when a move completes successfully, and the final value() is within tolerance of the setpoint.
  - signal: moveFailed(int explanation): emitted when a move is not able to complete for some reason. Possible explanation codes are:

  - \c AMControl::NotConnectedFailure (1): Could not start the move because the control's not connected or responding
  - \c AMControl::ToleranceFailure (2): The move finished, but failed to achieve the tolerance requested
  - \c AMControl::TimeoutFailure (3): The move failed to finish (or start) within a timeout period specified.
  - \c AMContro::WasStoppedFailure (4): The move was interrupted by calling stop() before it could finish.
  - \c AMControl::OtherFailure (5) and higher: an error code defined by the specific control implementation


*/
	virtual bool moveInProgress() const { return false; }

	/// indicates whether the units is initiazted
	bool isUnitsInitialized() { return units_.length() != 0 && units_ != "?"; }
	/// Indicates the units associated with the value of this control.
	QString units() const { return units_; }
	/// Indicates the number of digits after the decimal point that are recommended for displaying this control's value.
	int displayPrecision() const { return displayPrecision_; }

	/// \name Information on the allowed range for this control:
	//@{
	/// These are the minimum and maximum allowed values for this control.
	virtual QPair<double,double> range() const { return QPair<double,double>(minimumValue(), maximumValue()); }
	virtual double minimumValue() const { return -1; }
	virtual double maximumValue() const { return -1; }
	/// Checks whether \p value is within the allowed range for this control.
	bool valueOutOfRange(double value) const { return (value > maximumValue() || value< minimumValue() ) ? true : false;}
	//@}

	/// \name Discrete Controls
	/// Some controls are discrete... Enum controls only provide specific discrete states/choices. (For example, the grating selector on most beamlines: Grating 1, Grating 2, Grating 3).
	//@{

	/// Indicates that a control is discrete.

	/*! \note This may not be valid the instant you create the control.  Watch for enumChanged().)
   \return true if this control represents a discrete set of choices.
  \sa enumNames(), enumCount()
  */
	bool isEnum() const { return (!enumNames_.isEmpty()); }

	/// If it is a discrete control, this gives you a list of descriptions for each state. (They match with numerical values 0, 1, 2, ...)

	/// \return a list of string descriptions for the states.  Returns an empty list if the control is not discrete. (ie: isEnum() == false).
	QStringList enumNames() const { return enumNames_; }

	/// If it is a discrete control, this tells you how many discrete options/states are available:
	unsigned enumCount() const { return enumNames_.count(); }

	/// Returns the enum string for a given \c controlValue. This function will check to make sure the control value is within the range of the set of enums, and return "[enum value out of range]" if not.
	QString enumNameAt(double controlValue) {
		int enumValue = (int)controlValue;
		if((unsigned)enumValue < (unsigned)enumNames_.count())
			return enumNames_.at(enumValue);
		else
			return "[enum value out of range]";
	}


	/// Sometimes the valid enum choices to use for move() may be different than the choices that describe value().  For example, value() might be described by "Open", "Closed", and "Moving", whereas for move(), only "Open" and "Close" may be relevant. This returns the enum values acceptable for move().
	/*! Note that if a control does not explicitly specify separate enums for move(), the regular enumNames() will be used.*/
	QStringList moveEnumNames() const { if(moveEnumNames_.isEmpty()) return enumNames(); return moveEnumNames_; }

	/// This indicates how many discrete options/states are available for move() choices.
	/*! Note that if a control does not explicitly specify separate enums for move(), the regular enumCount() will be used.*/
	unsigned moveEnumCount() const { if(moveEnumNames_.isEmpty()) return enumCount(); return moveEnumNames_.count(); }

	/// Returns the move()-relevant enum string for a given \c controlValue. This function will check to make sure the control value is within the range of moveEnumCount(), and return "[enum value out of range]" if not.
	/*! Note that if a control does not explicitly specify separate enums for move(), the regular enumNameAt() will be used.*/
	QString moveEnumNameAt(double controlValue) {
		if(moveEnumNames_.isEmpty())
			return enumNameAt(controlValue);

		int enumValue = (int)controlValue;
		if((unsigned)enumValue < (unsigned)moveEnumNames_.count())
			return moveEnumNames_.at(enumValue);
		else
			return "[enum value out of range]";
	}


	//@}


	/// Returns the alarm severity for this control.  This indicates how "serious" the alarm is, it is one of the values in AMControl::AlarmLevel.
	virtual int alarmSeverity() const { return NoAlarm; }
	/// Returns the alarm status for this control.  The alarm status is an integer that can be defined by the control implementation to explain the reason for the alarm.
	virtual int alarmStatus() const { return 0; }

	/// A list of actions which sets the parameters for a coordinated movement.
	virtual AMAction3* createSetParametersActions(double /*startPoint*/, double /*endPoint*/, double /*deltaTime*/) { return 0; }
	/// A list of actions which defines the steps required to initialize a coordinated movement.
	virtual AMAction3* createInitializeCoordinatedMovementActions() { return 0; }
	/// A list of actions which defines the steps required to trigger an initialized coordinated movement.
	virtual AMAction3* createStartCoordinatedMovementActions() { return 0; }
	/// A list of wait actions which, when complete, signify that a coordinated movement has been completed.
	virtual AMAction3* createWaitForCompletionActions() { return 0; }
public slots:
	/// This is used to move the control to a \c setpoint.  Returns NoFailure (0) if the move command was sent. (Does not guarantee that the move was actually started.)  Returns a FailureExplanation if it is known immediately that the control cannot be moved. Must reimplement for actual controls.
	virtual FailureExplanation move(double setpoint) {
		Q_UNUSED(setpoint);
		return NotConnectedFailure;
	}
	/// This is used to move a control a relative distance from its current position. Capable of moving relative from the feedback value or the setpoint (see enum documentation for details). For historic reasons, the default is relative from feedback value. The base class implementation simply issues a move() to the current value() plus \c distance. This may not be sufficient if moveRelative() will be called faster than value() updates in your implementation, in that case, it's recommended to re-implement this as appropriate.
	virtual FailureExplanation moveRelative(double distance, AMControl::RelativeMoveType relativeMoveType = AMControl::RelativeMoveFromValue) {
		switch(relativeMoveType){
		case AMControl::RelativeMoveFromValue:
			return move(value() + distance);
		case AMControl::RelativeMoveFromSetpoint:
			return move(setpoint() + distance);
		default:
			return move(value() + distance);
		}
	}

	/// This calibrates the control such that the old value becomes the new value. Fails if calibration has not been reimplemented.
	virtual FailureExplanation calibrate(double oldValue, double newValue) {
		Q_UNUSED(oldValue)
		Q_UNUSED(newValue)
		return OtherFailure;
	}

	/// This sets the tolerance level: the required level of accuracy for successful move()s.
	void setTolerance(double newTolerance) { tolerance_ = newTolerance; }

	/// This sets the flag to determine if moves happen when already within tolerance. If true (default) then a control will attempt a move no matter what. If that control never switches to the moving state, then the move will likely timeout even though it is within tolerance.
	void setAttemptMoveWhenWithinTolerance(bool attemptMoveWhenWithinTolerance) { attemptMoveWhenWithinTolerance_ = attemptMoveWhenWithinTolerance; }

	/// This sets the human-readable description for the control
	void setDescription(const QString &description) { description_ = description; }

	/// This sets the short form of a human-readable description. Very short, for when the context is known: Might be "X" as opposed to "SSA Manipulator X"
	void setContextKnownDescription(const QString &contextKnownDescription) { contextKnownDescription_= contextKnownDescription; }

	/// This is used to cancel or stop a move in progress. Must reimplement for actual controls.  It will be successful only if canStop() is true.  Returns true if the stop command was successfully sent.  (Note: this DOES NOT guarantee that the motor actually stopped!)
	virtual bool stop() { return false; }

	/// Set this control to accept move() requests while it isMoving(). The default is to reject move() requests if the control is already moving. This should be changed for controls that are capable of being re-targetted while in motion.
	void setAllowsMovesWhileMoving(bool allowMovesWhileMoving) { allowsMovesWhileMoving_ = allowMovesWhileMoving; }


signals:
	/// \name Signals associated with moveInProgress
	/// These signals announce changes of moveInProgress(). They only apply to moves started by a local move() command... ie: they don't occur when another system causes the control to change its value.
	//@{
	/// Emitted when moveInProgress() goes to true.
	void moveStarted();
	/// Emitted when allowsMovesWhileMoving() is true, and a move() is commanded while already moving. You can call setpoint() to find out where it's been updated to go to.
	void moveReTargetted();
	/// Emitted when a move successfully completes. (This is up to the implementation to define what counts as successful and complete.)
	void moveSucceeded();
	/// Emitted when a move is not able to complete successfully for some reason.
	/*! Possible explanation codes are defined in AMControl::FailureExplanation.
 */
	void moveFailed(int explanation);
	//@}

	/// \name Signals associated with isMoving()
	/// These signals announce changes of isMoving().
	/// They happen whenever the control value is known to be changing... whether we move()d it or not.
	//@{
	/// Announce changes in "isMoving()".
	void movingChanged(bool isMoving);
	/// Announces when the position/value of the control "value()" has changed.
	void valueChanged(double newValue);
	/*! Normally we expect that only this program is changing the setpoint and causing motion, using move().  If someone else changes the writePV (setpoint PV), this will signal you with the new value.*/
	void setpointChanged(double);
	//@}

	/// Announces when the unit string text has changed. Necessary because we might not know this until after a control connects.
	void unitsChanged(const QString& units);
	/// Announces when the displayPrecision() has changed. Necessary because we might not know this until after a control connects.
	void displayPrecisionChanged(int displayPrecision);
	/// Announces when the number or descriptions of discrete states changes.  Necessary because we might not know this until after a control connects.  When receiving this, you can check isEnum(), enumNames(), and moveEnumNames().
	/// \sa isEnum(), enumNames(), enumCount(), moveEnumNames(), moveEnumCount().
	void enumChanged();

	/// Announces changes in isConnected().
	/*! Emitted (true) when full functionality is achieved. Emitted (false) when full functionality is lost. What counts as functionality can be decided by the subclasses.
  */
	void connected(bool);

	/// Announces error states
	/*! Emits error codes defined above or passes along error messages if available
  */
	void error(int);
	void error(const QString&);

	/// Emitted when the control's alarm status or severity changes.  \c status is defined by the particular control implementation. \c severity is one of the levels in AMControl::AlarmLevel.
	void alarmChanged(int status, int severity);

	/// Notifier that the calibrating state has changed.
	void calibratingChanged(bool isCalibrating);
	/// Notifier that a calibration has started.
	void calibrationStarted();
	/// Notifier that a calibration has failed. Provides an integer failure explaination.
	void calibrationFailed(int explaination);
	/// Notifier that a calibration has succeeded.
	void calibrationSucceeded();
	/// Notifier that the minimum value has changed.
	void minimumValueChanged(double newValue);
	/// Notifier that the maximum value has changed.
	void maximumValueChanged(double newValue);

protected:
	/// List of pointers to our subcontrols
	QList<AMControl*> children_;
	/// True if the control should allow additional move() commands while it's already moving. Some hardware can handle this. If this is false, move() requests issued while the control is moving are ignored.  It is false by default. Subclassses should change this if required.
	bool allowsMovesWhileMoving_;

	/// A flag to tell controls whether or not to attempt a move when they are already within tolerance
	bool attemptMoveWhenWithinTolerance_;


protected slots:
	/// This is used internally by subclasses to set the unit text:
	void setUnits(const QString& newUnits) { if(units_ == newUnits) return; units_ = newUnits; emit unitsChanged(units_); }

	/// This is used internally by subclasses to set the display precision
	void setDisplayPrecision(int displayPrecision) { if(displayPrecision_ == displayPrecision) return; displayPrecision_ = displayPrecision; emit displayPrecisionChanged(displayPrecision_); }

	/// This is used internally to flag whether a Control is labelled isEnum(), and add the names for each state. If it IS a discrete control (\c enumStateNames is not empty), then the tolerance is automatically set to a value less than 1 (0.1).  This makes sense when the values must be integer (discrete) values.
	void setEnumStates(const QStringList& enumStateNames) { if(enumNames_ == enumStateNames) return; enumNames_ = enumStateNames; if(enumNames_.count() > 0) {setTolerance(0.1);} emit enumChanged(); }

	/// This is used internally to specify the allowed enum names for the move() aspect of the control, if they happen to be different than the enums that apply to value().  This can sometimes happen when there are status enums included in the value() [ex: "Open", "Closed", and "Moving"], so the valid choices for move() are different [ex: "Open" and "Close"].
	/*! If isEnum() returns true and this is not specified by a subclass implementation, the regular enumNames() will be assumed to apply for both move() and value(). moveEnumNames() will return enumNames(). */
	void setMoveEnumStates(const QStringList& enumStateNames) { if(moveEnumNames_ == enumStateNames) return; moveEnumNames_ = enumStateNames; emit enumChanged(); }

private:
	// subclasses should use the protected methods to access these, to ensure signal generation.
	double tolerance_;
	QString units_;
	QStringList enumNames_;
	QStringList moveEnumNames_;
	int displayPrecision_;
	/// Human-readable description
	QString description_;
	/// Human-readable description. Very short, for when the context is known. Might be "X" as opposed to "SSA Manipulator X"
	QString contextKnownDescription_;
};


// End of doxygen group: control
/**
 @}
 */
#endif // CONTROL_H
