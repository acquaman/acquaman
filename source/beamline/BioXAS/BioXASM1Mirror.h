#ifndef BIOXASM1MIRROR_H
#define BIOXASM1MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASM1Mirror : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASM1Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM1Mirror();

	/// Returns the vertical upstream inbound motor control.
	CLSMAXvMotor* verticalUpstreamInboundControl() const { return verticalUpstreamInb_; }
	/// Returns the vertical upstream outbound motor control.
	CLSMAXvMotor* verticalUpstreamOutboundControl() const { return verticalUpstreamOutb_; }
	/// Returns the vertical downstream motor control.
	CLSMAXvMotor* verticalDownstreamControl() const { return verticalDownstream_; }
	/// Returns the stripe selection control.
	CLSMAXvMotor* stripeSelectControl() const { return stripeSelect_; }
	/// Returns the yaw control.
	CLSMAXvMotor* yawControl() const { return yaw_; }
	/// Returns the bender upstream control.
	CLSMAXvMotor* benderUpstreamControl() const { return benderUpstream_; }
	/// Returns the bender downstream control.
	CLSMAXvMotor* benderDownstreamControl() const { return benderDownstream_; }
	/// Returns the upper slit blade control.
	CLSMAXvMotor* upperSlitBladeControl() const { return upperSlitBlade_; }

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The vertical upstream inbound motor control.
	CLSMAXvMotor *verticalUpstreamInb_;
	/// The vertical upstream outbound motor control.
	CLSMAXvMotor *verticalUpstreamOutb_;
	/// The vertical downstream motor control.
	CLSMAXvMotor *verticalDownstream_;
	/// The stripe selection motor control.
	CLSMAXvMotor *stripeSelect_;
	/// The yaw motor control.
	CLSMAXvMotor *yaw_;
	/// The bender upstream motor control.
	CLSMAXvMotor *benderUpstream_;
	/// The bender downstream motor control.
	CLSMAXvMotor *benderDownstream_;
	/// The upper slit blade motor control.
	CLSMAXvMotor *upperSlitBlade_;

};

#endif // BIOXASM1MIRROR_H
