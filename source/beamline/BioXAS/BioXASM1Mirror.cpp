#include "BioXASM1Mirror.h"

BioXASM1Mirror::BioXASM1Mirror(QObject *parent) :
	BioXASBeamlineComponent(parent)
{
	verticalUpstreamInb_ = 0;
	verticalUpstreamOutb_ = 0;
	verticalDownstream_ = 0;
	stripeSelect_ = 0;
	yaw_ = 0;
	benderUpstream_ = 0;
	benderDownstream_ = 0;
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

void BioXASM1Mirror::updateConnected()
{
	bool isConnected = (
				verticalUpstreamInb_ && verticalUpstreamInb_->isConnected() &&
				verticalUpstreamOutb_ && verticalUpstreamOutb_->isConnected() &&
				verticalDownstream_ && verticalDownstream_->isConnected() &&
				stripeSelect_ && stripeSelect_->isConnected() &&
				yaw_ && yaw_->isConnected() &&
				benderUpstream_ && benderUpstream_->isConnected() &&
				benderDownstream_ && benderDownstream_->isConnected()
				);

	setConnected(isConnected);
}
