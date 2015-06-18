#include "BioXASMirror.h"

BioXASMirror::BioXASMirror(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize member variables.

	upstreamInboardMotor_ = 0;
	upstreamOutboardMotor_ = 0;
	downstreamMotor_ = 0;
	stripeSelectMotor_ = 0;
	yawMotor_ = 0;
	benderUpstreamMotor_ = 0;
	benderDownstreamMotor_ = 0;

	pitch_ = 0;
	roll_ = 0;
	height_ = 0;
	lateral_ = 0;
	yaw_ = 0;
	bend_ = 0;

	upstreamLength_ = 0;
	downstreamLength_ = 0;
}

BioXASMirror::~BioXASMirror()
{

}

bool BioXASMirror::isConnected() const
{
	bool isConnected = (
				upstreamInboardMotor_ && upstreamInboardMotor_->isConnected() &&
				upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected() &&
				downstreamMotor_ && downstreamMotor_->isConnected() &&
				stripeSelectMotor_ && stripeSelectMotor_->isConnected() &&
				yawMotor_ && yawMotor_->isConnected() &&
				benderUpstreamMotor_ && benderUpstreamMotor_->isConnected() &&
				benderDownstreamMotor_ && benderDownstreamMotor_->isConnected() &&

				pitch_ && pitch_->isConnected() &&
				roll_ && roll_->isConnected() &&
				height_ && height_->isConnected() &&
				lateral_ && lateral_->isConnected() &&
				yaw_ && yaw_->isConnected() &&
				bend_ && bend_->isConnected()
				);

	return isConnected;
}
