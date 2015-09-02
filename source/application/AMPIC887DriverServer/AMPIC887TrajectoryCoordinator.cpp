#include "AMPIC887TrajectoryCoordinator.h"
#include "AMPIC887Controller.h"
#include <QDebug>
AMPIC887TrajectoryCoordinator::AMPIC887TrajectoryCoordinator(const AMPIC887AxisMap<double>& initialTargets,
															 QObject *parent) :
    QObject(parent)
{
	lastSetTargetPositions_ = initialTargets;
	connectedOnce_ = false;

	xIsSet_ = false;
	yIsSet_ = false;
	zIsSet_ = false;
	uIsSet_ = false;
	vIsSet_ = false;
	wIsSet_ = false;

	xAxisTrajectorySetpoint_ = new AMSinglePVControl("HexapodTrajectoryXSetpoint", "HXPD1611-4-I10-01:trajectory:X:mm", this, AXIS_POSITION_TOLERANCE);
	yAxisTrajectorySetpoint_ = new AMSinglePVControl("HexapodTrajectoryYSetpoint", "HXPD1611-4-I10-01:trajectory:Y:mm", this, AXIS_POSITION_TOLERANCE);
	zAxisTrajectorySetpoint_ = new AMSinglePVControl("HexapodTrajectoryZSetpoint", "HXPD1611-4-I10-01:trajectory:Z:mm", this, AXIS_POSITION_TOLERANCE);
	uAxisTrajectorySetpoint_ = new AMSinglePVControl("HexapodTrajectoryUSetpoint", "HXPD1611-4-I10-01:trajectory:U:deg", this, AXIS_POSITION_TOLERANCE);
	vAxisTrajectorySetpoint_ = new AMSinglePVControl("HexapodTrajectoryVSetpoint", "HXPD1611-4-I10-01:trajectory:V:deg", this, AXIS_POSITION_TOLERANCE);
	wAxisTrajectorySetpoint_ = new AMSinglePVControl("HexapodTrajectoryWSetpoint", "HXPD1611-4-I10-01:trajectory:W:deg", this, AXIS_POSITION_TOLERANCE);
	trajectoryStartMove_ = new AMSinglePVControl("HexapodTrajectoryStart", "HXPD1611-4-I10-01:trajectory:start", this, 0.5);
	trajectoryReset_ = new AMSinglePVControl("HexapodTrajectoryReset", "HXPD1611-4-I10-01:trajectory:reset", this, 0.5);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(xAxisTrajectorySetpoint_);
	allControls_->addControl(yAxisTrajectorySetpoint_);
	allControls_->addControl(zAxisTrajectorySetpoint_);
	allControls_->addControl(uAxisTrajectorySetpoint_);
	allControls_->addControl(vAxisTrajectorySetpoint_);
	allControls_->addControl(wAxisTrajectorySetpoint_);
	allControls_->addControl(trajectoryStartMove_);
	allControls_->addControl(trajectoryReset_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllConnected(bool)));
}

void AMPIC887TrajectoryCoordinator::setLastSetTargetPosition(const AMPIC887AxisMap<double> &targetPositions)
{
	lastSetTargetPositions_ = targetPositions;
}

AMPIC887AxisMap<double> AMPIC887TrajectoryCoordinator::currentTrajectory() const
{
	AMPIC887AxisMap<double> returnTrajectory;

	if(xIsSet_) {
		returnTrajectory.insert(AMGCS2::XAxis, xAxisTrajectorySetpoint_->value());
	}

	if(yIsSet_) {
		returnTrajectory.insert(AMGCS2::YAxis, yAxisTrajectorySetpoint_->value());
	}

	if(zIsSet_) {
		returnTrajectory.insert(AMGCS2::ZAxis, zAxisTrajectorySetpoint_->value());
	}

	if(uIsSet_) {
		returnTrajectory.insert(AMGCS2::UAxis, uAxisTrajectorySetpoint_->value());
	}

	if(vIsSet_) {
		returnTrajectory.insert(AMGCS2::VAxis, vAxisTrajectorySetpoint_->value());
	}

	if(wIsSet_) {
		returnTrajectory.insert(AMGCS2::WAxis, wAxisTrajectorySetpoint_->value());
	}

	return returnTrajectory;
}

