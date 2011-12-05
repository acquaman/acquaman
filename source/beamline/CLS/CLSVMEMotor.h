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


#ifndef CLSVMEMOTOR_H
#define CLSVMEMOTOR_H

#include "beamline/AMControl.h"

/// This function object provides the moving check for the CLSVMEMotors
class AMControlStatusCheckerCLSVME : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compare to \c isStoppedValue, and return true if the status value is not equal to isStoppedValue (something that isn't stopped is moving)
	AMControlStatusCheckerCLSVME() {}

	/// Return true (moving) if the \c statusValue is not 0 (STOPPED) and is not 3 (FORCED STOP) and is not 4 (ERROR)
	virtual bool operator()(quint32 statusValue) { return (statusValue != 0) && (statusValue != 3) && (statusValue != 4); }
};

/*!
  This class encapsulates many of the standard components found in a CLS VME motor.  Unlike standard AMControls, the CLS VME motors can set other things such as the velocity, acceleration, or directly access the steps
  rather then giving a distance in millimeters.  Therefore, this class is meant to give more control over the motors in the event that you want more control over the actual
  motor.
  */
class CLSVMEMotor : public AMPVwStatusControl
{
	Q_OBJECT

public:
	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param baseName The base of the PV name (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this motor
	  \param hasEncoder Should be set to true if the motor has an encoder
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success
	  \param moveStartTimeout How long the motor has to start moving before something is considered amis
	  \param parent QObject parent class
	  */
	CLSVMEMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// Indicates that all process variables for this motor are connected
	virtual bool isConnected() const;

	/// Returns the step setting.  Returns 0.5 if the motor isn't connected yet.  This is because steps are actually given in integers.
	double step() const;
	/// Returns the velocity setting for the velocity profile.  Returns 0.5 if the motor isn't connected yet.  This is because velocity is actually given in integers.
	double velocity() const;
	/// Returns the base velocity setting for the velocity profile.  Returns 0.5 if the motor isn't connected yet.  This is because baseVelocity is actually given in integers.
	double baseVelocity() const;
	/// Returns the acceleration setting for the velocity profile.  Returns 0.5 if the motor isn't connected yet.  This is because acceleration is actually given in integers.
	double acceleration() const;
	/// Returns the current velocity of the motor (zero when not moving, presumably non-zero when in motion).  Returns 0.5 if the motor isn't connected yet.  This is because currentVelocity is actually given in integers.
	double currentVelocity() const;

	/// Returns whether the kill PV is being used for stops
	bool usingKill() const;

public slots:
	/// Sets the step setting.  NOTE: This will move the motor based on the value you give it.  Similar to \code move(double).
	void setStep(double step);
	/// Sets the velocity setting for the velocity profile
	void setVelocity(double velocity);
	/// Sets the base velocity setting for the velocity profile
	void setBaseVelocity(double baseVelocity);
	/// Sets the acceleration setting for the velocity profile
	void setAcceleration(double acceleration);

	/// Sets whether the kill PV is being used for stops (if false, then the regular stop PV is used)
	void setUsingKill(bool usingKill);

	/// Tell the motor to stop. If usingKill is true, then the kill PV will be used otherwise the stop PV will be used (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
	virtual bool stop();

signals:
	/// Emitted when the step setting changes.
	void stepChanged(double step);
	/// Emitted when the velocity setting changes
	void velocityChanged(double velocity);
	/// Emitted when the base velocity setting changes
	void baseVelocityChanged(double baseVelocity);
	/// Emitted when the acceleration setting changes
	void accelerationChanged(double acceleration);
	/// Emitted when the current velocity reading changes
	void currentVelocityChanged(double currentVelocity);

protected:
	/// Read-write control for the step setting.
	AMPVControl *step_;
	/// Read-write control for the velocity setting
	AMPVControl *velocity_;
	/// Read-write control for base velocity setting
	AMPVControl *baseVelocity_;
	/// Read-write control for acceleration setting
	AMPVControl *acceleration_;
	/// Readonly control for actual velocity feedback
	AMReadOnlyPVControl *currentVelocity_;

	/// PV for the "kill" functionality
	AMProcessVariable *killPV_;

	/// Toggle boolean for which stop PV to use (kill stops the motor IMMEDIATELY, while stop follows the velocity profile)
	bool usingKill_;
};

#endif // CLSVMEMOTOR_H
