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

double AMPIC887HexapodState::cycleTime() const
{
	return cycleTime_;
}

void AMPIC887HexapodState::setCycleTime(double cycleTime)
{
	cycleTime_ = cycleTime;
}

double AMPIC887HexapodState::velocity() const
{
	return velocity_;
}

void AMPIC887HexapodState::setVelocity(double velocity)
{
	velocity_ = velocity;
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

void AMPIC887HexapodState::initialize(bool isInServoMode, double cycleTime, double velocity)
{
	isInServoMode_ = isInServoMode;
	cycleTime_ = cycleTime;
	velocity_ = velocity;

	isInitialized_ = true;
}

QString AMPIC887HexapodState::statusString() const
{
	QString servoModeString;
	if(isInServoMode_) {
		servoModeString = "Yes";
	} else {
		servoModeString = "No";
	}
	QString stateString = QString("Hexapod:\nVelocity: %1 mm/s \nServo Mode:%2\n\n")
			.arg(velocity_)
			.arg(servoModeString);


	stateString.append(QString("%1\n\n%2\n\n%3\n\n%4\n\n%5\n\n%6")
			.arg(xAxisState_->statusString())
			.arg(yAxisState_->statusString())
			.arg(zAxisState_->statusString())
			.arg(uAxisState_->statusString())
			.arg(vAxisState_->statusString())
			.arg(wAxisState_->statusString()));

	return stateString;
}

bool AMPIC887HexapodState::areAllAxesReferenced() const
{
	return xAxisState_->isReferenced() &&
			yAxisState_->isReferenced() &&
			zAxisState_->isReferenced() &&
			uAxisState_->isReferenced() &&
			vAxisState_->isReferenced() &&
			wAxisState_->isReferenced();
}
