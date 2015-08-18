#include "AMPIC887EpicsCoordinator.h"
#include "AMPIC887Controller.h"
#include <QDebug>
#include <QTimer>

AMPIC887EpicsCoordinator::AMPIC887EpicsCoordinator(AMPIC887Controller* controller, QObject *parent) :
    QObject(parent)
{
	connectedOnce_ = false;
	controller_ = controller;
	allControls_ = new AMControlSet(this);
	trajectoryCoordinator_ = new AMPIC887TrajectoryCoordinator(controller_->targetPositions(AMPIC887AxisCollection()), this);

	xAxisFeedback_ = new AMSinglePVControl("HexapodXAxisValue", "HXPD1611-4-I10-01:X:mm:fbk", this, AXIS_POSITION_TOLERANCE);
	xAxisSetpoint_ = new AMSinglePVControl("HexapodXAxisSetpoint", "HXPD1611-4-I10-01:X:mm", this, AXIS_POSITION_TOLERANCE);
	xAxisStatus_ = new AMSinglePVControl("HexapodXAxisStatus", "HXPD1611-4-I10-01:X:status", this, 0.5);
	xAxisDriveHigh_ = new AMSinglePVControl("HexapodXAxisDriveHigh", "HXPD1611-4-I10-01:X:mm.DRVH", this, AXIS_POSITION_TOLERANCE);
	xAxisDriveLow_ = new AMSinglePVControl("HexapodXAxisDriveLow", "HXPD1611-4-I10-01:X:mm.DRVL", this, AXIS_POSITION_TOLERANCE);

	yAxisFeedback_ = new AMSinglePVControl("HexapodYAxisValue", "HXPD1611-4-I10-01:Y:mm:fbk", this, AXIS_POSITION_TOLERANCE);
	yAxisSetpoint_ = new AMSinglePVControl("HexapodYAxisSetpoint", "HXPD1611-4-I10-01:Y:mm", this, AXIS_POSITION_TOLERANCE);
	yAxisStatus_ = new AMSinglePVControl("HexapodYAxisStatus", "HXPD1611-4-I10-01:Y:status", this, 0.5);
	yAxisDriveHigh_ = new AMSinglePVControl("HexapodYAxisDriveHigh", "HXPD1611-4-I10-01:Y:mm.DRVH", this, AXIS_POSITION_TOLERANCE);
	yAxisDriveLow_ = new AMSinglePVControl("HexapodYAxisDriveLow", "HXPD1611-4-I10-01:Y:mm.DRVL", this, AXIS_POSITION_TOLERANCE);

	zAxisFeedback_ = new AMSinglePVControl("HexapodZAxisValue", "HXPD1611-4-I10-01:Z:mm:fbk", this, AXIS_POSITION_TOLERANCE);
	zAxisSetpoint_ = new AMSinglePVControl("HexapodZAxisSetpoint", "HXPD1611-4-I10-01:Z:mm", this, AXIS_POSITION_TOLERANCE);
	zAxisStatus_ = new AMSinglePVControl("HexapodZAxisStatus", "HXPD1611-4-I10-01:Z:status", this, 0.5);
	zAxisDriveHigh_ = new AMSinglePVControl("HexapodZAxisDriveHigh", "HXPD1611-4-I10-01:Z:mm.DRVH", this, AXIS_POSITION_TOLERANCE);
	zAxisDriveLow_ = new AMSinglePVControl("HexapodZAxisDriveLow", "HXPD1611-4-I10-01:Z:mm.DRVL", this, AXIS_POSITION_TOLERANCE);

	uAxisFeedback_ = new AMSinglePVControl("HexapodUAxisValue", "HXPD1611-4-I10-01:U:deg:fbk", this, AXIS_POSITION_TOLERANCE);
	uAxisSetpoint_ = new AMSinglePVControl("HexapodUAxisSetpoint", "HXPD1611-4-I10-01:U:deg", this, AXIS_POSITION_TOLERANCE);
	uAxisStatus_ = new AMSinglePVControl("HexapodUAxisStatus", "HXPD1611-4-I10-01:U:status", this, 0.5);
	uAxisDriveHigh_ = new AMSinglePVControl("HexapodUAxisDriveHigh", "HXPD1611-4-I10-01:U:deg.DRVH", this, AXIS_POSITION_TOLERANCE);
	uAxisDriveLow_ = new AMSinglePVControl("HexapodUAxisDriveLow", "HXPD1611-4-I10-01:U:deg.DRVL", this, AXIS_POSITION_TOLERANCE);

	vAxisFeedback_ = new AMSinglePVControl("HexapodVAxisValue", "HXPD1611-4-I10-01:V:deg:fbk", this, AXIS_POSITION_TOLERANCE);
	vAxisSetpoint_ = new AMSinglePVControl("HexapodVAxisSetpoint", "HXPD1611-4-I10-01:V:deg", this, AXIS_POSITION_TOLERANCE);
	vAxisStatus_ = new AMSinglePVControl("HexapodVAxisStatus", "HXPD1611-4-I10-01:V:status", this, 0.5);
	vAxisDriveHigh_ = new AMSinglePVControl("HexapodVAxisDriveHigh", "HXPD1611-4-I10-01:V:deg.DRVH", this, AXIS_POSITION_TOLERANCE);
	vAxisDriveLow_ = new AMSinglePVControl("HexapodVAxisDriveLow", "HXPD1611-4-I10-01:V:deg.DRVL", this, AXIS_POSITION_TOLERANCE);

	wAxisFeedback_ = new AMSinglePVControl("HexapodWAxisValue", "HXPD1611-4-I10-01:W:deg:fbk", this, AXIS_POSITION_TOLERANCE);
	wAxisSetpoint_ = new AMSinglePVControl("HexapodWAxisSetpoint", "HXPD1611-4-I10-01:W:deg", this, AXIS_POSITION_TOLERANCE);
	wAxisStatus_ = new AMSinglePVControl("HexapodWAxisStatus", "HXPD1611-4-I10-01:W:status", this, 0.5);
	wAxisDriveHigh_ = new AMSinglePVControl("HexapodWAxisDriveHigh", "HXPD1611-4-I10-01:W:deg.DRVH", this, AXIS_POSITION_TOLERANCE);
	wAxisDriveLow_ = new AMSinglePVControl("HexapodWAxisDriveLow", "HXPD1611-4-I10-01:W:deg.DRVL", this, AXIS_POSITION_TOLERANCE);

	systemVelocityFeedback_ = new AMSinglePVControl("HexapodSystemVelocityValue", "HXPD1611-4-I10-01:velocity:fbk", this, 0.001);
	systemVelocitySetpoint_ = new AMSinglePVControl("HexapodSystemVelocityValue", "HXPD1611-4-I10-01:velocity", this, 0.001);
	stopAll_ = new AMSinglePVControl("HexapodStopAll", "HXPD1611-4-I10-01:stop", this, 0.5);

	allControls_->addControl(xAxisFeedback_);
	allControls_->addControl(xAxisSetpoint_);
	allControls_->addControl(xAxisStatus_);
	allControls_->addControl(xAxisDriveHigh_);
	allControls_->addControl(xAxisDriveLow_);

	allControls_->addControl(yAxisFeedback_);
	allControls_->addControl(yAxisSetpoint_);
	allControls_->addControl(yAxisStatus_);
	allControls_->addControl(yAxisDriveHigh_);
	allControls_->addControl(yAxisDriveLow_);

	allControls_->addControl(zAxisFeedback_);
	allControls_->addControl(zAxisSetpoint_);
	allControls_->addControl(zAxisStatus_);
	allControls_->addControl(zAxisDriveHigh_);
	allControls_->addControl(zAxisDriveLow_);

	allControls_->addControl(uAxisFeedback_);
	allControls_->addControl(uAxisSetpoint_);
	allControls_->addControl(uAxisStatus_);
	allControls_->addControl(uAxisDriveHigh_);
	allControls_->addControl(uAxisDriveLow_);

	allControls_->addControl(vAxisFeedback_);
	allControls_->addControl(vAxisSetpoint_);
	allControls_->addControl(vAxisStatus_);
	allControls_->addControl(vAxisDriveHigh_);
	allControls_->addControl(vAxisDriveLow_);

	allControls_->addControl(wAxisFeedback_);
	allControls_->addControl(wAxisSetpoint_);
	allControls_->addControl(wAxisStatus_);
	allControls_->addControl(wAxisDriveHigh_);
	allControls_->addControl(wAxisDriveLow_);

	allControls_->addControl(systemVelocityFeedback_);
	allControls_->addControl(systemVelocitySetpoint_);
	allControls_->addControl(stopAll_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllConnected(bool)));
	connect(trajectoryCoordinator_, SIGNAL(startTrajectoryMotion()),
			this, SLOT(onTrajectoryMove()));
	connect(controller_, SIGNAL(targetPositionChanged(AMPIC887AxisMap<double>)),
			this, SLOT(onTargetPositionChanged(AMPIC887AxisMap<double>)));
}

