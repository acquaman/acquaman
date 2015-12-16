#ifndef BIOXASMIRROR_H
#define BIOXASMIRROR_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"
#include "beamline/BioXAS/BioXASMirrorControl.h"
#include "beamline/BioXAS/BioXASMirrorPitchControl.h"
#include "beamline/BioXAS/BioXASMirrorRollControl.h"
#include "beamline/BioXAS/BioXASMirrorHeightControl.h"
#include "beamline/BioXAS/BioXASMirrorLateralControl.h"
#include "beamline/BioXAS/BioXASMirrorYawControl.h"
#include "beamline/BioXAS/BioXASMirrorBendControl.h"

class BioXASMirror : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirror(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMirror();

	/// Returns true if this control can be stopped, false otherwise. Reimplemented to consider only a subset of children.
	virtual bool canStop() const;

	/// Returns the current connected state. True if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upstream inboard motor control.
	BioXASMirrorMotor* upstreamInboardMotor() const { return upstreamInboardMotor_; }
	/// Returns the upstream outboard motor control.
	BioXASMirrorMotor* upstreamOutboardMotor() const { return upstreamOutboardMotor_; }
	/// Returns the downstream motor control.
	BioXASMirrorMotor* downstreamMotor() const { return downstreamMotor_; }
	/// Returns the stripe selection motor control.
	CLSMAXvMotor* stripeSelectMotor() const { return stripeSelectMotor_; }
	/// Returns the yaw motor control.
	CLSMAXvMotor* yawMotor() const { return yawMotor_; }
	/// Returns the bender upstream motor control.
	CLSMAXvMotor* upstreamBenderMotor() const { return upstreamBenderMotor_; }
	/// Returns the bender downstream motor control.
	CLSMAXvMotor* downstreamBenderMotor() const { return downstreamBenderMotor_; }

	/// Returns the pitch control.
	BioXASMirrorPitchControl* pitch() const { return pitch_; }
	/// Returns the roll control.
	BioXASMirrorRollControl* roll() const { return roll_; }
	/// Returns the height control.
	BioXASMirrorHeightControl* height() const { return height_; }
	/// Returns the lateral control.
	BioXASMirrorLateralControl* lateral() const { return lateral_; }
	/// Returns the yaw control.
	BioXASMirrorYawControl* yaw() const { return yaw_; }
	/// Returns the bend control.
	BioXASMirrorBendControl* bend() const { return bend_; }

	/// Returns the upstream mirror length.
	double upstreamLength() const { return upstreamLength_; }
	/// Returns the downstream mirror length.
	double downstreamLength() const { return downstreamLength_; }

signals:
	/// Notifier that the upstream mirror length has changed.
	void upstreamLengthChanged(double newLength);
	/// Notifier that the downstream mirror length has changed.
	void downstreamLengthChanged(double newLength);
	/// Notifier that the upstream inboard motor control has changed.
	void upstreamInboardMotorChanged(BioXASMirrorMotor *newControl);
	/// Notifier that the upstream outboard motor control has changed.
	void upstreamOutboardMotorChanged(BioXASMirrorMotor *newControl);
	/// Notifier that the downstream motor control has changed.
	void downstreamMotorChanged(BioXASMirrorMotor *newControl);
	/// Notifier that the stripe selection motor control has changed.
	void stripeSelectMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the yaw motor control has changed.
	void yawMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the upstream bender motor control has changed.
	void upstreamBenderMotorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the downstream bender motor control has changed.
	void downstreamBenderMotorChanged(CLSMAXvMotor *newControl);
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

protected slots:
	/// Sets the upstream mirror length.
	void setUpstreamLength(double newLength);
	/// Sets the downstream mirror length.
	void setDownstreamLength(double newLength);

	/// Sets the upstream inboard motor control.
	void setUpstreamInboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the upstream outboard motor control.
	void setUpstreamOutboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the downstream motor control.
	void setDownstreamMotor(BioXASMirrorMotor *newControl);
	/// Sets the stripe select motor control.
	void setStripeSelectMotor(CLSMAXvMotor *newControl);
	/// Sets the yaw motor control.
	void setYawMotor(CLSMAXvMotor *newControl);
	/// Sets the upstream bender motor control.
	void setUpstreamBenderMotor(CLSMAXvMotor *newControl);
	/// Sets the downstream bender motor control.
	void setDownstreamBenderMotor(CLSMAXvMotor *newControl);

	/// Sets the pitch control.
	void setPitch(BioXASMirrorPitchControl *newControl);
	/// Sets the roll control.
	void setRoll(BioXASMirrorRollControl *newControl);
	/// Sets the height control.
	void setHeight(BioXASMirrorHeightControl *newControl);
	/// Sets the lateral control.
	void setLateral(BioXASMirrorLateralControl *newControl);
	/// Sets the yaw control.
	void setYaw(BioXASMirrorYawControl *newControl);
	/// Sets the bend control.
	void setBend(BioXASMirrorBendControl *newControl);

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

private:
	/// The upstream inboard motor control.
	BioXASMirrorMotor *upstreamInboardMotor_;
	/// The upstream outboard motor control.
	BioXASMirrorMotor *upstreamOutboardMotor_;
	/// The downstream motor control.
	BioXASMirrorMotor *downstreamMotor_;
	/// The stripe selection motor control.
	CLSMAXvMotor *stripeSelectMotor_;
	/// The yaw motor control.
	CLSMAXvMotor *yawMotor_;
	/// The bender upstream motor control.
	CLSMAXvMotor *upstreamBenderMotor_;
	/// The bender downstream motor control.
	CLSMAXvMotor *downstreamBenderMotor_;

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

	/// Returns the upstream mirror length.
	double upstreamLength_;
	/// Returns the downstream mirror length.
	double downstreamLength_;
};

#endif // BIOXASMIRROR_H
