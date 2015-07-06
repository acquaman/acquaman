#ifndef AMPIC887HEXAPODSTATE_H
#define AMPIC887HEXAPODSTATE_H

#include "AMPIC887AxisState.h"
#include "AMGCS2.h"
#include <QString>
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
	  * The state of the contained axis with the provided designation.
	  * \param axisDesignation ~ The designation of the axis whose state will
	  * be returned.
	  * \returns The state of the axis with the provided designation, or 0 if
	  * the UnknownAxis is provided.
	  */
	AMPIC887AxisState* axisState(AMGCS2::Axis axisDesignation);

	/*!
	  * The state of the x axis.
	  */
	AMPIC887AxisState* xAxisState() const;

	/*!
	  * The state of the y axis.
	  */
	AMPIC887AxisState* yAxisState() const;

	/*!
	  * The state of the z axis.
	  */
	AMPIC887AxisState* zAxisState() const;

	/*!
	  * The state of the u axis.
	  */
	AMPIC887AxisState* uAxisState() const;

	/*!
	  * The state of the v axis.
	  */
	AMPIC887AxisState* vAxisState() const;

	/*!
	  * The state of the w axis.
	  */
	AMPIC887AxisState* wAxisState() const;

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
	  * The value of motion velocity stored within this hexapod state.
	  */
	double velocity() const;

	/*!
	  * Sets the value of the velocity stored within this hexapod state.
	  * \param velocity ~ The new velocity value to store.
	  */
	void setVelocity(double velocity);

	/*!
	  * Whether all the data for this hexapod state has been initialized. To fully
	  * initialize the data it is necessary to call initialize() and to initialize
	  * all of the contained axis states.
	  */
	bool isAllInitialized() const;

	/*!
	  * Initializes the data for this hexapod state.
	  * NOTE: This does not initialize the data for the contained axis states. In
	  * order for isAllInitialized() to return true, all the axis states must also
	  * be initialized with data.
	  */
	void initialize(bool isInServoMode, double velocity);

	/*!
	  * A status string displaying the current data stored in this hexapod state.
	  */
	QString statusString() const;
protected:
	bool isInitialized_;
	bool isInServoMode_;
	double velocity_;

	AMPIC887AxisState* xAxisState_;
	AMPIC887AxisState* yAxisState_;
	AMPIC887AxisState* zAxisState_;
	AMPIC887AxisState* uAxisState_;
	AMPIC887AxisState* vAxisState_;
	AMPIC887AxisState* wAxisState_;
};

#endif // AMPIC887HEXAPODSTATE_H
