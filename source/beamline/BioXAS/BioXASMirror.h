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

	/// Returns the current connected state. True if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upstream inboard motor control.
	AMControl* upstreamInboardMotorControl() const { return upstreamInboardMotor_; }
	/// Returns the upstream outboard motor control.
	AMControl* upstreamOutboardMotorControl() const { return upstreamOutboardMotor_; }
	/// Returns the downstream motor control.
	AMControl* downstreamMotorControl() const { return downstreamMotor_; }
	/// Returns the stripe selection motor control.
	AMControl* stripeSelectMotorControl() const { return stripeSelectMotor_; }
	/// Returns the yaw motor control.
	AMControl* yawMotorControl() const { return yawMotor_; }
	/// Returns the bender upstream motor control.
	AMControl* benderUpstreamMotorControl() const { return benderUpstreamMotor_; }
	/// Returns the bender downstream motor control.
	AMControl* benderDownstreamMotorControl() const { return benderDownstreamMotor_; }

	/// Returns the pitch control.
	AMControl* pitchControl() const { return pitch_; }
	/// Returns the roll control.
	AMControl* rollControl() const { return roll_; }
	/// Returns the height control.
	AMControl* heightControl() const { return height_; }
	/// Returns the lateral control.
	AMControl* lateralControl() const { return lateral_; }
	/// Returns the yaw control.
	AMControl* yawControl() const { return yaw_; }
	/// Returns the bend radius control.
	AMControl* bendControl() const { return bend_; }

	/// Returns the upstream mirror length.
	double upstreamLength() const { return upstreamLength_; }
	/// Returns the downstream mirror length.
	double downstreamLength() const { return downstreamLength_; }

protected:
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
	CLSMAXvMotor *benderUpstreamMotor_;
	/// The bender downstream motor control.
	CLSMAXvMotor *benderDownstreamMotor_;

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
