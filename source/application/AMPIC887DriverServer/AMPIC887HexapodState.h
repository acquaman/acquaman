#ifndef AMPIC887HEXAPODSTATE_H
#define AMPIC887HEXAPODSTATE_H

#include "AMPIC887AxisState.h"
#include "AMGCS2.h"
#include <QString>
#include <QFlags>
#include "AMPIC887AxisMap.h"
/*!
  * A class which holds the state of the hexapod being controlled by a PI C887.11
  * controller.
  */
class AMPIC887HexapodState
{
public:
	/*!
	  * Creates an instance of an uninitialized hexapod state with no data in any
	  * of its axes.
	  */
    AMPIC887HexapodState();

	/*!
	  * Frees the resources contained within this hexapod state.
	  */
	~AMPIC887HexapodState();

	/*!
	  * The servo mode state stored within this hexapod state.
	  */
	bool isInServoMode() const;

	/*!
	  * Sets the servo mode state to store.
	  * \param isInServoMode ~ The servo mode state to store.
	  */
	void setIsInServoMode(bool isInServoMode);

	/*!
	  * The cycle time value stored within this hexapod state.
	  */
	double cycleTime() const;

	/*!
	  * Sets the cycle time value to store.
	  * \param cycleTime ~ The cycle time value to store.
	  */
	void setCycleTime(double cycleTime);

	/*!
	  * The value of motion velocity stored within this hexapod state.
	  */
	double velocity() const;

	/*!
	  * Sets the value of the velocity stored within this hexapod state.
	  * \param velocity ~ The new velocity value to store.
	  */
	void setVelocity(double velocity);

	/*!
	  * Whether all the data for this hexapod state has been initialized.
	  */
	bool isInitialized() const;

	/*!
	  * Initializes the data for this hexapod state.
	  * NOTE: This does not initialize the data for the contained axis states. In
	  * order for isAllInitialized() to return true, all the axis states must also
	  * be initialized with data.
	  * \param isInServoMode ~ Whether the axes of the hexapod are in servo mode.
	  * \param cycleTime ~ The cycle time for the hexapod.
	  * \param velocity ~ The current axis velocity.
	  */
	void initialize(bool isInServoMode, double cycleTime, double velocity,
					AMPIC887AxisMap<bool> referencedStates,
					AMPIC887AxisMap<double> currentPositions,
					AMPIC887AxisMap<double> lowSoftLimits,
					AMPIC887AxisMap<double> highSoftLimits,
					AMPIC887AxisMap<bool> softLimitStates,
					AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits,
					AMPIC887AxisMap<double> stepSizes,
					AMPIC887AxisMap<double> minCommandablePositions,
					AMPIC887AxisMap<double> maxCommandablePositions,
					AMPIC887AxisMap<double> pivotPoints);

	/*!
	  * A status string displaying the current data stored in this hexapod state.
	  */
	QString statusString() const;

	/*!
	  * Whether the referenced state of all the axes within this state show their
	  * isReferenced state as true.
	  */
	bool areAllAxesReferenced() const;

	bool referencedState(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<bool> referencedStates() const;

	void setReferencedState(AMGCS2::Axis axis, bool referencedState);

	double currentPosition(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> currentPositions() const;

	void setCurrentPosition(AMGCS2::Axis axis, double currentPosition);

	double targetPosition(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> targetPositions() const;

	void setTargetPosition(AMGCS2::Axis axis, double targetPosition);

	double lowSoftLimit(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> lowSoftLimits() const;

	void setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit);

	double highSoftLimit(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> highSoftLimits() const;

	void setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit);

	bool softLimitState(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<bool> softLimitStates() const;

	void setSoftLimitState(AMGCS2::Axis axis, double softLimitState);

	AMGCS2::PositionUnits positionUnits(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits();

	double stepSize(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> stepSizes() const;

	void setStepSize(AMGCS2::Axis axis, double stepSize);

	double minCommandablePosition(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> minCommandablePositions() const;

	double maxCommandablePosition(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> maxCommandablePositions() const;

	double pivotPoint(AMGCS2::Axis axis) const;

	const AMPIC887AxisMap<double> pivotPoints() const;

	void setPivotPoint(AMGCS2::Axis axis, double pivotPoint);
protected:
	bool isInitialized_;
	bool isInServoMode_;
	double cycleTime_;
	double velocity_;

	AMPIC887AxisMap<bool> referencedStates_;
	AMPIC887AxisMap<double> currentPositions_;
	AMPIC887AxisMap<double> targetPositions_;
	AMPIC887AxisMap<double> lowSoftLimits_;
	AMPIC887AxisMap<double> highSoftLimits_;
	AMPIC887AxisMap<bool> softLimitStates_;
	AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits_;
	AMPIC887AxisMap<double> stepSizes_;
	AMPIC887AxisMap<double> minCommandablePositions_;
	AMPIC887AxisMap<double> maxCommandablePositions_;
	AMPIC887AxisMap<double> pivotPoints_;
};

#endif // AMPIC887HEXAPODSTATE_H