void AMPIC887TrajectoryCoordinator::onAllConnected(bool connectedState)
{
	if(connectedState) {
		connectedOnce_ = true;

		qDebug() << "Initializing x trajectory setpoint to " << lastSetTargetPositions_.value(AMGCS2::XAxis);
		if(!xAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::XAxis))) {
			xAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::XAxis));
		}

		qDebug() << "Initializing y trajectory setpoint to " << lastSetTargetPositions_.value(AMGCS2::YAxis);
		if(!yAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::YAxis))) {
			yAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::YAxis));
		}

		qDebug() << "Initializing z trajectory setpoint to " << lastSetTargetPositions_.value(AMGCS2::ZAxis);
		if(!zAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::ZAxis))) {
			zAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::ZAxis));
		}

		qDebug() << "Initializing u trajectory setpoint to " << lastSetTargetPositions_.value(AMGCS2::UAxis);
		if(!uAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::UAxis))) {
			uAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::UAxis));
		}

		qDebug() << "Initializing v trajectory setpoint to " << lastSetTargetPositions_.value(AMGCS2::VAxis);
		if(!vAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::VAxis))) {
			vAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::VAxis));
		}

		qDebug() << "Initializing w trajectory setpoint to " << lastSetTargetPositions_.value(AMGCS2::WAxis);
		if(!wAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::WAxis))) {
			wAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::WAxis));
		}

		connect(xAxisTrajectorySetpoint_, SIGNAL(valueChanged(double)),
				this, SLOT(onXAxisSetpointChanged(double)));
		connect(yAxisTrajectorySetpoint_, SIGNAL(valueChanged(double)),
				this, SLOT(onYAxisSetpointChanged(double)));
		connect(zAxisTrajectorySetpoint_, SIGNAL(valueChanged(double)),
				this, SLOT(onZAxisSetpointChanged(double)));
		connect(uAxisTrajectorySetpoint_, SIGNAL(valueChanged(double)),
				this, SLOT(onUAxisSetpointChanged(double)));
		connect(vAxisTrajectorySetpoint_, SIGNAL(valueChanged(double)),
				this, SLOT(onVAxisSetpointChanged(double)));
		connect(wAxisTrajectorySetpoint_, SIGNAL(valueChanged(double)),
				this, SLOT(onWAxisSetpointChanged(double)));
		connect(trajectoryStartMove_, SIGNAL(valueChanged(double)),
				this, SLOT(onStartChanged(double)));
		connect(trajectoryReset_, SIGNAL(valueChanged(double)),
				this, SLOT(onResetChanged(double)));
	}
}

void AMPIC887TrajectoryCoordinator::onXAxisSetpointChanged(double setpoint)
{
	qDebug() << "X Axis trajectory set to " << setpoint;
	xIsSet_ = true;

}

void AMPIC887TrajectoryCoordinator::onYAxisSetpointChanged(double setpoint)
{
	qDebug() << "Y Axis trajectory set to " << setpoint;
	yIsSet_ = true;

}

void AMPIC887TrajectoryCoordinator::onZAxisSetpointChanged(double setpoint)
{
	qDebug() << "Z Axis trajectory set to " << setpoint;
	zIsSet_ = true;

}

void AMPIC887TrajectoryCoordinator::onUAxisSetpointChanged(double setpoint)
{

	qDebug() << "U Axis trajectory set to " << setpoint;
	uIsSet_ = true;

}

void AMPIC887TrajectoryCoordinator::onVAxisSetpointChanged(double setpoint)
{
	qDebug() << "V Axis trajectory set to " << setpoint;
	vIsSet_ = true;

}

void AMPIC887TrajectoryCoordinator::onWAxisSetpointChanged(double setpoint)
{
	qDebug() << "W Axis trajectory set to " << setpoint;
	wIsSet_ = true;

}

void AMPIC887TrajectoryCoordinator::onStartChanged(double /*setpoint*/)
{
	if(trajectoryStartMove_->withinTolerance(1.0)) {
		qDebug() << "Trajectory motion started";
		emit startTrajectoryMotion();

		xIsSet_ = false;
		yIsSet_ = false;
		zIsSet_ = false;
		uIsSet_ = false;
		vIsSet_ = false;
		wIsSet_ = false;

		trajectoryStartMove_->move(0.0);
	}

	if(!trajectoryStartMove_->withinTolerance(0.0)) {
		trajectoryStartMove_->move(0.0);
	}
}

void AMPIC887TrajectoryCoordinator::onResetChanged(double /*setpoint*/)
{
	if(trajectoryReset_->withinTolerance(1.0)) {
		qDebug() << "Resetting trajectory positions to: ";
		qDebug() << lastSetTargetPositions_.toString("Value");
		xIsSet_ = false;
		yIsSet_ = false;
		zIsSet_ = false;
		uIsSet_ = false;
		vIsSet_ = false;
		wIsSet_ = false;

		if(!xAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::XAxis))) {
			xAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::XAxis));
		}

		if(!yAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::YAxis))) {
			yAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::YAxis));
		}

		if(!zAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::ZAxis))) {
			zAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::ZAxis));
		}

		if(!uAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::UAxis))) {
			uAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::UAxis));
		}

		if(!vAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::VAxis))) {
			vAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::VAxis));
		}

		if(!wAxisTrajectorySetpoint_->withinTolerance(lastSetTargetPositions_.value(AMGCS2::WAxis))) {
			wAxisTrajectorySetpoint_->move(lastSetTargetPositions_.value(AMGCS2::WAxis));
		}

		trajectoryReset_->move(0.0);
	}

	if(!trajectoryReset_->withinTolerance(0.0)) {
		trajectoryReset_->move(0.0);
	}
}

