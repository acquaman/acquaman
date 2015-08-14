#include "SGMHexapod.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"

SGMHexapod::SGMHexapod(QObject *parent) :
    QObject(parent)
{
	xAxis_ = new AMPVwStatusControl("Hexapod X", "HXPD1611-4-I10-01:X:mm:fbk", "HXPD1611-4-I10-01:X:mm", "HXPD1611-4-I10-01:X:status", "HXPD1611-4-I10-01:stop",this, 0.01, 2.0, new CLSMAXvControlStatusChecker());
	yAxis_ = new AMPVwStatusControl("Hexapod Y", "HXPD1611-4-I10-01:Y:mm:fbk", "HXPD1611-4-I10-01:Y:mm", "HXPD1611-4-I10-01:Y:status", "HXPD1611-4-I10-01:stop",this, 0.01, 2.0, new CLSMAXvControlStatusChecker());
	zAxis_ = new AMPVwStatusControl("Hexapod Z", "HXPD1611-4-I10-01:Z:mm:fbk", "HXPD1611-4-I10-01:Z:mm", "HXPD1611-4-I10-01:Z:status", "HXPD1611-4-I10-01:stop",this, 0.01, 2.0, new CLSMAXvControlStatusChecker());
	uAxis_ = new AMPVwStatusControl("Hexapod U", "HXPD1611-4-I10-01:U:deg:fbk", "HXPD1611-4-I10-01:U:deg", "HXPD1611-4-I10-01:U:status", "HXPD1611-4-I10-01:stop", this, 0.01, 2.0, new CLSMAXvControlStatusChecker());
	vAxis_ = new AMPVwStatusControl("Hexapod V", "HXPD1611-4-I10-01:V:deg:fbk", "HXPD1611-4-I10-01:V:deg", "HXPD1611-4-I10-01:V:status", "HXPD1611-4-I10-01:stop", this, 0.01, 2.0, new CLSMAXvControlStatusChecker());
	wAxis_ = new AMPVwStatusControl("Hexapod W", "HXPD1611-4-I10-01:W:deg:fbk", "HXPD1611-4-I10-01:W:deg", "HXPD1611-4-I10-01:W:status", "HXPD1611-4-I10-01:stop", this, 0.01, 2.0, new CLSMAXvControlStatusChecker());

	systemVelocity_ = new AMPVControl("Hexapod Velocity", "HXPD1611-4-I10-01:velocity:fbk", "HXPD1611-4-I10-01:velocity",QString(), this, 0.001, 2.0);
	stopAll_ = new AMSinglePVControl("Hexapod Stop", "HXPD1611-4-I10-01:stop", this, 0.5);

	xAxisTrajectorySetpoint_ = new AMSinglePVControl("Hexapod X Trajectory Setpoint", "HXPD1611-4-I10-01:trajectory:X:mm", this, 0.01);
	yAxisTrajectorySetpoint_ = new AMSinglePVControl("Hexapod Y Trajectory Setpoint", "HXPD1611-4-I10-01:trajectory:Y:mm", this, 0.01);
	zAxisTrajectorySetpoint_ = new AMSinglePVControl("Hexapod Z Trajectory Setpoint", "HXPD1611-4-I10-01:trajectory:Z:mm", this, 0.01);
	uAxisTrajectorySetpoint_ = new AMSinglePVControl("Hexapod U Trajectory Setpoint", "HXPD1611-4-I10-01:trajectory:U:deg", this, 0.01);
	vAxisTrajectorySetpoint_ = new AMSinglePVControl("Hexapod V Trajectory Setpoint", "HXPD1611-4-I10-01:trajectory:V:deg", this, 0.01);
	wAxisTrajectorySetpoint_ = new AMSinglePVControl("Hexapod W Trajectory Setpoint", "HXPD1611-4-I10-01:trajectory:W:deg", this, 0.01);

	trajectoryStart_ = new AMSinglePVControl("Hexapod Trajectory Move Start", "HXPD1611-4-I10-01:trajectory:start", this, 0.5);
	trajectoryReset_ = new AMSinglePVControl("Hexapod Trajectory Reset", "HXPD1611-4-I10-01:trajectory:reset", this, 0.5);
}

AMControl * SGMHexapod::xAxis() const
{
	return xAxis_;
}

AMControl * SGMHexapod::yAxis() const
{
	return yAxis_;
}

AMControl * SGMHexapod::zAxis() const
{
	return zAxis_;
}

AMControl * SGMHexapod::uAxis() const
{
	return uAxis_;
}

AMControl * SGMHexapod::vAxis() const
{
	return vAxis_;
}

AMControl * SGMHexapod::wAxis() const
{
	return wAxis_;
}

AMControl * SGMHexapod::systemVelocity() const
{
	return systemVelocity_;
}

AMControl * SGMHexapod::stopAll() const
{
	return stopAll_;
}

AMControl * SGMHexapod::xAxisTrajectorySetpoint() const
{
	return xAxisTrajectorySetpoint_;
}

AMControl * SGMHexapod::yAxisTrajectorySetpoint() const
{
	return yAxisTrajectorySetpoint_;
}

AMControl * SGMHexapod::zAxisTrajectorySetpoint() const
{
	return zAxisTrajectorySetpoint_;
}

AMControl * SGMHexapod::uAxisTrajectorySetpoint() const
{
	return uAxisTrajectorySetpoint_;
}

AMControl * SGMHexapod::vAxisTrajectorySetpoint() const
{
	return vAxisTrajectorySetpoint_;
}

AMControl * SGMHexapod::wAxisTrajectorySetpoint() const
{
	return wAxisTrajectorySetpoint_;
}

AMControl * SGMHexapod::trajectoryStart() const
{
	return trajectoryStart_;
}

AMControl * SGMHexapod::trajectoryReset() const
{
	return trajectoryReset_;
}
