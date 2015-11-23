#include "AMPIC887ControllerState.h"

AMPIC887ControllerState::AMPIC887ControllerState()
{
	hexapodState_ = new AMPIC887HexapodState();
	dataRecorderState_ = new AMPIC887DataRecorderState();
	currentCommandLevel_ = AMGCS2::UnknownCommandLevel;
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

void AMPIC887ControllerState::initialize(AMGCS2::ControllerCommandLevel commandLevel,
										 const QString &availableParameters,
										 const QString& identification)
{
	currentCommandLevel_ = commandLevel;
	availableParameters_ = availableParameters;
	identificationString_ = identification;
}

bool AMPIC887ControllerState::isDataValid() const
{
	return currentCommandLevel_ != AMGCS2::UnknownCommandLevel &&
			!availableParameters_.isEmpty() &&
			!identificationString_.isEmpty() &&
			hexapodState_->isDataValid() &&
			dataRecorderState_->isDataValid();
}

QString AMPIC887ControllerState::statusString() const
{
	return QString("%1\n\n%2").arg(hexapodState_->statusString())
			.arg(dataRecorderState_->statusString());
}

QString AMPIC887ControllerState::availableParameters() const
{
	return availableParameters_;
}

QString AMPIC887ControllerState::identificationString() const
{
	return identificationString_;
}

AMGCS2::ControllerCommandLevel AMPIC887ControllerState::currentCommandLevel() const
{
	return currentCommandLevel_;
}

void AMPIC887ControllerState::setCurrentCommandLevel(AMGCS2::ControllerCommandLevel currentCommandLevel)
{
	currentCommandLevel_ = currentCommandLevel;
}
