#ifndef AMPIC887TRAJECTORYCOORDINATOR_H
#define AMPIC887TRAJECTORYCOORDINATOR_H

#include <QObject>
#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "AMPIC887AxisMap.h"

/*!
  * A class which represents an adapter between the driver application and the pvs
  * which relate to the Trajectory motions of the hexapod.
  * These PVs are used in such a way that they are each set one at a time (for each
  * axis that you wish to move) and then the start pv is called to perform the
  * move.
  */
class AMPIC887TrajectoryCoordinator : public QObject
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of a trajectory coordinator which will coordinate with
	  * the trajectory PVs.
	  * \param initialTargets ~ A set of the target positions at the time that
	  * the coordinator is created.
	  */
	explicit AMPIC887TrajectoryCoordinator(const AMPIC887AxisMap<double>& initialTargets,
										   QObject *parent = 0);

	/*!
	  * Virtual destructor for the trajectory coordinator.
	  */
	virtual ~AMPIC887TrajectoryCoordinator() {}

	/*!
	  * Sets the target positions of the axes to which the traectory PVs will be
	  * set when the reset PV is invoked.
	  */
	void setLastSetTargetPosition(const AMPIC887AxisMap<double>& targetPositions);

	/*!
	  * The current set trajectories since the last time the reset or start PVs
	  * were called. If no trajectory positions have been set, an emty map is
	  * returned.
	  */
	AMPIC887AxisMap<double> currentTrajectory() const;
signals:
	/*!
	  * A signal which indicates that a trajectory motion has been requested.
	  */
	void startTrajectoryMotion();
public slots:

protected slots:
	/*!
	  * Handles signals from the control set which indicates that a connection
	  * has been made with all the contained PV controls. Connects the required
	  * PV signals to the member slot functions.
	  * \param connectedState ~ Unused.
	  */
	void onAllConnected(bool connectedState);

	/*!
	  * Handles signals indicating that the X Axis trajectory PV has been changed.
	  * Sets a bit flag which indicates that the x axis is to be included in the
	  * next trajectory move.
	  */
	void onXAxisSetpointChanged(double setpoint);

	/*!
	  * Handles signals indicating that the Y Axis trajectory PV has been changed.
	  * Sets a bit flag which indicates that the y axis is to be included in the
	  * next trajectory move.
	  */
	void onYAxisSetpointChanged(double setpoint);

	/*!
	  * Handles signals indicating that the Z Axis trajectory PV has been changed.
	  * Sets a bit flag which indicates that the z axis is to be included in the
	  * next trajectory move.
	  */
	void onZAxisSetpointChanged(double setpoint);

	/*!
	  * Handles signals indicating that the U Axis trajectory PV has been changed.
	  * Sets a bit flag which indicates that the u axis is to be included in the
	  * next trajectory move.
	  */
	void onUAxisSetpointChanged(double setpoint);

	/*!
	  * Handles signals indicating that the V Axis trajectory PV has been changed.
	  * Sets a bit flag which indicates that the v axis is to be included in the
	  * next trajectory move.
	  */
	void onVAxisSetpointChanged(double setpoint);

	/*!
	  * Handles signals indicating that the W Axis trajectory PV has been changed.
	  * Sets a bit flag which indicates that the w axis is to be included in the
	  * next trajectory move.
	  */
	void onWAxisSetpointChanged(double setpoint);

	/*!
	  * Handles signals indicating that the start PV has been altered.
	  * If the value has been set to 1.0, then a trajectory is constructed based
	  * on the set bit flags, a signal in emitted that a trajectory move should
	  * be started and the bit flags for the axes are reset to false.
	  */
	void onStartChanged(double setpoint);

	/*!
	  * Handles signals indicating that the reset PV has been altered.
	  * If the value has been set to 1.0, then the setpoint values for the axes
	  * are all set to the last set target position.
	  */
	void onResetChanged(double setpoint);
protected:

	AMControlSet* allControls_;

	AMControl* xAxisTrajectorySetpoint_;
	AMControl* yAxisTrajectorySetpoint_;
	AMControl* zAxisTrajectorySetpoint_;
	AMControl* uAxisTrajectorySetpoint_;
	AMControl* vAxisTrajectorySetpoint_;
	AMControl* wAxisTrajectorySetpoint_;
	AMControl* trajectoryStartMove_;
	AMControl* trajectoryReset_;

	// Flags which indicate that a value has been set for each axis in the current
	// trajectory setting.
	bool xIsSet_;
	bool yIsSet_;
	bool zIsSet_;
	bool uIsSet_;
	bool vIsSet_;
	bool wIsSet_;

	bool connectedOnce_;

	AMPIC887AxisMap<double> lastSetTargetPositions_;
};

#endif // AMPIC887TRAJECTORYCOORDINATOR_H