void AMPIC887EpicsCoordinator::onTargetPositionChanged(const AMPIC887AxisMap<double> &targetPositions)
{

	trajectoryCoordinator_->setLastSetTargetPosition(targetPositions);

	if(connectedOnce_ && !xAxisSetpoint_->withinTolerance(targetPositions.value(AMGCS2::XAxis))) {
		xAxisSetpoint_->move(targetPositions.value(AMGCS2::XAxis));
	}

	if(connectedOnce_ && !yAxisSetpoint_->withinTolerance(targetPositions.value(AMGCS2::YAxis))) {
		yAxisSetpoint_->move(targetPositions.value(AMGCS2::YAxis));
	}

	if(connectedOnce_ && !zAxisSetpoint_->withinTolerance(targetPositions.value(AMGCS2::ZAxis))) {
		zAxisSetpoint_->move(targetPositions.value(AMGCS2::ZAxis));
	}

	if(connectedOnce_ && !uAxisSetpoint_->withinTolerance(targetPositions.value(AMGCS2::UAxis))) {
		uAxisSetpoint_->move(targetPositions.value(AMGCS2::UAxis));
	}

	if(connectedOnce_ && !vAxisSetpoint_->withinTolerance(targetPositions.value(AMGCS2::VAxis))) {
		vAxisSetpoint_->move(targetPositions.value(AMGCS2::VAxis));
	}

	if(connectedOnce_ && !wAxisSetpoint_->withinTolerance(targetPositions.value(AMGCS2::WAxis))) {
		wAxisSetpoint_->move(targetPositions.value(AMGCS2::WAxis));
	}
}

