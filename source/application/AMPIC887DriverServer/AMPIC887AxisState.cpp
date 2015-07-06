#include "AMPIC887AxisState.h"

AMPIC887AxisState::AMPIC887AxisState(AMGCS2::Axis axisDesignation)
{
	isInitialized_ = false;

	axisDesignation_ = axisDesignation_;
	isReferenced_ = false;
	currentPosition_ = 0;
	targetPosition_ = 0;
	lowSoftLimit_ = 0;
	highSoftLimit_ = 0;
	areSoftLimitsActive_ = false;
	positionUnits_ = AMGCS2::UnknownPositionUnit;
	stepSize_ = 0;
	maxCommandablePosition_ = 0;
	minCommandablePosition_ = 0;

	pivotPoint_ = 0;

}

void AMPIC887AxisState::initialize(bool isReferenced,
								   double currentPosition,
								   double lowSoftLimit,
								   double highSoftLimit,
								   bool areSoftLimitsActive,
								   AMGCS2::PositionUnits positionUnits,
								   double stepSize,
								   double maxCommandablePosition,
								   double minCommandablePosition,
								   double pivotPoint)
{
	isReferenced_ = isReferenced;
	currentPosition_ = currentPosition;
	targetPosition_ = currentPosition; // As we shouldn't be moving
	lowSoftLimit_ = lowSoftLimit;
	highSoftLimit_ = highSoftLimit;
	areSoftLimitsActive_ = areSoftLimitsActive;
	positionUnits_ = positionUnits;
	stepSize_ = stepSize;
	maxCommandablePosition_ = maxCommandablePosition;
	minCommandablePosition_ = minCommandablePosition;
	pivotPoint_ = pivotPoint;

	isInitialized_ = true;
}

bool AMPIC887AxisState::isInitialized() const
{
	return isInitialized_;
}

bool AMPIC887AxisState::isReferenced() const
{
	return isReferenced_;
}

void AMPIC887AxisState::setIsReferenced(bool isReferenced)
{
	isReferenced_ = isReferenced;
}

double AMPIC887AxisState::currentPosition() const
{
	return currentPosition_;
}

void AMPIC887AxisState::setCurrentPosition(double currentPosition)
{
	currentPosition_ = currentPosition;
}

double AMPIC887AxisState::targetPosition() const
{
	return targetPosition_;
}

void AMPIC887AxisState::setTargetPosition(double targetPosition)
{
	targetPosition_ = targetPosition;
}

double AMPIC887AxisState::lowSoftLimit() const
{
	return lowSoftLimit_;
}

void AMPIC887AxisState::setLowSoftLimit(double lowSoftLimit)
{
	lowSoftLimit_ = lowSoftLimit;
}

double AMPIC887AxisState::highSoftLimit() const
{
	return highSoftLimit_;
}

void AMPIC887AxisState::setHighSoftLimit(double highSoftLimit)
{
	highSoftLimit_ = highSoftLimit;
}

bool AMPIC887AxisState::areSoftLimitsActive() const
{
	return areSoftLimitsActive_;
}

void AMPIC887AxisState::setAreSoftLimitsActive(bool areSoftLimitsActive)
{
	areSoftLimitsActive_ = areSoftLimitsActive;
}

AMGCS2::PositionUnits AMPIC887AxisState::positionUnits() const
{
	return positionUnits_;
}

double AMPIC887AxisState::stepSize() const
{
	return stepSize_;
}

void AMPIC887AxisState::setStepSize(double stepSize)
{
	stepSize_ = stepSize;
}

double AMPIC887AxisState::maxCommandablePosition() const
{
	return maxCommandablePosition_;
}

double AMPIC887AxisState::minCommandablePosition() const
{
	return minCommandablePosition_;
}

double AMPIC887AxisState::pivotPoint() const
{
	return pivotPoint_;
}

void AMPIC887AxisState::setPivotPoint(double pivotPoint)
{
	pivotPoint_ = pivotPoint;
}
