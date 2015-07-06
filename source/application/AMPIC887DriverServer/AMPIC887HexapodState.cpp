#include "AMPIC887HexapodState.h"

AMPIC887HexapodState::AMPIC887HexapodState()
{
	xAxisState_ = new AMPIC887AxisState(AMGCS2::XAxis);
	yAxisState_ = new AMPIC887AxisState(AMGCS2::YAxis);
	zAxisState_ = new AMPIC887AxisState(AMGCS2::ZAxis);
	uAxisState_ = new AMPIC887AxisState(AMGCS2::UAxis);
	vAxisState_ = new AMPIC887AxisState(AMGCS2::VAxis);
	wAxisState_ = new AMPIC887AxisState(AMGCS2::WAxis);
	isInServoMode_ = false;
	isInitialized_ = false;
}

AMPIC887HexapodState::~AMPIC887HexapodState()
{
	delete xAxisState_;
	delete yAxisState_;
	delete zAxisState_;
	delete uAxisState_;
	delete vAxisState_;
	delete wAxisState_;
}

AMPIC887AxisState * AMPIC887HexapodState::axisState(AMGCS2::Axis axisDesignation)
{
	switch(axisDesignation) {
	case AMGCS2::XAxis:
		return xAxisState_;
	case AMGCS2::YAxis:
		return yAxisState_;
	case AMGCS2::ZAxis:
		return zAxisState_;
	case AMGCS2::UAxis:
		return uAxisState_;
	case AMGCS2::VAxis:
		return vAxisState_;
	case AMGCS2::WAxis:
		return wAxisState_;
	default:
		return 0;
	}
}

AMPIC887AxisState * AMPIC887HexapodState::xAxisState() const
{
	return xAxisState_;
}

AMPIC887AxisState * AMPIC887HexapodState::yAxisState() const
{
	return yAxisState_;
}

AMPIC887AxisState * AMPIC887HexapodState::zAxisState() const
{
	return zAxisState_;
}

AMPIC887AxisState * AMPIC887HexapodState::uAxisState() const
{
	return uAxisState_;
}

AMPIC887AxisState * AMPIC887HexapodState::vAxisState() const
{
	return vAxisState_;
}

AMPIC887AxisState * AMPIC887HexapodState::wAxisState() const
{
	return wAxisState_;
}

bool AMPIC887HexapodState::isInServoMode() const
{
	return isInServoMode_;
}

void AMPIC887HexapodState::setIsInServoMode(bool isInServoMode)
{
	isInServoMode_ = isInServoMode;
}

bool AMPIC887HexapodState::isAllInitialized() const
{
	return isInitialized_ &&
			xAxisState_->isInitialized() &&
			yAxisState_->isInitialized() &&
			zAxisState_->isInitialized() &&
			uAxisState_->isInitialized() &&
			vAxisState_->isInitialized() &&
			wAxisState_->isInitialized();
}

void AMPIC887HexapodState::initialize(bool isInServoMode, double velocity)
{
	isInServoMode_ = isInServoMode;
	velocity_ = velocity;

	isInitialized_ = true;
}
