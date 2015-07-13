#include "AMPIC887HexapodState.h"

AMPIC887HexapodState::AMPIC887HexapodState()
{
	isInServoMode_ = false;
	isInitialized_ = false;
}

AMPIC887HexapodState::~AMPIC887HexapodState()
{

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

bool AMPIC887HexapodState::isInitialized() const
{
	return isInitialized_;
}

void AMPIC887HexapodState::initialize(bool isInServoMode, double cycleTime, double velocity,
									  AMPIC887AxisMap<bool> referencedStates,
									  AMPIC887AxisMap<double> currentPositions,
									  AMPIC887AxisMap<double> lowSoftLimits,
									  AMPIC887AxisMap<double> highSoftLimits,
									  AMPIC887AxisMap<bool> softLimitStates,
									  AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits,
									  AMPIC887AxisMap<double> stepSizes,
									  AMPIC887AxisMap<double> minCommandablePositions,
									  AMPIC887AxisMap<double> maxCommandablePositions,
									  AMPIC887AxisMap<double> pivotPoints)
{
	isInServoMode_ = isInServoMode;
	cycleTime_ = cycleTime;
	velocity_ = velocity;
	referencedStates_ = referencedStates;
	currentPositions_ = currentPositions;
	lowSoftLimits_ = lowSoftLimits;
	highSoftLimits_ = highSoftLimits;
	softLimitStates_ = softLimitStates;
	positionUnits_ = positionUnits;
	stepSizes_ = stepSizes;
	minCommandablePositions_ = minCommandablePositions;
	maxCommandablePositions_ = maxCommandablePositions;
	pivotPoints_ = pivotPoints;

	// Duplicate current positions into target positions
	AMPIC887AxisCollection axes = currentPositions.axes();
	foreach(AMGCS2::Axis currentAxis, axes) {
		targetPositions_.insert(currentAxis, currentPositions_.value(currentAxis));
	}

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


// TODO FIX THIS

	return stateString;
}

bool AMPIC887HexapodState::areAllAxesReferenced() const
{
	return isInitialized_ &&
			referencedStates_.value(AMGCS2::XAxis) &&
			referencedStates_.value(AMGCS2::YAxis) &&
			referencedStates_.value(AMGCS2::ZAxis) &&
			referencedStates_.value(AMGCS2::UAxis) &&
			referencedStates_.value(AMGCS2::VAxis) &&
			referencedStates_.value(AMGCS2::WAxis);
}

bool AMPIC887HexapodState::referencedState(AMGCS2::Axis axis) const
{
	return referencedStates_.value(axis, false);
}

const AMPIC887AxisMap<bool> AMPIC887HexapodState::referencedStates() const
{
	return referencedStates_;
}

void AMPIC887HexapodState::setReferencedState(AMGCS2::Axis axis, bool referencedState)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}

	referencedStates_.insert(axis, referencedState);
}

double AMPIC887HexapodState::currentPosition(AMGCS2::Axis axis) const
{
	return currentPositions_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::currentPositions() const
{
	return currentPositions_;
}

void AMPIC887HexapodState::setCurrentPosition(AMGCS2::Axis axis, double currentPosition)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}

	currentPositions_.insert(axis, currentPosition);
}

double AMPIC887HexapodState::targetPosition(AMGCS2::Axis axis) const
{
	return targetPositions_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::targetPositions() const
{
	return targetPositions_;
}

void AMPIC887HexapodState::setTargetPosition(AMGCS2::Axis axis, double targetPosition)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}

	currentPositions_.insert(axis, targetPosition);
}

double AMPIC887HexapodState::lowSoftLimit(AMGCS2::Axis axis) const
{
	return lowSoftLimits_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::lowSoftLimits() const
{
	return lowSoftLimits_;
}

void AMPIC887HexapodState::setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}
}

double AMPIC887HexapodState::highSoftLimit(AMGCS2::Axis axis) const
{
	return highSoftLimits_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::highSoftLimits() const
{
	return highSoftLimits_;
}

void AMPIC887HexapodState::setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}

	highSoftLimits_.insert(axis, highSoftLimit);
}

bool AMPIC887HexapodState::softLimitState(AMGCS2::Axis axis) const
{
	return softLimitStates_.value(axis, false);
}

const AMPIC887AxisMap<bool> AMPIC887HexapodState::softLimitStates() const
{
	return softLimitStates_;
}

void AMPIC887HexapodState::setSoftLimitState(AMGCS2::Axis axis, double softLimitState)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}

	softLimitStates_.insert(axis, softLimitState);
}

AMGCS2::PositionUnits AMPIC887HexapodState::positionUnits(AMGCS2::Axis axis) const
{
	return positionUnits_.value(axis, AMGCS2::UnknownPositionUnit);
}

const AMPIC887AxisMap<AMGCS2::PositionUnits> AMPIC887HexapodState::positionUnits()
{
	return positionUnits_;
}

double AMPIC887HexapodState::stepSize(AMGCS2::Axis axis) const
{
	return stepSizes_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::stepSizes() const
{
	return stepSizes_;
}

void AMPIC887HexapodState::setStepSize(AMGCS2::Axis axis, double stepSize)
{
	if(axis == AMGCS2::UnknownAxis) {
		return;
	}

	stepSizes_.insert(axis, stepSize);
}

double AMPIC887HexapodState::minCommandablePosition(AMGCS2::Axis axis) const
{
	return minCommandablePositions_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::minCommandablePositions() const
{
	return minCommandablePositions_;
}

double AMPIC887HexapodState::maxCommandablePosition(AMGCS2::Axis axis) const
{
	return maxCommandablePositions_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::maxCommandablePositions() const
{
	return maxCommandablePositions_;
}

double AMPIC887HexapodState::pivotPoint(AMGCS2::Axis axis) const
{
	return pivotPoints_.value(axis, 0);
}

const AMPIC887AxisMap<double> AMPIC887HexapodState::pivotPoints() const
{
	return pivotPoints_;
}

void AMPIC887HexapodState::setPivotPoint(AMGCS2::Axis axis, double pivotPoint)
{
	if(axis == AMGCS2::UnknownAxis ||
			axis == AMGCS2::UAxis ||
			axis == AMGCS2::VAxis ||
			axis == AMGCS2::WAxis) {
		return;
	}

	pivotPoints_.insert(axis, pivotPoint);
}
