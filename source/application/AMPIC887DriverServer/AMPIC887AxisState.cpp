#include "AMPIC887AxisState.h"
#include "AMGCS2Support.h"

AMPIC887AxisState::AMPIC887AxisState(AMGCS2::Axis axisDesignation)
{
	isInitialized_ = false;

	axisDesignation_ = axisDesignation;
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

QString AMPIC887AxisState::statusString() const
{
	if(!isInitialized_) {
		return QString("Data not yet initialized for axis %1\n")
				.arg(AMGCS2Support::axisToCharacter(axisDesignation_));
	}

	QString dataString = QString("Data for axis %1:\n")
			.arg(AMGCS2Support::axisToCharacter(axisDesignation_));

	QString unitsString = AMGCS2Support::positionUnitsToString(positionUnits_);
	dataString.append(QString("Is Referenced: %1\nCurrent Position: %2 %3\nTarget Position: %4 %3\nLow Soft Limit: %5 %3\nHigh Soft Limit: %6 %3\nSoft Limits Active: %7\nStep Size: %8 %3\nMax Position: %9 %3\nMin Position: %10 %3\n")
					  .arg(isReferenced_)
					  .arg(currentPosition_)
					  .arg(unitsString)
					  .arg(targetPosition_)
					  .arg(lowSoftLimit_)
					  .arg(highSoftLimit_)
					  .arg(areSoftLimitsActive_)
					  .arg(stepSize_)
					  .arg(maxCommandablePosition_)
					  .arg(minCommandablePosition_));

	if(axisDesignation_ == AMGCS2::XAxis ||
			axisDesignation_ == AMGCS2::YAxis ||
			axisDesignation_ == AMGCS2::ZAxis) {

		dataString.append(QString("Pivot Point: %1 %2\n")
				.arg(pivotPoint_)
				.arg(unitsString));
	}

	return dataString;
}
