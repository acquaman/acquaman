#include "BioXASMirror.h"

BioXASMirror::BioXASMirror(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize member variables.

	upstreamInboard_ = 0;
	upstreamOutboard_ = 0;
	downstream_ = 0;
	stripeSelect_ = 0;
	yaw_ = 0;
	benderUpstream_ = 0;
	benderDownstream_ = 0;

	pitch_ = 0;
	roll_ = 0;
	height_ = 0;
}

BioXASMirror::~BioXASMirror()
{

}

bool BioXASMirror::isConnected() const
{
	bool isConnected = (
				upstreamInboard_ && upstreamInboard_->isConnected() &&
				upstreamOutboard_ && upstreamOutboard_->isConnected() &&
				downstream_ && downstream_->isConnected() &&
				stripeSelect_ && stripeSelect_->isConnected() &&
				yaw_ && yaw_->isConnected() &&
				benderUpstream_ && benderUpstream_->isConnected() &&
				benderDownstream_ && benderDownstream_->isConnected() &&

				pitch_ && pitch_->isConnected() &&
				roll_ && roll_->isConnected() &&
				height_ && height_->isConnected()
				);

	return isConnected;
}
