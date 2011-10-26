/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef ACQMAN_CONTROL_H
#define ACQMAN_CONTROL_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
#include "AMProcessVariable.h"
#include <QDebug>
#include <QTimer>
#include <cmath>

#include <float.h>

#include "dataman/info/AMControlInfoList.h"

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

  - void move(double setpoint): set a Control to a requested target value

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
  - double tolerance(): This "deadband" or "epsilon" represents how close a AMControl must get to it's target value for it to count as inPosition() (or for the move to have succeeded).  It represents the level of accuracy the scientist requires for this quantity; ie: the maximum acceptable error between measure() and setpoint().  The default value is CONTROL_TOLERANCE_DONT_CARE -- a very large number.  Use setTolerance() to change it.
  - bool inPosition(): indicates the AMControl is within tolerance() of it's requested target.
  - bool withinTolerance(double target): indicates the AMControl is within tolerance() of a specified target

  <b>Monitoring Motion</b>

  There are two useful but distinct ideas on whether a control is "moving", particularly for distributed control systems like in a synchrotron.  Often, beamline parameters can be controlled from a user's software interface, but they can also spontaneously start moving due to external events or other control interfaces.

One concept of moving simply reflects what you would see in the physical world: is the value of the Control currently changing? (either triggered by the user, or by external inputs)
In other cases, what you really want to know is whether the Control started or stopped moving <em>in response to your instructions</em>.  This is useful when scanning a value, and wanting to receive confirmation that the Control is doing what you told it to.

The Control abstraction provides two different properties (and associated signals) for these two concepts. The first one simply indicates motion, regardless of the cause:

  - bool isMoving(): indicates that the Control's measurement is transient / on its way toward a steady-state value.
  - signal: movingChanged(bool isMoving): provides notification of changes in isMoving()
  - signal: valueChanged(double newValue): emits a signal whenever a new sample (measured value) is obtained.

The second category only reflects motion due to instructions sent by \em this software:

  - bool moveInProgress(): A move that you requested is currently happening
  - signal: moveStarted(): emitted when moveInProgress first goes to true
  - signal: moveSucceeded(): emitted when a move completes successfully, and the final measure()d value is within tolerance of the setpoint.
  - signal: moveFailed(int explanation): emitted when a move is not able to complete for some reason. Possible explanation codes are:
	-# Could not start the move because the control's not connected / responding
	-# The move finished, but failed to achieve the tolerance requested
	-# The move did not complete within a timeout period specified
	-# an error code defined by the specific control implementation

<b>Warning About Expected Behaviour</b>

All behaviour about the current state of a control is based on whether the control has made it to its destination within the tolerance() that has been set for it.
However, this assumes that you set this tolerance yourself because the default is CONTROL_TOLERANCE_DONT_CARE.
If the tolerance is left to its default value you are not able to trust the signals such as moveSucceeded() because they will be emitted the first time the system gets a feedback value, <b>even if the control is still moving</b>!
Therefore, in order to use AMControl to the fullest of its capabilities make sure you set the tolerance.


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

These properties are defined as the general characteristics that all Controls are expected to have, regardless of the actual hardware or connection method. (For example, they should be equally useful for representing Process Variables in an Epics-based control system, devices connected directly via TCP-IP or serial lines, or off-site hoofadinkuses interfaced using smoke signals or bongo drums.)

The AMControl interface should be inherited to implement real-world control devices.  The exact meaning of the properties might change depending on the physical device and its capabilities.  As a useful example, we've provided a set of real controls with different levels of capability that implement Epics Process-Variable type connections:

<b>Example Classes</b>
<table>
<tr><td>Class Name					<td>Description
<tr><td>AMControl					<td>Interface definition. Can be instantiated, but not particularly useful.
<tr><td>AMControlGroup				<td>Empty control that doesn't move or measure, but reports the combined moving status and connected status of its subcontrols
<tr><td>AMReadOnlyPVControl			<td>Measures a single process variable.  Cannot be used to set values.
<tr><td>AMPVControl					<td>Measures a feedback process variable, and moves a setpoint PV.
<tr><td>AMReadOnlyPVwStatusControl	<td>Measures a single process variable, and monitors a second PV for moving status
<tr><td>AMPVwStatusControl			<td>Measures a feedback PV, moves a setpoint PV, and monitors a status PV
</table>


