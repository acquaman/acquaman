#ifndef AMPIC887HEXAPODSTATE_H
#define AMPIC887HEXAPODSTATE_H

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
	  * \param servoMode ~ The servo mode state to store.
	  */
	void setServoMode(bool servoMode);

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
	  * Initializes the data for this hexapod state.
	  * NOTE: This does not initialize the data for the contained axis states. In
	  * order for isAllInitialized() to return true, all the axis states must also
	  * be initialized with data.
	  * \param servoMode ~ Whether the axes of the hexapod are in servo mode.
	  * \param cycleTime ~ The cycle time for the hexapod.
	  * \param velocity ~ The current axis velocity.
	  */
	void initialize(bool servoMode, double cycleTime, double velocity,
					const AMPIC887AxisMap<bool>& referencedStates,
					const AMPIC887AxisMap<double>& currentPositions,
					const AMPIC887AxisMap<double>& lowSoftLimits,
					const AMPIC887AxisMap<double>& highSoftLimits,
					const AMPIC887AxisMap<bool>& softLimitStates,
					const AMPIC887AxisMap<bool>& limitSwitchStates,
					const AMPIC887AxisMap<AMGCS2::PositionUnits>& positionUnits,
					const AMPIC887AxisMap<double>& stepSizes,
					const AMPIC887AxisMap<double>& minCommandablePositions,
					const AMPIC887AxisMap<double>& maxCommandablePositions,
					const AMPIC887AxisMap<double>& pivotPoints);

	/*!
	  * Whether the data contained within this hexapod state is valid for safe
	  * operation of a hexapod.
	  */
	bool isDataValid() const;

	/*!
	  * A status string displaying the current data stored in this hexapod state.
	  */
	QString statusString() const;

	/*!
	  * Whether the referenced state of all the axes within this state show their
	  * isReferenced state as true.
	  */
	bool areAllAxesReferenced() const;

	/*!
	  * The referenced state stored for the provided axis.
	  * \param axis ~ The axis whose stored reference state is to be returned.
	  */
	bool referencedState(AMGCS2::Axis axis) const;

	/*!
	  * The reference state stored for all axes.
	  */
	const AMPIC887AxisMap<bool> referencedStates() const;

	/*!
	  * Sets the referenced state stored for the provided axis.
	  * \param axis ~ The axis whose referenced state is to be set.
	  * \param referencedState ~ The value to which the referenced state will
	  * be set.
	  */
	void setReferencedState(AMGCS2::Axis axis, bool referencedState);

	/*!
	  * Sets the referenced states stored based on the provided map.
	  * \param referencedStates ~ The map of referenced states with which this
	  * state's referenced state will be updated.
	  */
	void setReferencedStates(const AMPIC887AxisMap<bool>& referencedStates);

	/*!
	  * The current position value stored for the provided axis.
	  * \param axis ~ The axis whose stored current position value is to be
	  * returned.
	  */
	double currentPosition(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the axis current position values stored.
	  */
	const AMPIC887AxisMap<double> currentPositions() const;

	/*!
	  * Sets the current position value stored for the provided axis.
	  * \param axis ~ The axis whose current position value is to be stored.
	  * \param currentPosition ~ The current position which will be stored for
	  * the axis.
	  */
	void setCurrentPosition(AMGCS2::Axis axis, double currentPosition);

	/*!
	  * Sets the current positions stored based on the provided map.
	  * \param currentPositions ~ The map of current positions with which this
	  * state's current positions will be updated.
	  */
	void setCurrentPositions(const AMPIC887AxisMap<double>& currentPositions);

	/*!
	  * The target position value stored for the provided axis.
	  * \param axis ~ The axis whose target position value is to be returned.
	  */
	double targetPosition(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the axis target position values stored.
	  */
	const AMPIC887AxisMap<double> targetPositions() const;

	/*!
	  * Sets the target position value stored for the provided axis.
	  * \param axis ~ The axis whose target position value is to be set.
	  * \param targetPosition ~ The value which is to be stored for the axis.
	  */
	void setTargetPosition(AMGCS2::Axis axis, double targetPosition);

	/*!
	  * Sets the target positions stored based on the provided map.
	  * \param targetPositions ~ The map of target positions with which this state's
	  * target positions will be updated.
	  */
	void setTargetPositions(const AMPIC887AxisMap<double> targetPositions);

	/*!
	  * The low soft limit value stored for the provided axis.
	  * \param axis ~ The axis whose low soft limit value is to be returned.
	  */
	double lowSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the low soft limit values stored.
	  */
	const AMPIC887AxisMap<double> lowSoftLimits() const;

	/*!
	  * Sets the stored value for the low soft limit of the provided axis.
	  * \param axis ~ The axis whose stored low soft limit value is to be altered.
	  * \param lowSoftLimit ~ The low soft limit value to be stored for the axis.
	  */
	void setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit);

	/*!
	  * Sets the low soft limits stored based on the provided map.
	  * \param lowSoftLimits ~ The map of low soft limits with which this state's
	  * low soft limits will be updated.
	  */
	void setLowSoftLimits(const AMPIC887AxisMap<double>& lowSoftLimits);

	/*!
	  * The high soft limit value stored for the provided axis.
	  * \param axis ~ The axis whose high soft limit value is to be returned.
	  */
	double highSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the high soft limit values stored.
	  */
	const AMPIC887AxisMap<double> highSoftLimits() const;

	/*!
	  * Sets the stored value for the high soft limit of the provided axis.
	  * \param axis ~ The axis whose stored high soft limit value is to be altered.
	  * \param highSoftLimit ~ The high soft limit value to be stored for the axis.
	  */
	void setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit);

	/*!
	  * Sets the high soft limits stored based on the provided map.
	  * \param highSoftLimits ~ The map of high soft limits with which this state's
	  * high soft limits will be updated.
	  */
	void setHighSoftLimits(const AMPIC887AxisMap<double>& highSoftLimits);

	/*!
	  * The soft limit state stored for the provided axis.
	  * \param axis ~ The axis whose stored low soft limit state is to be returned.
	  */
	bool softLimitState(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the soft limit states stored.
	  */
	const AMPIC887AxisMap<bool> softLimitStates() const;

	/*!
	  * Sets the stored low soft limit state of the provided axis to the provided
	  * value.
	  * \param axis ~ The axis whose low soft limit state is to be altered.
	  * \param softLimitState ~ The soft limit state to be stored for the axis.
	  */
	void setSoftLimitState(AMGCS2::Axis axis, bool softLimitState);

	/*!
	  * Sets the soft limit states stored based on the provided map.
	  * \param softLimitStates ~ The map of soft limits states with which this
	  * state's soft limit states will be updated.
	  */
	void setSoftLimitStates(const AMPIC887AxisMap<bool>& softLimitStates);

	/*!
	  * The limit switch state value stored for the provided axis.
	  * \param axis ~ The axis whose limit switch state value is to be returned.
	  */
	bool limitSwitchState(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the stored limit switch states stored.
	  */
	const AMPIC887AxisMap<bool> limitSwitchStates() const;

	/*!
	  * The position units stored for the provided axis.
	  * \param axis ~ The axis whose position units are to be returned.
	  */
	AMGCS2::PositionUnits positionUnits(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the position units stored.
	  */
	const AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits();

	/*!
	  * The step size stored for the provided axis.
	  * \axis ~ The axis whose step size is to be returned.
	  */
	double stepSize(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the step sizes stored.
	  */
	const AMPIC887AxisMap<double> stepSizes() const;

	/*!
	  * Sets the step size value stored for the provided axis.
	  * axis ~ The axis whose stored steps size value is to be altered.
	  * stepSize ~ The step size value to be stored.
	  */
	void setStepSize(AMGCS2::Axis axis, double stepSize);

	/*!
	  * Sets the step sizes stored based on the provided map.
	  * \param stepSizes ~ The map of step sizes with which this state's step
	  * sizes will be updated.
	  */
	void setStepSizes(const AMPIC887AxisMap<double>& stepSizes);

	/*!
	  * The min commandable position stored for the provided axis.
	  * \param axis ~ The axis whose stored min commandable position is to be
	  * returned.
	  */
	double minCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the min commandable positions stored.
	  */
	const AMPIC887AxisMap<double> minCommandablePositions() const;

	/*!
	  * The max commandable position stored for the provided axis.
	  * \param axis ~ The axis whose stored max commandable position is to be
	  * returned.
	  */
	double maxCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the max commandable positions stored.
	  */
	const AMPIC887AxisMap<double> maxCommandablePositions() const;

	/*!
	  * The pivot point stored for the provided axis.
	  * \param axis ~ The axis whose pivot point value is to be stored.
	  */
	double pivotPoint(AMGCS2::Axis axis) const;

	/*!
	  * A map of all the pivot point values stored.
	  */
	const AMPIC887AxisMap<double> pivotPoints() const;

	/*!
	  * Sets the pivot point value stored for the provided axis.
	  * \param axis ~ The axis whose stored pivot point value is to be altered.
	  * \param pivotPoint ~ The pivot point value to be stored for the axis.
	  */
	void setPivotPoint(AMGCS2::Axis axis, double pivotPoint);

	/*!
	  * Sets the pivot points stored based on the provided map.
	  * \param pivotPoints ~ The map of pivot points with which this state's pivot
	  * points will be updated.
	  */
	void setPivotPoints(const AMPIC887AxisMap<double>& pivotPoints);

protected:
	bool servoMode_;
	double cycleTime_;
	double velocity_;

	AMPIC887AxisMap<bool> referencedStates_;
	AMPIC887AxisMap<double> currentPositions_;
	AMPIC887AxisMap<double> targetPositions_;
	AMPIC887AxisMap<double> lowSoftLimits_;
	AMPIC887AxisMap<double> highSoftLimits_;
	AMPIC887AxisMap<bool> softLimitStates_;
	AMPIC887AxisMap<bool> limitSwitchStates_;
	AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits_;
	AMPIC887AxisMap<double> stepSizes_;
	AMPIC887AxisMap<double> minCommandablePositions_;
	AMPIC887AxisMap<double> maxCommandablePositions_;
	AMPIC887AxisMap<double> pivotPoints_;
};

#endif // AMPIC887HEXAPODSTATE_H