void AMPIC887EpicsCoordinator::onPositionUpdate(const AMPIC887AxisMap<double> &newPositions)
{
	if(!xAxisFeedback_->withinTolerance(newPositions.value(AMGCS2::XAxis))) {
		xAxisFeedback_->move(newPositions.value(AMGCS2::XAxis));
	}

	if(!yAxisFeedback_->withinTolerance(newPositions.value(AMGCS2::YAxis))) {
		yAxisFeedback_->move(newPositions.value(AMGCS2::YAxis));
	}

	if(!zAxisFeedback_->withinTolerance(newPositions.value(AMGCS2::ZAxis))) {
		zAxisFeedback_->move(newPositions.value(AMGCS2::ZAxis));
	}

	if(!uAxisFeedback_->withinTolerance(newPositions.value(AMGCS2::UAxis))) {
		uAxisFeedback_->move(newPositions.value(AMGCS2::UAxis));
	}

	if(!vAxisFeedback_->withinTolerance(newPositions.value(AMGCS2::VAxis))) {
		vAxisFeedback_->move(newPositions.value(AMGCS2::VAxis));
	}

	if(!wAxisFeedback_->withinTolerance(newPositions.value(AMGCS2::WAxis))) {
		wAxisFeedback_->move(newPositions.value(AMGCS2::WAxis));
	}
}

