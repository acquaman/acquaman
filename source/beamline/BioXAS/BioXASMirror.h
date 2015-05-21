#ifndef BIOXASMIRROR_H
#define BIOXASMIRROR_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"
#include "beamline/BioXAS/BioXASMirrorPitchControl.h"
#include "beamline/BioXAS/BioXASMirrorRollControl.h"

class BioXASMirror : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMirror();

	/// Returns the upstream inboard motor control.
	AMControl* upstreamInboardControl() const { return upstreamInboard_; }
	/// Returns the upstream outboard motor control.
	AMControl* upstreamOutboardControl() const { return upstreamOutboard_; }
	/// Returns the downstream motor control.
	AMControl* downstreamControl() const { return downstream_; }
	/// Returns the stripe selection control.
	AMControl* stripeSelectControl() const { return stripeSelect_; }
	/// Returns the yaw control.
	AMControl* yawControl() const { return yaw_; }
	/// Returns the bender upstream control.
	AMControl* benderUpstreamControl() const { return benderUpstream_; }
	/// Returns the bender downstream control.
	AMControl* benderDownstreamControl() const { return benderDownstream_; }

	/// Returns the pitch control.
	AMControl* pitchControl() const { return pitch_; }
	/// Returns the roll control.
	AMControl* rollControl() const { return roll_; }

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The upstream inboard motor control.
	BioXASMirrorMotor *upstreamInboard_;
	/// The upstream outboard motor control.
	BioXASMirrorMotor *upstreamOutboard_;
	/// The downstream motor control.
	BioXASMirrorMotor *downstream_;
	/// The stripe selection motor control.
	CLSMAXvMotor *stripeSelect_;
	/// The yaw motor control.
	CLSMAXvMotor *yaw_;
	/// The bender upstream motor control.
	CLSMAXvMotor *benderUpstream_;
	/// The bender downstream motor control.
	CLSMAXvMotor *benderDownstream_;

	/// The pitch pseudomotor control.
	BioXASMirrorPitchControl *pitch_;
	/// The roll pseudomotor control.
	BioXASMirrorRollControl *roll_;
};

#endif // BIOXASMIRROR_H
