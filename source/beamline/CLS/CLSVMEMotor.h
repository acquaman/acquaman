#ifndef CLSVMEMOTOR_H
#define CLSVMEMOTOR_H

#include "beamline/AMControl.h"

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
	  \param tolerance The tolerance that will be used in the Acquaman framework to help determine position values
	  \param moveStartTimeout How long the motor has to start moving before something is considered amis
	  \param parent QObject parent class
	  */
	CLSVMEMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// Indicates that all process variables for this motor are connected
	virtual bool isConnected() const;

	/// Returns the velocity setting for the velocity profile
	double velocity() const;
	/// Returns the base velocity setting for the velocity profile
	double baseVelocity() const;
	/// Returns the acceleration setting for the velocity profile
	double acceleration() const;
	/// Returns the current velocity of the motor (zero when not moving, presumably non-zero when in motion)
	double currentVelocity() const;

	/// Returns whether the kill PV is being used for stops
	bool usingKill() const;

public slots:
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

protected:
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
	/// PV for the encoder feedback in the engineering units
	AMProcessVariable *encoderReadbackPV_;
	/// PV for the readback on the position (used if no encoder present)
	AMProcessVariable *positionReadbackPV_;

	/// Toggle boolean for which stop PV to use (kill stops the motor IMMEDIATELY, while stop follows the velocity profile)
	bool usingKill_;
};

#endif // CLSVMEMOTOR_H