<b>Examples of where to use these</b>
<table>
<tr><td>Class Name					<td>Usage example
<tr><td>AMControl					<td>
<tr><td>AMControlGroup				<td>A "Beamline" group that shows whether anything on the beamline is moving or disconnected
<tr><td>AMReadOnlyPVControl			<td>Observing the storage ring current ("isMoving()" is hard to define here, so its meaningless)
<tr><td>AMPVControl					<td>High-voltage bias settings, with a setpoint and feedback value
<tr><td>AMReadOnlyPVwStatusControl	<td>Another beamline's energy motor feedback, with moving/not-moving indicators
<tr><td>AMPVwStatusControl			<td>Your beamline's energy motor setpoint and feedback
</table>

We need to carefully specify the behavior for all of these implementation-defined situations:

<b>isConnected() means:</b>

<table>
<tr><td>AMControl					<td>[is always false]
<tr><td>AMControlGroup				<td>all children are connected
<tr><td>AMReadOnlyPVControl			<td>feedback PV is connected
<tr><td>AMPVControl					<td>feedback and setpoint PVs are connected
<tr><td>AMReadOnlyPVwStatusControl	<td>feedback and motion-status PVs are connected
<tr><td>AMPVwStatusControl			<td>feedback, setpoint, and motion-status PVs are all connected
</table>

<b>isMoving() means:</b>

<table>
<tr><td>AMControl					<td>[is always false]
<tr><td>AMControlGroup				<td>any child is moving
<tr><td>AMReadOnlyPVControl			<td>[is always false: no good way to determine this]
<tr><td>AMPVControl					<td>one of your moves is executing (moveInProgress = true)
<tr><td>AMReadOnlyPVwStatusControl	<td>motion-status PV indicates that the control is moving
<tr><td>AMPVwStatusControl			<td>motion-status PV indicates that the control is moving
</table>

<b>moveSucceeded() means:</b>

<table>
<tr><td>AMControl					<td>[never happens]
<tr><td>AMControlGroup				<td>[never happens]
<tr><td>AMReadOnlyPVControl			<td>[never happens]
<tr><td>AMPVControl					<td>after starting a move, the measure()d value came within tolerance() of setpoint() before completionTimeout()
<tr><td>AMReadOnlyPVwStatusControl	<td>[never happens]
<tr><td>AMPVwStatusControl			<td>motion-status PV indicates that the control stopped moving, and the value was not within tolerance of setpoint()
</table>

<b>moveFailed() means:</b>

<table>
<tr><td>AMControl					<td>[any move]
<tr><td>AMControlGroup				<td>[any move]
<tr><td>AMReadOnlyPVControl			<td>[any move]
<tr><td>AMPVControl					<td>after starting a move, the completionTimeout() occured before measure()d value came within tolerance() of setpoint()
<tr><td>AMReadOnlyPVwStatusControl	<td>[any move]
<tr><td>AMPVwStatusControl			<td>motion-status PV indicated that the control stopped moving, and the value was not within tolerance() of setpoint.  OR... The PV did not start isMoving() within startTimeout().
</table>


  */


class AMControl : public QObject {

	Q_OBJECT
public:

	/// This enum type is used to describe the reason for a move failure.
	/*! Possible explanation codes are:
	*/
	enum FailureExplanation {
		NotConnectedFailure = 1, ///< Could not start the move because the control's not connected / responding
		ToleranceFailure,		///< The move finished, but failed to achieve the tolerance requested
		TimeoutFailure,			///< The move did not complete within a timeout period specified
		WasStoppedFailure,		///< The move was prematurely stopped with the stop() command.
		OtherFailure 			///<  an error code defined by the specific control implementation
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

	/// Base Class Constructor
	/*! \param name A unique descriptive name for this control.
		\param units The default unit description.
		\param parent QObject parent for memory management
		*/
	AMControl(const QString& name, const QString& units = "n/a", QObject* parent = 0, const QString description = "") : QObject(parent), units_(units), description_(description) {
		setObjectName(name);
		wasConnected_ = false;
		tolerance_ = AMCONTROL_TOLERANCE_DONT_CARE;
		contextKnownDescription_ = "";
	}

