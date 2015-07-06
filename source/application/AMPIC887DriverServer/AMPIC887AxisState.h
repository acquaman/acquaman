#ifndef AMPIC887AXISSTATE_H
#define AMPIC887AXISSTATE_H

#include "AMGCS2.h"
#include <QString>
/*!
  * A class which holds the state of an individual axis of the hexapod being controlled
  * by a PI C887.11 controller.
  *
  * NOTE: This class acts as a simple data store which is used by the AMPIC887ControllerStatus
  * class to hold information in a consistent way. It performs no communication
  * with the controller. I.e. calling setCurrentPosition(value) will not command
  * this axis to move to the provided position, and currentPosition() will not
  * neccessarily return the current position of the axis if the controller does
  * not refresh it when it knows a move is taking place.
  */
class AMPIC887AxisState
{
public:
	/*!
	  * Creates an uninitialized axis state which currently stores no data and
	  * is designated as the UnknownAxis.
	  * \param axisDesignation ~ The designation for the axis.
	  */
	AMPIC887AxisState(AMGCS2::Axis axisDesignation);

	/*!
	  * Initializes the data for this axis state. Also sets the isInitialized flag
	  * to true. Makes the assumption that the axis is not in a state of movement
	  * at the time the initialization is performed.
	  * \param isReferenced ~ Whether a reference move has been successfully performed
	  * on this axis.
	  * \param currentPosition ~ The current position of this axis (will also be
	  * used as the targetPosition as the axis should not be moving).
	  * \param lowSoftLimit ~ The lowest position this axis can take when areSoftLimitsActive
	  * is true.
	  * \param highSoftLimit ~ The highest position this axis can take when areSoftLimitsActive
	  * is true.
	  * \param areSoftLimitsActive ~ Whether the low and high soft limits are currently
	  * limiting the motions of the axis.
	  * \param positionUnits ~ The units the position of this axis is measured in.
	  * \param stepSize ~ The step size of the motions of this axis.
	  * \param maxCommandablePosition ~ The highest position this axis can take.
	  * \param minCommandablePosition ~ The lowest position this axis can take.
	  * \param pivotPoint ~ The pivot point within this axis around which rotations
	  * take place. NOTE: Only valid for X, Y and Z axes. Defaults to 0.
	  */
	void initialize(bool isReferenced,
					double currentPosition,
					double lowSoftLimit,
					double highSoftLimit,
					bool areSoftLimitsActive,
					AMGCS2::PositionUnits positionUnits,
					double stepSize,
					double maxCommandablePosition,
					double minCommandablePosition,
					double pivotPoint = 0);

	/*!
	  * The designation identifier for the axis.
	  */
	AMGCS2::Axis axisDesignation() const;

	/*!
	  * Whether the state of this axis has been initialized with data.
	  */
	bool isInitialized() const;

	/*!
	  * The current referenced state stored within this axis state.
	  */
	bool isReferenced() const;

	/*!
	  * Sets the current referenced state of this axis.
	  * \param isReferenced ~ The new referenced state to store.
	  */
	void setIsReferenced(bool isReferenced);

	/*!
	  * The current position stored within this axis state.
	  */
	double currentPosition() const;

	/*!
	  * Sets the current position value stored within this axis state.
	  * \param currentPosition ~ The new value to store.
	  */
	void setCurrentPosition(double currentPosition);

	/*!
	  * The target position stored within this axis state.
	  */
	double targetPosition() const;

	/*!
	  * Sets the target position value stored within this axis state.
	  * \param targetPosition ~ The new target position to store.
	  */
	void setTargetPosition(double targetPosition);

	/*!
	  * The low soft limit value stored within this axis state.
	  */
	double lowSoftLimit() const;

	/*!
	  * Sets the low soft limit value stored within this axis state.
	  * \param lowSoftLimit ~ The new low soft limit to store.
	  */
	void setLowSoftLimit(double lowSoftLimit);

	/*!
	  * The high soft limit value stored within this axis state.
	  */
	double highSoftLimit() const;

	/*!
	  * Sets the high soft limit value stored within this axis state.
	  * \param highSoftLimit ~ The new high soft limit to store.
	  */
	void setHighSoftLimit(double highSoftLimit);

	/*!
	  * Sets whether this axis state is storing the soft limits as being active.
	  */
	bool areSoftLimitsActive() const;

	/*!
	  * Sets the soft limit states within this axis state.
	  * \param areSoftLimitsActive ~ The new active state of the soft limits to
	  * store.
	  */
	void setAreSoftLimitsActive(bool areSoftLimitsActive);

	/*!
	  * The position units stored within this axis state.
	  */
	AMGCS2::PositionUnits positionUnits() const;

	/*!
	  * The value of the step size stored within this axis state.
	  */
	double stepSize() const;

	/*!
	  * Sets the value of the step size stored within this axis state.
	  * \param stepSize ~ The new step size value to store.
	  */
	void setStepSize(double stepSize);

	/*!
	  * The max commandable position value stored within this axis state.
	  */
	double maxCommandablePosition() const;

	/*!
	  * The min commandable position value stored within this axis state.
	  */
	double minCommandablePosition() const;

	/*!
	  * The pivot point value stored within this axis state.
	  */
	double pivotPoint() const;

	/*!
	  * Sets the value of the pivot point stored within this axis state.
	  * \param pivotPoint ~ The new pivot point value to store.
	  */
	void setPivotPoint(double pivotPoint);

	/*!
	  * A status string describing the current state of the data stored in this
	  * axis state.
	  */
	QString statusString() const;
protected:
	bool isInitialized_;

	AMGCS2::Axis axisDesignation_;
	bool isReferenced_;
	double currentPosition_;
	double targetPosition_;
	double lowSoftLimit_;
	double highSoftLimit_;
	bool areSoftLimitsActive_;
	AMGCS2::PositionUnits positionUnits_;
	double stepSize_;
	double maxCommandablePosition_;
	double minCommandablePosition_;
	double pivotPoint_;
};

#endif // AMPIC887AXISSTATE_H
