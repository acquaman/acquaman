#include "BioXASM2Mirror.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"

BioXASM2Mirror::BioXASM2Mirror(QObject *parent) :
    QObject(parent)
{
	// Initialize class variables.
	// Subclasses should create instances of each control.

	isConnected_ = false;

	screen_ = 0;
	verticalUpstreamInb_ = 0;
	verticalUpstreamOutb_ = 0;
	verticalDownstream_ = 0;
	stripeSelect_ = 0;
	yaw_ = 0;
	benderUpstream_ = 0;
	benderDownstream_ = 0;

	pseudoRoll_ = 0;
	pseudoPitch_ = 0;
	pseudoYaw_ = 0;
	pseudoHeight_ = 0;
	pseudoLateral_ = 0;
}

BioXASM2Mirror::~BioXASM2Mirror()
{

}

void BioXASM2Mirror::setScreenPosition(Screen::Position newPosition)
{
	if (screen_ && screen_->isConnected() && newPosition != Screen::Invalid) {
		screen_->move(newPosition);
	}
}

void BioXASM2Mirror::setConnected(bool isConnected)
{
	if (isConnected_ != isConnected) {
		isConnected_ = isConnected;
		emit connected(isConnected_);
	}
}

void BioXASM2Mirror::onConnectedChanged()
{
	bool screenOK = (screen_ && screen_->isConnected());
	bool verticalUpstreamInbOK = (verticalUpstreamInb_ && verticalUpstreamInb_->isConnected());
	bool verticalUpstreamOutbOK = (verticalUpstreamOutb_ && verticalUpstreamOutb_->isConnected());
	bool verticalDownstreamOK = (verticalDownstream_ && verticalDownstream_->isConnected());
	bool stripeSelectOK = (stripeSelect_ && stripeSelect_->isConnected());
	bool yawOK = (yaw_ && yaw_->isConnected());
	bool benderUpstreamOK = (benderUpstream_ && benderUpstream_->isConnected());
	bool benderDownstreamOK = (benderDownstream_ && benderDownstream_->isConnected());

	bool result = (
				screenOK &&
				verticalUpstreamInbOK &&
				verticalUpstreamOutbOK &&
				verticalDownstreamOK &&
				stripeSelectOK &&
				yawOK &&
				benderUpstreamOK &&
				benderDownstreamOK &&
				pseudoRoll_ &&
				pseudoPitch_ &&
				pseudoYaw_ &&
				pseudoHeight_ &&
				pseudoLateral_
				);

	setConnected(result);
}


