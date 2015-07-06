#include "AMPIC887ControllerState.h"

AMPIC887ControllerState::AMPIC887ControllerState()
{
	hexapodState_ = new AMPIC887HexapodState();
}

AMPIC887ControllerState::~AMPIC887ControllerState()
{
	delete hexapodState_;
}

AMPIC887HexapodState * AMPIC887ControllerState::hexapodState() const
{
	return hexapodState_;
}

bool AMPIC887ControllerState::isAllInitialized() const
{
	return hexapodState_->isAllInitialized();
}
