#ifndef BIOXASMIRROR_H
#define BIOXASMIRROR_H

#include "beamline/BioXAS/BioXASMirrorControl.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"
#include "beamline/BioXAS/BioXASMirrorControl.h"
#include "beamline/BioXAS/BioXASMirrorPitchControl.h"
#include "beamline/BioXAS/BioXASMirrorRollControl.h"
#include "beamline/BioXAS/BioXASMirrorHeightControl.h"
#include "beamline/BioXAS/BioXASMirrorLateralControl.h"
#include "beamline/BioXAS/BioXASMirrorYawControl.h"
#include "beamline/BioXAS/BioXASMirrorBendControl.h"

#define BIOXASMIRROR_NOT_CONNECTED 827018
#define BIOXASMIRROR_CANNOT_MOVE 827019
#define BIOXASMIRROR_ALREADY_MOVING 827020

class BioXASMirror : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirror(const QString &name, QObject *parent = 0, const QString &description = "");
	/// Destructor.
	virtual ~BioXASMirror();

	/// Returns true if this control can be stopped, false otherwise. Reimplemented to consider only a subset of children.
	virtual bool canStop() const;

	/// Returns the current connected state. True if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the pitch value.
	double pitchValue() const;
	/// Returns the roll value.
	double rollValue() const;
	/// Returns the height value.
	double heightValue() const;
	/// Returns the yaw value.
	double yawValue() const;
	/// Returns the lateral value.
	double lateralValue() const;
	/// Returns the bend value.
	double bendValue() const;

	/// Returns the pitch control.
	BioXASMirrorPitchControl* pitch() const { return pitch_; }
	/// Returns the roll control.
	BioXASMirrorRollControl* roll() const { return roll_; }
	/// Returns the height control.
	BioXASMirrorHeightControl* height() const { return height_; }
	/// Returns the yaw control.
	BioXASMirrorYawControl* yaw() const { return yaw_; }
	/// Returns the lateral control.
	BioXASMirrorLateralControl* lateral() const { return lateral_; }
	/// Returns the bend control.
	BioXASMirrorBendControl* bend() const { return bend_; }

signals:
	/// Notifier that the pitch value has changed.
	void pitchValueChanged(double newValue);
	/// Notifier that the roll value has changed.
	void rollValueChanged(double newValue);
	/// Notifier that the height value has changed.
	void heightValueChanged(double newValue);
	/// Notifier that the yaw value has changed.
	void yawValueChanged(double newValue);
	/// Notifier that the lateral value has changed.
	void lateralValueChanged(double newValue);
	/// Notifier that the bend value has changed.
	void bendValueChanged(double newValue);

	/// Notifier that the pitch control has changed.
	void pitchChanged(BioXASMirrorPitchControl *newControl);
	/// Notifier that the roll control has changed.
	void rollChanged(BioXASMirrorRollControl *newControl);
	/// Notifier that the height control has changed.
	void heightChanged(BioXASMirrorHeightControl *newControl);
	/// Notifier that the lateral control has changed.
	void lateralChanged(BioXASMirrorLateralControl *newControl);
	/// Notifier that the yaw control has changed.
	void yawChanged(BioXASMirrorYawControl *newControl);
	/// Notifier that the bend control has changed.
	void bendChanged(BioXASMirrorBendControl *newControl);

public slots:
	/// Sets the upstream mirror length. Reimplemented to update the yaw and lateral controls.
	virtual void setUpstreamLength(double newValue);
	/// Sets the downstream mirror length. Reimplemented to update the yaw and lateral controls.
	virtual void setDownstreamLength(double newValue);

	/// Sets the upstream inboard motor control. Reimplemented to update the pitch, roll, and height controls.
	virtual void setUpstreamInboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the upstream outboard motor control. Reimplemented to update the pitch, roll, and height controls.
	virtual void setUpstreamOutboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the downstream motor control. Reimplemented to update the pitch, roll, and height controls.
	virtual void setDownstreamMotor(BioXASMirrorMotor *newControl);
	/// Sets the yaw motor control. Reimplemented to update the yaw and lateral controls.
	virtual void setYawMotor(AMControl *newControl);
	/// Sets the lateral motor control. Reimplemented to update the yaw and lateral controls.
	virtual void setLateralMotor(AMControl *newControl);
	/// Sets the upstream bender control. Reimplemented to update the bend control.
	virtual void setUpstreamBenderMotor(AMControl *newControl);
	/// Sets the downstream bender control. Reimplemented to update the bend control.
	virtual void setDownstreamBenderMotor(AMControl *newControl);

	/// Sets the pitch control.
	void setPitch(BioXASMirrorPitchControl *newControl);
	/// Sets the roll control.
	void setRoll(BioXASMirrorRollControl *newControl);
	/// Sets the height control.
	void setHeight(BioXASMirrorHeightControl *newControl);
	/// Sets the yaw control.
	void setYaw(BioXASMirrorYawControl *newControl);
	/// Sets the lateral control.
	void setLateral(BioXASMirrorLateralControl *newControl);
	/// Sets the bend control.
	void setBend(BioXASMirrorBendControl *newControl);

	/// Moves the mirror to a new position with the given pitch, roll, height positions.
	virtual AMControl::FailureExplanation moveMirror(double pitch, double roll, double height);
	/// Moves the mirror to a new pitch position.
	virtual AMControl::FailureExplanation moveMirrorPitch(double pitch);
	/// Moves the mirror to a new roll position.
	virtual AMControl::FailureExplanation moveMirrorRoll(double roll);
	/// Moves the mirror to a new height position.
	virtual AMControl::FailureExplanation moveMirrorHeight(double height);

	/// Moves the mirror to a new position with the given yaw and lateral positions.
	virtual AMControl::FailureExplanation moveMirror(double yaw, double lateral);
	/// Moves the mirror to a new yaw position.
	virtual AMControl::FailureExplanation moveMirrorYaw(double yaw);
	/// Moves the mirror to a new lateral position.
	virtual AMControl::FailureExplanation moveMirrorLateral(double lateral);

protected slots:
	/// Updates the pitch control.
	void updatePitch();
	/// Updates the roll control.
	void updateRoll();
	/// Updates the height control.
	void updateHeight();
	/// Updates the lateral control.
	void updateLateral();
	/// Updates the yaw control.
	void updateYaw();
	/// Updates the bend control.
	void updateBend();

protected:
	/// The pitch pseudomotor control.
	BioXASMirrorPitchControl *pitch_;
	/// The roll pseudomotor control.
	BioXASMirrorRollControl *roll_;
	/// The height pseudomotor control.
	BioXASMirrorHeightControl *height_;
	/// The lateral pseudomotor control.
	BioXASMirrorLateralControl *lateral_;
	/// The yaw pseudomotor control.
	BioXASMirrorYawControl *yaw_;
	/// The bend radius pseudomotor control.
	BioXASMirrorBendControl *bend_;
};

#endif // BIOXASMIRROR_H
