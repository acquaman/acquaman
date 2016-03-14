#ifndef SGMGRATINGANGLECONTROL_H
#define SGMGRATINGANGLECONTROL_H

#define DEFAULT_GRATING_ANGLE_VELOCITY 12000
#define DEFAULT_GRATING_ANGLE_ACCELERATION  5000
#define DEFAULT_GRATING_ANGLE_MOVE_TYPE 3 // Increase closed loop mode

#define SGM_GRATING_MAX_CONTINUOUS_STEP_VELOCITY 20000
#define SGM_GRATING_MIN_CONTINUOUS_STEP_VELOCITY 300

#include "actions3/AMAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPseudoMotorControl.h"
#include "beamline/SGM/energy/SGMGratingSupport.h"
class AMRange;
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
	  * Whether the control is set to close loop mode.
	  */
	bool isClosedLoop() const;

	/*!
	  * The velocity control for the grating (in steps/s)
	  */
	AMControl* stepVelocityControl() const;

	/*!
	  * The acceleration control for the grating (in steps/s/s)
	  */
	AMControl* stepAccelerationControl() const;

	/*!
	  * The control which enumerates the different movement types for the grating
	  * angle.
	  *  [0] None
	  *  [1] Increase Only
	  *  [2] Decrease Only
	  *  [3] Increase
	  *  [4] Decrease
	  */
	AMControl* movementTypeControl() const;

	/*!
	  * The conversion ratio between each encoder count and the number of steps.
	  */
	double stepsPerEncoderCount() const;

	/*!
	  * Creates an action which returns the movement type, velocity and acceleration
	  * to their default values.
	  */
	AMAction3* createDefaultsAction();

	/*!
	  * Utility method which returns the min and max time bounds for a coordinated
	  * move on a given grating.
	  */
	AMRange timeBoundsForEnergyMove(double startEnergy,
	                                double endEnergy,
	                                SGMGratingSupport::GratingTranslation currentTranslation);
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
	AMControl* stepMotorControl_;
	AMControl* stepVelocityControl_;
	AMControl* stepAccelerationControl_;
	AMControl* stepsPerEncoderCountControl_;
	AMControl* movementTypeControl_;
};

#endif // SGMGRATINGANGLECONTROL_H