void AMPIC887EpicsCoordinator::onSystemVelocityChanged(double systemVelocity)
{
	if(!systemVelocityFeedback_->withinTolerance(systemVelocity)) {
		systemVelocityFeedback_->move(systemVelocity);
	}
}

void AMPIC887EpicsCoordinator::onMotionStartedChanged(AMGCS2::AxisMovementStatuses movementStatuses)
{
	qDebug() << "Motion started " << movementStatuses;

	qDebug() << "Status before = " << xAxisStatus_->value();
	if(movementStatuses.testFlag(AMGCS2::XAxisIsMoving) && !xAxisStatus_->withinTolerance(1)) {
		qDebug() << "Motion started on x Axis";
		xAxisStatus_->move(1);
	}

	qDebug() << "Status before = " << yAxisStatus_->value();
	if(movementStatuses.testFlag(AMGCS2::YAxisIsMoving) && !yAxisStatus_->withinTolerance(1)) {
		qDebug() << "Motion started on y Axis";
		yAxisStatus_->move(1);
	}

	qDebug() << "Status before = " << zAxisStatus_->value();
	if(movementStatuses.testFlag(AMGCS2::ZAxisIsMoving) && !zAxisStatus_->withinTolerance(1)) {
		qDebug() << "Motion started on z Axis";
		zAxisStatus_->move(1);
	}

	qDebug() << "Status before = " << uAxisStatus_->value();
	if(movementStatuses.testFlag(AMGCS2::UAxisIsMoving) && !uAxisStatus_->withinTolerance(1)) {
		qDebug() << "Motion started on u Axis";
		uAxisStatus_->move(1);
	}

	qDebug() << "Status before = " << vAxisStatus_->value();
	if(movementStatuses.testFlag(AMGCS2::VAxisIsMoving) && !vAxisStatus_->withinTolerance(1)) {
		qDebug() << "Motion started on v Axis";
		vAxisStatus_->move(1);
	}

	qDebug() << "Status before = " << wAxisStatus_->value();
	if(movementStatuses.testFlag(AMGCS2::WAxisIsMoving) && !wAxisStatus_->withinTolerance(1)) {
		qDebug() << "Motion started on w Axis";
		wAxisStatus_->move(1);
	}
}

void AMPIC887EpicsCoordinator::onMotionCompleted()
{
	qDebug() << "All motions completed";
	if(!xAxisStatus_->withinTolerance(0)) {
		xAxisStatus_->move(0);
	}

	if(!yAxisStatus_->withinTolerance(0)) {
		yAxisStatus_->move(0);
	}

	if(!zAxisStatus_->withinTolerance(0)) {
		zAxisStatus_->move(0);
	}

	if(!uAxisStatus_->withinTolerance(0)) {
		uAxisStatus_->move(0);
	}

	if(!vAxisStatus_->withinTolerance(0)) {
		vAxisStatus_->move(0);
	}

	if(!wAxisStatus_->withinTolerance(0)) {
		wAxisStatus_->move(0);
	}
}

void AMPIC887EpicsCoordinator::onMotionFailed(AMGCS2::AxisMovementStatuses movementStatuses)
{
	qDebug() << "Motions failed";
	if(!xAxisStatus_->withinTolerance(4) && movementStatuses.testFlag(AMGCS2::XAxisIsMoving)) {
		xAxisStatus_->move(4);
	}

	if(!yAxisStatus_->withinTolerance(4) && movementStatuses.testFlag(AMGCS2::YAxisIsMoving)) {
		yAxisStatus_->move(4);
	}

	if(!zAxisStatus_->withinTolerance(4) && movementStatuses.testFlag(AMGCS2::ZAxisIsMoving)) {
		zAxisStatus_->move(4);
	}

	if(!uAxisStatus_->withinTolerance(4) && movementStatuses.testFlag(AMGCS2::UAxisIsMoving)) {
		uAxisStatus_->move(4);
	}

	if(!vAxisStatus_->withinTolerance(4) && movementStatuses.testFlag(AMGCS2::VAxisIsMoving)) {
		vAxisStatus_->move(4);
	}

	if(!wAxisStatus_->withinTolerance(4) && movementStatuses.testFlag(AMGCS2::WAxisIsMoving)) {
		wAxisStatus_->move(4);
	}
}