	/// \name Control info
	/// One feature of a control is that it can create a snapshot of its current state and pass it on as an AMControlInfo.
	AMControlInfo toInfo() { return AMControlInfo(name(), value(), minimumValue(), maximumValue(), units(), tolerance(), description(), contextKnownDescription()); }

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
	/// Add a subcontrol to the control group. Subclasses can reimplement this if they need to connect to the child's signals, etc.
	virtual void addChildControl(AMControl* control) { children_ << control; }
	//@}

	/// Returns a descriptive and hopefully-unique name for this control:
	QString name() const { return objectName(); }

	/// Returns a human-readable description for this control
	QString description() const { return description_;}

	/// Returns a short human-readable description for when the context is known or is implicit
	QString contextKnownDescription() const { return contextKnownDescription_;}

	/// This value defines how close the final measure()d position must be to the setpoint(), for the move() to have succeeded.
	double tolerance() const { return tolerance_; }

	/// This represents the current value/position of the control. Must reimplement for actual controls.
	virtual double value() const { return -1; }

	/// this is the "setpoint": the last place the control was told to go:
	virtual double setpoint() const { return -1; }

	/// this indicates whether a control is "in position" (ie: its value is within tolerance of the setpoint)
	virtual bool inPosition() const { return fabs(value()-setpoint()) < tolerance(); }

	/// this indicates whether a contorl is "within tolerance" of a given target (ie: the target specified is the same as the current value within the set tolerance)
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
  - signal: moveSucceeded(): emitted when a move completes successfully, and the final measure()d value is within tolerance of the setpoint.
  - signal: moveFailed(int explanation): emitted when a move is not able to complete for some reason. Possible explanation codes are:

		- \c AMControl::NotConnectedFailure: Could not start the move because the control's not connected or responding
		- \c AMControl::ToleranceFailure: The move finished, but failed to achieve the tolerance requested
		- \c AMControl::TimeoutFailure: The move failed to finish (or start) within a timeout period specified.
		- \c AMControl::OtherFailure and higher: an error code defined by the specific control implementation


*/
	virtual bool moveInProgress() const { return false; }

	/// Indicates the units associated with the value of this control.
	virtual QString units() const { return units_; }

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

	/*! \note This may not be valid the instant you create the control.  Watch for enumChanges(const QStringList& newEnumNames).)
		 \return true if this control represents a discrete set of choices.
		\sa enumNames(), enumCount()
		*/
	bool isEnum() const { return (enumNames_.count() > 0); }

	/// If it is a discrete control, this gives you a list of descriptions for each state. (They match with numerical values 0, 1, 2, ...)

	/// \return a list of string descriptions for the states.  Returns an empty list if the control is not discrete. (ie: isEnum() == false).
	QStringList enumNames() const { return enumNames_; }

	/// If it is a discrete control, this tells you how many discrete options/states are available:
	unsigned enumCount() const { return enumNames_.count(); }	// TODO: there could be problems if you use a feedback PV and a setpoint PV with different number of enum choices. How to handle this?

	/// Returns the enum string for a given \c controlValue. This function will check to make sure the control value is within the range of the set of enums.
	QString enumNameAt(double controlValue) {
		int enumValue = controlValue;
		if((unsigned)enumValue < (unsigned)enumNames_.count())
			return enumNames_.at(enumValue);
		else
			return "[enum value out of range]";
	}

	//@}

public slots:
	/// This is used to move the control to a setpoint.  Must reimplement for actual controls
	virtual void move(double setpoint) {
		Q_UNUSED(setpoint);
		emit moveFailed(AMControl::NotConnectedFailure);  // The default implementation cannot move.
	}

	/// This sets the tolerance level: the required level of accuracy for successful move()s.
	void setTolerance(double newTolerance) { tolerance_ = newTolerance; }

	/// This sets the human-readable description for the control
	void setDescription(const QString &description) { description_ = description; }

	/// This sets the short form of a human-readable description for when the context is known or implicit
	void setContextKnownDescription(const QString &contextKnownDescription) { contextKnownDescription_= contextKnownDescription; }

	/// This is used to cancel or stop a move in progress. Must reimplement for actual controls.  It will be successful only if canStop() is true.  Returns true if the stop command was successfully sent.  (Note: this DOES NOT guarantee that the motor actually stopped!)
	virtual bool stop() { return false; }

