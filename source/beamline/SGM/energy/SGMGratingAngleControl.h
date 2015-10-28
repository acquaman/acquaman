#ifndef SGMGRATINGANGLECONTROL_H
#define SGMGRATINGANGLECONTROL_H

#include "beamline/AMPseudoMotorControl.h"
class SGMGratingAngleControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:

    explicit SGMGratingAngleControl(QObject *parent = 0);

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
	  * Whether the control is currently in closed loop mode.
	  */
	bool isClosedLoop() const;

	/*!
	  * The raw, non encoded, step control
	  */
	AMControl* stepControl() const;

	/*!
	  * The velocity control for the grating (in steps/s)
	  */
	AMControl* stepVelocityControl() const;

	/*!
	  * The acceleration control for the grating (in steps/s/s)
	  */
	AMControl* stepAccelerationControl() const;

	/*!
	  * The conversion ratio between each encoder count and the number of steps.
	  */
	double stepsPerEncoderCount() const;
signals:

public slots:
protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();
protected:
	AMAction3* createMoveAction(double setpoint);

	AMControl* gratingAngleEncoderControl_;
	AMControl* gratingAngleStepControl_;
	AMControl* gratingAngleStepVelocityControl_;
	AMControl* gratingAngleStepAccelerationControl_;
	AMControl* gratingAngleStepsPerEncoderCountControl_;
	AMControl* gratingAngleMoveType_;
	bool isClosedLoop_;
};

#endif // SGMGRATINGANGLECONTROL_H
