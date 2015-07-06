#include "AMPIC887ControllerState.h"

AMPIC887ControllerState::AMPIC887ControllerState()
{
	hexapodState_ = new AMPIC887HexapodState();
	dataRecorderState_ = new AMPIC887DataRecorderState();
}

AMPIC887ControllerState::~AMPIC887ControllerState()
{
	delete hexapodState_;
	delete dataRecorderState_;
}

AMPIC887HexapodState * AMPIC887ControllerState::hexapodState() const
{
	return hexapodState_;
}

AMPIC887DataRecorderState * AMPIC887ControllerState::dataRecorderState() const
{
	return dataRecorderState_;
}

bool AMPIC887ControllerState::isAllInitialized() const
{
	return hexapodState_->isAllInitialized() &&
			dataRecorderState_->isAllInitialized();
}

QString AMPIC887ControllerState::statusString() const
{
	return QString("%1\n\n%2").arg(hexapodState_->statusString())
			.arg(dataRecorderState_->statusString());
}