	/// Moves all of the AMControl's subcontrols (children and grandchildren, etc) based on a \c controlList QMap of Control Names and setpoint values.
	/*! \param controlList specifies a set of AMControls by their name(), and specifies a target value for each.
		\param errorLevel specifies what counts as success. \todo David: write out what these are.
		\todo Change name to setChildrenState().
	  */
	bool setState(const QMap<QString, double> controlList, unsigned int errorLevel = 0);


signals:
	/// \name Signals associated with moveInProgress
	/// These signals announce changes of moveInProgress(). They only apply to moves started by a local move() command... ie: they don't occur when another system causes the control to change its value.
	//@{
	/// Emitted when moveInProgress() goes to true.
	void moveStarted();
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

	/// Announces when the unit string text has changed. Necessary because we might not know this until after a delayed control connects.
	void unitsChanged(const QString& units);
	/// Announces when the number or descriptions of discrete states changes.  Necessary because we might not know this when the Control is first created.
	/// \sa isEnum(), enumNames(), and enumCount().
	void enumChanges(const QStringList& enumStateNames);

	/// Announces changes in isConnected().
	/*! Emitted (true) when full functionality is achieved. Emitted (false) when full functionality is lost. What counts as functionality can be decided by the subclasses.
		*/
	void connected(bool);

	/// Announces error states
	/*! Emits error codes defined above or passes along error messages if available
		*/
	void error(int);
	void error(const QString&);

protected:
	/// List of pointers to our subcontrols
	QList<AMControl*> children_;
	/// Used to detect changes in isConnected()
	bool wasConnected_;


protected slots:
	/// This is used internally to set the unit text:
	virtual void setUnits(const QString& units) { units_ = units; emit unitsChanged(units_); }
	/// This is used internally to flag whether a Control is labelled isEnum(), and add the names for each state. If it IS a discrete control (enumStateNames != 0), then the tolerance is set to a value less than 1.  This makes sense when the values must be integer (discrete) values.
	virtual void setEnumStates(const QStringList& enumStateNames) { enumNames_ = enumStateNames; if(enumNames_.count() > 0) {setTolerance(0.1);} emit enumChanges(enumNames_); }

	/// Used internally by setStateList, called recursively. \todo MIGHT NEED TO BE VIRTUAL for reimplementation in child classes
	bool searchSetChildren(QMap<QString, double> *controlList, QMap<QString, AMControl*> *executeList, unsigned int errorLevel);

private:
	// subclasses should use the protected methods to access these, to ensure signal generation.

	double tolerance_;
	QString units_;
	QStringList enumNames_;
	QString description_; //Human-readable description
	QString contextKnownDescription_; //Human-readable description. Very short, for when the context is known. Might be "X" as opposed to "SSA Manipulator X"

};

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
	AMReadOnlyPVControl(const QString& name, const QString& readPVname, QObject* parent = 0, const QString decription = "");

	/// \name Reimplemented Public Functions:
	//@{
	/// most recent value of this measurement
	virtual double value() const { return readPV_->lastValue(); }
	/// Indicates that the Control is ready for use.  In this case, means that the readPV is connected and readable.
	virtual bool isConnected() const { return readPV_->readReady(); }
	//virtual bool isConnected() const { return canMeasure(); }
	/// Indicates whether the readPV is readable.
	virtual bool canMeasure() const { return readPV_->canRead(); }
	/// Indicates that we \em should be able to measure from this Control.  Always true.
	virtual bool shouldMeasure() const { return true; }

	/// Minimum value taken from the readPV's lower graphing limit within EPICS.
	virtual double minimumValue() const { return readPV_->lowerGraphicalLimit(); }
	/// Maximum value taken from the readPV's upper graphing limit within EPICS.
	virtual double maximumValue() const { return readPV_->upperGraphicalLimit(); }
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


protected slots:
	/// This is called when reading the PV's control information completes successfully.
	void onReadPVInitialized();

	/// Override this if you want custom handling if the readPV fails to connect.

	/// You can also monitor the readConnectionTimeoutOccurred() signal.
	void onConnectionTimeout() { setUnits("?"); emit connected(false); emit error(AMControl::CannotConnectError); }

	/// This is called when a PV channel connects or disconnects
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
	AMPVControl(const QString& name,
			const QString& readPVname,
			const QString& writePVname,
			const QString& stopPVname = QString(),
			QObject* parent = 0,
			double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
			double completionTimeoutSeconds = 10.0,
			int stopValue = 1,
			const QString &description = "");

