#ifndef SGMUNDULATORCONTROL_H
#define SGMUNDULATORCONTROL_H

#define DEFAULT_UNDULATOR_VELOCITY 5000
#define DEFAULT_UNDULATOR_ACCELERATION  2500

#include "beamline/AMPseudoMotorControl.h"

class SGMUndulatorControl : public AMPseudoMotorControl
{
	Q_OBJECT
public:
	explicit SGMUndulatorControl(QObject *parent = 0);

	/*!
	  * Whether the control is designed to be able to measure values.
	  */
	bool shouldMeasure() const;

	/*!
	  * Whether the control is designed to be able to move.
	  */
	bool shouldMove() const;

	/*!
	  * Whether the control is designed to be able to stop.
	  */
	bool shouldStop() const;

	/*!
	  * Whether the control can measure values in its current state.
	  */
	bool canMeasure() const;

	/*!
	  * Whether the control can move in its current state.
	  */
	bool canMove() const;

	/*!
	  * Whether the control can stop in its current state.
	  */
	bool canStop() const;

	/*!
	  * The velocity control for the grating (in steps/s)
	  */
	AMControl* stepVelocityControl() const;

	/*!
	  * The acceleration control for the grating (in steps/s/s)
	  */
	AMControl* stepAccelerationControl() const;

	/*!
	  * The stepper motor control for the undulator. For motions at a specific
	  * velocity it is necessary to target the undulator step.
	  */
	AMControl* stepControl() const;
signals:
protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();
	/// Updates the connection state when any of the controls connection state changes.
	void onControlSetConnectionChanged(bool isConnected);
protected:
	AMAction3* createMoveAction(double setpoint);

	AMControl* encoderControl_;
	AMControl* stepControl_;
	AMControl* stepVelocityControl_;
	AMControl* stepAccelerationControl_;


};

#endif // SGMUNDULATORCONTROL_H