void AMPIC887EpicsCoordinator::onXAxisSetpointChanged(double setpoint)
{
	if(!xAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::XAxis))) {
		qDebug() << "X setpoint to " << setpoint;
		AMPIC887AxisMap<double> newPosition;
		newPosition.insert(AMGCS2::XAxis, setpoint);
		controller_->move(newPosition);
	} else {
		// Setpoint has been changed to same value. We need to pretend that we've
		// moved for AM's sake.
		onMotionStartedChanged(AMGCS2::XAxisIsMoving);
		QTimer::singleShot(20, this, SLOT(onMotionCompleted()));
	}
}

void AMPIC887EpicsCoordinator::onYAxisSetpointChanged(double setpoint)
{
	if(!yAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::YAxis))) {
		qDebug() << "Y setpoint to " << setpoint;
		AMPIC887AxisMap<double> newPosition;
		newPosition.insert(AMGCS2::YAxis, setpoint);
		controller_->move(newPosition);
	} else {
		// Setpoint has been changed to same value. We need to pretend that we've
		// moved for AM's sake.
		onMotionStartedChanged(AMGCS2::YAxisIsMoving);
		QTimer::singleShot(20, this, SLOT(onMotionCompleted()));
	}
}

void AMPIC887EpicsCoordinator::onZAxisSetpointChanged(double setpoint)
{
	if(!zAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::ZAxis))) {
		qDebug() << "Z setpoint to " << setpoint;
		AMPIC887AxisMap<double> newPosition;
		newPosition.insert(AMGCS2::ZAxis, setpoint);
		controller_->move(newPosition);
	} else {
		// Setpoint has been changed to same value. We need to pretend that we've
		// moved for AM's sake.
		onMotionStartedChanged(AMGCS2::ZAxisIsMoving);
		QTimer::singleShot(20, this, SLOT(onMotionCompleted()));
	}
}

void AMPIC887EpicsCoordinator::onUAxisSetpointChanged(double setpoint)
{
	if(!uAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::UAxis))) {
		qDebug() << "U setpoint to " << setpoint;
		AMPIC887AxisMap<double> newPosition;
		newPosition.insert(AMGCS2::UAxis, setpoint);
		controller_->move(newPosition);
	} else {
		// Setpoint has been changed to same value. We need to pretend that we've
		// moved for AM's sake.
		onMotionStartedChanged(AMGCS2::UAxisIsMoving);
		QTimer::singleShot(20, this, SLOT(onMotionCompleted()));
	}
}

void AMPIC887EpicsCoordinator::onVAxisSetpointChanged(double setpoint)
{
	if(!vAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::VAxis))) {
		qDebug() << "V setpoint to " << setpoint;
		AMPIC887AxisMap<double> newPosition;
		newPosition.insert(AMGCS2::VAxis, setpoint);
		controller_->move(newPosition);
	} else {
		// Setpoint has been changed to same value. We need to pretend that we've
		// moved for AM's sake.
		onMotionStartedChanged(AMGCS2::VAxisIsMoving);
		QTimer::singleShot(20, this, SLOT(onMotionCompleted()));
	}
}

void AMPIC887EpicsCoordinator::onWAxisSetpointChanged(double setpoint)
{
	if(!wAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::WAxis))) {
		qDebug() << "W setpoint to " << setpoint;
		AMPIC887AxisMap<double> newPosition;
		newPosition.insert(AMGCS2::WAxis, setpoint);
		controller_->move(newPosition);
	} else {
		// Setpoint has been changed to same value. We need to pretend that we've
		// moved for AM's sake.
		onMotionStartedChanged(AMGCS2::WAxisIsMoving);
		QTimer::singleShot(20, this, SLOT(onMotionCompleted()));
	}
}