	/// \name Reimplemented Public Functions:
	//@{
	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool isMoving() const{ return moveInProgress(); }
	/// Indicates that a move (that you requested) is currently completing... hasn't reached destination, and hasn't time'd out.
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// Implies that we can read from the feedback PV and write to the setpoint PV.
	//virtual bool isConnected() const { return canMeasure() && canMove(); }
	virtual bool isConnected() const { return readPV_->readReady() && writePV_->writeReady(); }
	/// Indicates that we can currently write to the setpoint PV.
	virtual bool canMove() const { return writePV_->canWrite(); }
	/// This Control class has the theoretical ability to move. Always true.
	virtual bool shouldMove() const { return true; }
	/// This Control class can currently stop() if it can write to the stop PV.
	virtual bool canStop() const { return !noStopPV_ && stopPV_->canWrite(); }
	/// This Control class has the theoretical ability to stop, if a stopPVname has been provided.
	virtual bool shouldStop() const { return !noStopPV_; }

	/// Minimum allowed value derived from the writePV's DRV_LOW field, as defined by EPICS
	virtual double minimumValue() const { return writePV_->lowerControlLimit(); }
	/// Maximum allowed value derived from the writePV's DRV_HIGH field, as defined by EPICS
	virtual double maximumValue() const { return writePV_->upperControlLimit(); }

	/// The target value for the setpoint PV
	virtual double setpoint() const { return setpoint_; }
	//@}

	/// \name Additional public functions:
	//@{
	/// The EPICS channel-access name of the writePV
	QString writePVName() const { return writePV_->pvName(); }
	/// The value of the writePV. This will match setpoint() unless someone else (another program or person in the facility) is changing the setpoint.
	double writePVValue() const { return writePV_->lastValue(); }
	/// Read-only access to the writePV.  Using this to change the writePVs value by connecting to its slots is not allowed/not supported.
	const AMProcessVariable* writePV() const { return writePV_; }
	/// Returns the number of seconds allowed for a move() to reach its target setpoint().
	double completionTimeout() const { return completionTimeout_; }
	/// Switches the writePV to using ca_put instead of ca_put_callback.  This seems to be necessary when using some of the more exotic record types such as the mca record type.  This is set to false by default.
	void disableWritePVPutCallback(bool disable) { writePV_->disablePutCallbackMode(disable); }
	//@}

public slots:

	/// Start a move to the value setpoint (reimplemented)
	virtual void move(double setpoint);

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

	/// used for change-detection of the connection state:
	bool wasConnected_;

	/// true if no stopPVname was provided... Means we can't stop(), and shouldStop() and canStop() are false.
	bool noStopPV_;
	/// The value written to the stopPV_ when attempting to stop().
	int stopValue_;

protected slots:

	/// (overridden) Handle a connection timeout from either the readPV_ or writePV_
	/*! The units come from the readPV, so if it's out, we don't know what the units are.
		In any case, if either one doesn't connected, we're not connected.*/
	void onConnectionTimeout() { if(sender() == readPV_) { setUnits("?"); } emit connected(false); emit error(AMControl::CannotConnectError); }

	/// This is called when a PV channel (read or write) connects or disconnects
	void onPVConnected(bool connected);

	/// This is called when there is a Write PV channel error:
	void onWritePVError(int errorCode);

	/// This is used to handle the timeout of a move
	void onCompletionTimeout();

	/// This is used to check every new value, to see if we entered tolerance
	void onNewFeedbackValue(double val);


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
	/// Returns true if the \c statusValue indicates the control is moving, and false if it's not.
	virtual bool operator()(quint32 statusValue) = 0;
};

/// This function object provides the default "isMoving" check for AMReadOnlyPVwStatusControl.  It will compare the current status value against the value provided in the constructor, and return true if equal.
class AMControlStatusCheckerDefault : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compared to \c isMovingValue, and return true if the status value is equal to isMovingValue.
	AMControlStatusCheckerDefault(quint32 isMovingValue) : isMovingValue_(isMovingValue) {}

	/// Returns true (moving) if the \c statusValue matches isMovingValue_;
	virtual bool operator()(quint32 statusValue) { return statusValue == isMovingValue_; }

protected:
	quint32 isMovingValue_;
};

/// This function object provides the opposite of the default check for AMReadyOnlyPVwStatusControl (instead of providing the one value which represents "moving" you provide the one value which represents "stopped"). It will compare the current status against the value provided in the constuctor, and return true if not equal.
class AMControlStatusCheckerStopped : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compare to \c isStoppedValue, and return true if the status value is not equal to isStoppedValue (something that isn't stopped is moving)
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
	//virtual bool isConnected() const { return canMeasure() && movingPV_->canRead(); }
	virtual bool isConnected() const { return readPV_->readReady() && movingPV_->readReady(); }

	/// The movingPV now provides our moving status. (Masked with isMovingMask and compared to isMovingValue)
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

	/// This is called when a PV channel connects or disconnects
	void onPVConnected(bool connected);

	/// This is called when there is a Status PV channel error:
	void onStatusPVError(int errorCode);

	/// This is called whenever there is an update from the move status PV
	void onMovingChanged(int isMovingValue);

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
	/*! \param name A unique description of this control
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

	/// \name Reimplemented Public Functions:
	//@{
	/// Indicates that all three process variables are ready for action:
	//virtual bool isConnected() const { return canMeasure() && canMove() && movingPV_->canRead(); }
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

	/// Minimum allowed value derived from the writePV's DRV_LOW field, as defined by EPICS
	virtual double minimumValue() const { return writePV_->lowerControlLimit(); }
	/// Maximum allowed value derived from the writePV's DRV_HIGH field, as defined by EPICS
	virtual double maximumValue() const { return writePV_->upperControlLimit(); }

	/// This is the target of the last requested move:
	virtual double setpoint() const { return setpoint_; }
	//@}

	/// \name Additional public functions:
	//@{
	/// The EPICS channel-access name of the setpoint PV
	QString writePVName() const { return writePV_->pvName(); }
	/// The value of the writePV. This will match setpoint() unless someone else (another program or person in the facility) is changing the setpoint.
	double writePVValue() const { return writePV_->lastValue(); }
	/// Read-only access to the writePV.  Using this to change the writePVs value by connecting to its slots is not allowed/not supported.
	const AMProcessVariable* writePV() const { return writePV_; }
	/// The maximum time allowed for the Control to start isMoving() after a move() is issued.
	double moveStartTimeout() { return moveStartTimeout_; }
	/// Switches the writePV to using ca_put instead of ca_put_callback.  This seems to be necessary when using some of the more exotic record types such as the mca record type.  This is set to false by default.
	void disableWritePVPutCallback(bool disable) { writePV_->disablePutCallbackMode(disable); }
	//@}

public slots:

	/// Start a move to the value setpoint (reimplemented)
	virtual void move(double setpoint);

	/// Stop a move in progress (reimplemented)

	/// Tell the motor to stop.  (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
	virtual bool stop();

	/// set the completion timeout:
	void setMoveStartTimeout(double seconds) { moveStartTimeout_ = seconds; }

signals:
	/// These are specialized to report on the writePV's channel connection status.  You should be free to ignore them and use the signals defined in AMControl.
	void writeConnectionTimeoutOccurred();
	/// Signals changes in writePVValue().
	/*! Normally we expect that only this program is changing the setpoint and causing motion, using move().  If someone else changes the writePV (setpoint PV), this will signal you with the new value.*/
	void setpointChanged(double);

protected:
	/// This PV is used for the setpoint:
	AMProcessVariable* writePV_;
	/// Used for stopping
	AMProcessVariable* stopPV_;

	/// Used to detect moveStart timeouts:
	QTimer moveStartTimer_;
	/// Used to detect moveStart timeouts:
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

protected slots:

	/// This is called when there is a Status PV channel error:
	void onWritePVError(int errorCode);

	/// This is used to handle the timeout of a move start:
	void onMoveStartTimeout();

	/// This is used to add our own move tracking signals when isMoving() changes:
	void onIsMovingChanged(bool);



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
	AMReadOnlyWaveformBinningPVControl(const QString& name,
					   const QString& readPVname,
					   int lowIndex = 0,
					   int highIndex = 1,
					   QObject* parent = 0,
					   const QString &description = "");

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


protected slots:
	/// This is called when the read PV has new values
	void onReadPVValueChanged();

protected:
	int lowIndex_;
	int highIndex_;
};


// End of doxygen group: control
/**
 @}
 */
#endif // CONTROL_H