void AMPIC887EpicsCoordinator::onSystemVelocitySetpointChanged(double value)
{
	if(!systemVelocitySetpoint_->withinTolerance(controller_->systemVelocity())) {

		if(controller_->setSystemVelocity(value)) {
			qDebug() << "System velocity set to " << value;
		} else {
			qDebug() << "Failed to set system velocity to " << value;
		}
	}
}

void AMPIC887EpicsCoordinator::onStopAll(double)
{
	if(stopAll_->withinTolerance(1.0)) {
		controller_->stop();
		stopAll_->move(0.0);
	}

	if(!stopAll_->withinTolerance(0.0)) {
		stopAll_->move(0.0);
	}
}

void AMPIC887EpicsCoordinator::onAllConnected(bool connectedState)
{
	if(connectedState) {

		if(!controller_->isInValidState()) {
			qDebug() << "Initialization: Setting Axes status to Error";
			xAxisStatus_->move(4);
			yAxisStatus_->move(4);
			zAxisStatus_->move(4);
			uAxisStatus_->move(4);
			vAxisStatus_->move(4);
			wAxisStatus_->move(4);
		} else if (!connectedOnce_) {

			connectedOnce_ = true;
			// Initializing x Axis
			if(!xAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::XAxis))) {
				qDebug() << "Initialization: Setting x Axis position to " << controller_->currentPosition(AMGCS2::XAxis);
				xAxisFeedback_->move(controller_->currentPosition(AMGCS2::XAxis));
			}

			if(!xAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::XAxis))) {
				qDebug() << "Initialization: Setting x Axis setpoint to " << controller_->targetPosition(AMGCS2::XAxis);
				xAxisSetpoint_->move(controller_->targetPosition(AMGCS2::XAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::XAxisIsMoving) && !xAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting x Axis status to moving";
				xAxisStatus_->move(1);
			} else if (!xAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting x Axis status to move done";
				xAxisStatus_->move(0);
			}

			double minValue = controller_->minCommandablePosition(AMGCS2::XAxis);
			double maxValue = controller_->maxCommandablePosition(AMGCS2::XAxis);
			if(controller_->softLimitStatus(AMGCS2::XAxis)) {
				minValue = controller_->lowSoftLimit(AMGCS2::XAxis);
				maxValue = controller_->highSoftLimit(AMGCS2::XAxis);
			}

			if(!xAxisDriveLow_->withinTolerance(minValue)) {
				xAxisDriveLow_->move(minValue);
			}

			if(!xAxisDriveHigh_->withinTolerance(maxValue)) {
				xAxisDriveHigh_->move(maxValue);
			}


			// Initializing y Axis
			if(!yAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::YAxis))) {
				qDebug() << "Initialzation: Setting y Axis position to " << controller_->currentPosition(AMGCS2::YAxis);
				yAxisFeedback_->move(controller_->currentPosition(AMGCS2::YAxis));
			}

			if(!yAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::YAxis))) {
				qDebug() << "Initialization: Setting y Axis setpoint to " << controller_->targetPosition(AMGCS2::YAxis);
				yAxisSetpoint_->move(controller_->targetPosition(AMGCS2::YAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::YAxisIsMoving) && !yAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting y Axis status to moving";
				yAxisStatus_->move(1);
			} else if (!yAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting y Axis status to move done";
				yAxisStatus_->move(0);
			}

			minValue = controller_->minCommandablePosition(AMGCS2::YAxis);
			maxValue = controller_->maxCommandablePosition(AMGCS2::YAxis);
			if(controller_->softLimitStatus(AMGCS2::YAxis)) {
				minValue = controller_->lowSoftLimit(AMGCS2::YAxis);
				maxValue = controller_->highSoftLimit(AMGCS2::YAxis);
			}

			if(!yAxisDriveLow_->withinTolerance(minValue)) {
				yAxisDriveLow_->move(minValue);
			}

			if(!yAxisDriveHigh_->withinTolerance(maxValue)) {
				yAxisDriveHigh_->move(maxValue);
			}

			// Initializing z Axis
			if(!zAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::ZAxis))) {
				qDebug() << "Initialzation: Setting z Axis position to " << controller_->currentPosition(AMGCS2::ZAxis);
				zAxisFeedback_->move(controller_->currentPosition(AMGCS2::ZAxis));
			}

			if(!zAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::ZAxis))) {
				qDebug() << "Initialization: Setting z Axis setpoint to " << controller_->targetPosition(AMGCS2::ZAxis);
				zAxisSetpoint_->move(controller_->targetPosition(AMGCS2::ZAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::ZAxisIsMoving) && !zAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting z Axis status to moving";
				zAxisStatus_->move(1);
			} else if (!zAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting z Axis status to move done";
				zAxisStatus_->move(0);
			}

			minValue = controller_->minCommandablePosition(AMGCS2::ZAxis);
			maxValue = controller_->maxCommandablePosition(AMGCS2::ZAxis);
			if(controller_->softLimitStatus(AMGCS2::ZAxis)) {
				minValue = controller_->lowSoftLimit(AMGCS2::ZAxis);
				maxValue = controller_->highSoftLimit(AMGCS2::ZAxis);
			}

			if(!zAxisDriveLow_->withinTolerance(minValue)) {
				zAxisDriveLow_->move(minValue);
			}

			if(!zAxisDriveHigh_->withinTolerance(maxValue)) {
				zAxisDriveHigh_->move(maxValue);
			}

			// Initializing u Axis
			if(!uAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::UAxis))) {
				qDebug() << "Initialzation: Setting u Axis position to " << controller_->currentPosition(AMGCS2::UAxis);
				uAxisFeedback_->move(controller_->currentPosition(AMGCS2::UAxis));
			}

			if(!uAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::UAxis))) {
				qDebug() << "Initialization: Setting u Axis setpoint to " << controller_->targetPosition(AMGCS2::UAxis);
				uAxisSetpoint_->move(controller_->targetPosition(AMGCS2::UAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::UAxisIsMoving) && !uAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting u Axis status to moving";
				uAxisStatus_->move(1);
			} else if (!uAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting u Axis status to move done";
				uAxisStatus_->move(0);
			}

			minValue = controller_->minCommandablePosition(AMGCS2::UAxis);
			maxValue = controller_->maxCommandablePosition(AMGCS2::UAxis);
			if(controller_->softLimitStatus(AMGCS2::UAxis)) {
				minValue = controller_->lowSoftLimit(AMGCS2::UAxis);
				maxValue = controller_->highSoftLimit(AMGCS2::UAxis);
			}

			if(!uAxisDriveLow_->withinTolerance(minValue)) {
				uAxisDriveLow_->move(minValue);
			}

			if(!uAxisDriveHigh_->withinTolerance(maxValue)) {
				uAxisDriveHigh_->move(maxValue);
			}

			// Initializing v Axis
			if(!vAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::VAxis))) {
				qDebug() << "Initialzation: Setting v Axis position to " << controller_->currentPosition(AMGCS2::VAxis);
				vAxisFeedback_->move(controller_->currentPosition(AMGCS2::VAxis));
			}

			if(!vAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::VAxis))) {
				qDebug() << "Initialization: Setting v Axis setpoint to " << controller_->targetPosition(AMGCS2::VAxis);
				vAxisSetpoint_->move(controller_->targetPosition(AMGCS2::VAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::VAxisIsMoving) && !vAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting v Axis status to moving";
				vAxisStatus_->move(1);
			} else if (!vAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting v Axis status to move done";
				vAxisStatus_->move(0);
			}

			minValue = controller_->minCommandablePosition(AMGCS2::VAxis);
			maxValue = controller_->maxCommandablePosition(AMGCS2::VAxis);
			if(controller_->softLimitStatus(AMGCS2::VAxis)) {
				minValue = controller_->lowSoftLimit(AMGCS2::VAxis);
				maxValue = controller_->highSoftLimit(AMGCS2::VAxis);
			}

			if(!vAxisDriveLow_->withinTolerance(minValue)) {
				vAxisDriveLow_->move(minValue);
			}

			if(!vAxisDriveHigh_->withinTolerance(maxValue)) {
				vAxisDriveHigh_->move(maxValue);
			}

			// Initializing w Axis
			if(!wAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::WAxis))) {
				qDebug() << "Initialzation: Setting w Axis position to " << controller_->currentPosition(AMGCS2::WAxis);
				wAxisFeedback_->move(controller_->currentPosition(AMGCS2::WAxis));
			}

			if(!wAxisSetpoint_->withinTolerance(controller_->targetPosition(AMGCS2::WAxis))) {
				qDebug() << "Initialization: Setting w Axis setpoint to " << controller_->targetPosition(AMGCS2::WAxis);
				wAxisSetpoint_->move(controller_->targetPosition(AMGCS2::WAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::WAxisIsMoving) && !wAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting w Axis status to moving";
				wAxisStatus_->move(1);
			} else if (!wAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting w Axis status to move done";
				wAxisStatus_->move(0);
			}

			minValue = controller_->minCommandablePosition(AMGCS2::WAxis);
			maxValue = controller_->maxCommandablePosition(AMGCS2::WAxis);
			if(controller_->softLimitStatus(AMGCS2::WAxis)) {
				minValue = controller_->lowSoftLimit(AMGCS2::WAxis);
				maxValue = controller_->highSoftLimit(AMGCS2::WAxis);
			}

			if(!wAxisDriveLow_->withinTolerance(minValue)) {
				wAxisDriveLow_->move(minValue);
			}

			if(!wAxisDriveHigh_->withinTolerance(maxValue)) {
				wAxisDriveHigh_->move(maxValue);
			}

			// Initialize system velocity
			if(!systemVelocityFeedback_->withinTolerance(controller_->systemVelocity())) {
				qDebug() << "Initialization: Setting system velocity to " << controller_->systemVelocity();
				systemVelocityFeedback_->move(controller_->systemVelocity());
			}

			if(!systemVelocitySetpoint_->withinTolerance(controller_->systemVelocity())) {
				qDebug() << "Initialization: Setting system velocity setpoint to " << controller_->systemVelocity();
				systemVelocitySetpoint_->move(controller_->systemVelocity());
			}
		}

		connect(xAxisSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onXAxisSetpointChanged(double)));
		connect(yAxisSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onYAxisSetpointChanged(double)));
		connect(zAxisSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onZAxisSetpointChanged(double)));
		connect(uAxisSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onUAxisSetpointChanged(double)));
		connect(vAxisSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onVAxisSetpointChanged(double)));
		connect(wAxisSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onWAxisSetpointChanged(double)));

		connect(systemVelocitySetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onSystemVelocitySetpointChanged(double)));
		connect(stopAll_, SIGNAL(valueChanged(double)), this, SLOT(onStopAll(double)));

		connect(controller_, SIGNAL(moveStarted(AMGCS2::AxisMovementStatuses)), this, SLOT(onMotionStartedChanged(AMGCS2::AxisMovementStatuses)));
		connect(controller_, SIGNAL(moveFailed(AMGCS2::AxisMovementStatuses)), this, SLOT(onMotionFailed(AMGCS2::AxisMovementStatuses)));
		connect(controller_, SIGNAL(moveComplete()), this, SLOT(onMotionCompleted()));
		connect(controller_, SIGNAL(positionUpdate(AMPIC887AxisMap<double>)), this, SLOT(onPositionUpdate(AMPIC887AxisMap<double>)));
		connect(controller_, SIGNAL(systemVelocityChanged(double)), this, SLOT(onSystemVelocityChanged(double)));
	}
}

void AMPIC887EpicsCoordinator::onTrajectoryMove()
{
	AMPIC887AxisMap<double> targetPositions = trajectoryCoordinator_->currentTrajectory();

	if(!targetPositions.isEmpty()) {
		controller_->move(targetPositions);
	}
}











