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
	xAxisStatus_ = new AMSinglePVControl("HexapodXAxisStatus", "HXPD1611-4-I10-01:X:status", this, 0.5);
	xAxisRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:X:mm", 0, 1024, this);
	xAxisRecorderFeedback_->setAttemptDouble(true);

	yAxisFeedback_ = new AMSinglePVControl("HexapodYAxisValue", "HXPD1611-4-I10-01:Y:mm:fbk", this, AXIS_POSITION_TOLERANCE);
	yAxisStatus_ = new AMSinglePVControl("HexapodYAxisStatus", "HXPD1611-4-I10-01:Y:status", this, 0.5);
	yAxisRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:Y:mm", 0, 1024, this);
	yAxisRecorderFeedback_->setAttemptDouble(true);

	zAxisFeedback_ = new AMSinglePVControl("HexapodZAxisValue", "HXPD1611-4-I10-01:Z:mm:fbk", this, AXIS_POSITION_TOLERANCE);
	zAxisStatus_ = new AMSinglePVControl("HexapodZAxisStatus", "HXPD1611-4-I10-01:Z:status", this, 0.5);
	zAxisRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:Z:mm", 0, 1024, this);
	zAxisRecorderFeedback_->setAttemptDouble(true);

	uAxisFeedback_ = new AMSinglePVControl("HexapodUAxisValue", "HXPD1611-4-I10-01:U:deg:fbk", this, AXIS_POSITION_TOLERANCE);
	uAxisStatus_ = new AMSinglePVControl("HexapodUAxisStatus", "HXPD1611-4-I10-01:U:status", this, 0.5);
	uAxisRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:U:deg", 0, 1024, this);
	uAxisRecorderFeedback_->setAttemptDouble(true);

	vAxisFeedback_ = new AMSinglePVControl("HexapodVAxisValue", "HXPD1611-4-I10-01:V:deg:fbk", this, AXIS_POSITION_TOLERANCE);
	vAxisStatus_ = new AMSinglePVControl("HexapodVAxisStatus", "HXPD1611-4-I10-01:V:status", this, 0.5);
	vAxisRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:V:deg", 0, 1024, this);
	vAxisRecorderFeedback_->setAttemptDouble(true);

	wAxisFeedback_ = new AMSinglePVControl("HexapodWAxisValue", "HXPD1611-4-I10-01:W:deg:fbk", this, AXIS_POSITION_TOLERANCE);
	wAxisStatus_ = new AMSinglePVControl("HexapodWAxisStatus", "HXPD1611-4-I10-01:W:status", this, 0.5);
	wAxisRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:W:deg", 0, 1024, this);
	wAxisRecorderFeedback_->setAttemptDouble(true);

	timeRecorderFeedback_ = new AMWaveformBinningSinglePVControl("HexapodXAxisRecordedPositions", "HXPD1611-4-I10-01:recorder:time:s", 0, 1024, this);
	timeRecorderFeedback_->setAttemptDouble(true);

	dataRecorderActive_ = new AMSinglePVControl("HexapodRecorderActive", "HXPD1611-4-I10-01:recorder:active", this, 0.5);
	recordRateSetpoint_ = new AMSinglePVControl("HexapodRecordRateSetpoint", "HXPD1611-4-I10-01:recorder:rate:hz", this, 0.01);
	recordRateFeedback_ = new AMSinglePVControl("HexapodRecordRateFeedback", "HXPD1611-4-I10-01:recorder:rate:hz:fbk", this, 0.01);
	systemVelocityFeedback_ = new AMSinglePVControl("HexapodSystemVelocityValue", "HXPD1611-4-I10-01:velocity:fbk", this, 0.001);
	systemVelocitySetpoint_ = new AMSinglePVControl("HexapodSystemVelocityValue", "HXPD1611-4-I10-01:velocity", this, 0.001);
	stopAll_ = new AMSinglePVControl("HexapodStopAll", "HXPD1611-4-I10-01:stop", this, 0.5);

	allControls_->addControl(xAxisFeedback_);
	allControls_->addControl(xAxisStatus_);
	allControls_->addControl(xAxisRecorderFeedback_);

	allControls_->addControl(yAxisFeedback_);
	allControls_->addControl(yAxisStatus_);
	allControls_->addControl(yAxisRecorderFeedback_);

	allControls_->addControl(zAxisFeedback_);
	allControls_->addControl(zAxisStatus_);
	allControls_->addControl(zAxisRecorderFeedback_);

	allControls_->addControl(uAxisFeedback_);
	allControls_->addControl(uAxisStatus_);
	allControls_->addControl(uAxisRecorderFeedback_);

	allControls_->addControl(vAxisFeedback_);
	allControls_->addControl(vAxisStatus_);
	allControls_->addControl(vAxisRecorderFeedback_);

	allControls_->addControl(wAxisFeedback_);
	allControls_->addControl(wAxisStatus_);
	allControls_->addControl(wAxisRecorderFeedback_);

	allControls_->addControl(dataRecorderActive_);
	allControls_->addControl(timeRecorderFeedback_);
	allControls_->addControl(recordRateFeedback_);
	allControls_->addControl(recordRateSetpoint_);
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
}

void AMPIC887EpicsCoordinator::onPositionUpdate(const AMPIC887AxisMap<double> &newPositions)
{
	xAxisFeedback_->move(newPositions.value(AMGCS2::XAxis));
	yAxisFeedback_->move(newPositions.value(AMGCS2::YAxis));
	zAxisFeedback_->move(newPositions.value(AMGCS2::ZAxis));

	uAxisFeedback_->move(newPositions.value(AMGCS2::UAxis));
	vAxisFeedback_->move(newPositions.value(AMGCS2::VAxis));
	wAxisFeedback_->move(newPositions.value(AMGCS2::WAxis));
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
	if(controller_->movementStatuses() == 0) {
		qDebug() << "All motions completed";
		if(controller_->isDataRecorderActive()) {
			parseRecordedPositionData();
		}

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

			if(controller_->movementStatuses().testFlag(AMGCS2::XAxisIsMoving) && !xAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting x Axis status to moving";
				xAxisStatus_->move(1);
			} else if (!xAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting x Axis status to move done";
				xAxisStatus_->move(0);
			}

			// Initializing y Axis
			if(!yAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::YAxis))) {
				qDebug() << "Initialzation: Setting y Axis position to " << controller_->currentPosition(AMGCS2::YAxis);
				yAxisFeedback_->move(controller_->currentPosition(AMGCS2::YAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::YAxisIsMoving) && !yAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting y Axis status to moving";
				yAxisStatus_->move(1);
			} else if (!yAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting y Axis status to move done";
				yAxisStatus_->move(0);
			}

			// Initializing z Axis
			if(!zAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::ZAxis))) {
				qDebug() << "Initialzation: Setting z Axis position to " << controller_->currentPosition(AMGCS2::ZAxis);
				zAxisFeedback_->move(controller_->currentPosition(AMGCS2::ZAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::ZAxisIsMoving) && !zAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting z Axis status to moving";
				zAxisStatus_->move(1);
			} else if (!zAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting z Axis status to move done";
				zAxisStatus_->move(0);
			}

			// Initializing u Axis
			if(!uAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::UAxis))) {
				qDebug() << "Initialzation: Setting u Axis position to " << controller_->currentPosition(AMGCS2::UAxis);
				uAxisFeedback_->move(controller_->currentPosition(AMGCS2::UAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::UAxisIsMoving) && !uAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting u Axis status to moving";
				uAxisStatus_->move(1);
			} else if (!uAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting u Axis status to move done";
				uAxisStatus_->move(0);
			}

			// Initializing v Axis
			if(!vAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::VAxis))) {
				qDebug() << "Initialzation: Setting v Axis position to " << controller_->currentPosition(AMGCS2::VAxis);
				vAxisFeedback_->move(controller_->currentPosition(AMGCS2::VAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::VAxisIsMoving) && !vAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting v Axis status to moving";
				vAxisStatus_->move(1);
			} else if (!vAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting v Axis status to move done";
				vAxisStatus_->move(0);
			}

			// Initializing w Axis
			if(!wAxisFeedback_->withinTolerance(controller_->currentPosition(AMGCS2::WAxis))) {
				qDebug() << "Initialzation: Setting w Axis position to " << controller_->currentPosition(AMGCS2::WAxis);
				wAxisFeedback_->move(controller_->currentPosition(AMGCS2::WAxis));
			}

			if(controller_->movementStatuses().testFlag(AMGCS2::WAxisIsMoving) && !wAxisStatus_->withinTolerance(1)) {
				qDebug() << "Initialization: Setting w Axis status to moving";
				wAxisStatus_->move(1);
			} else if (!wAxisStatus_->withinTolerance(0)) {
				qDebug() << "Initialization: Setting w Axis status to move done";
				wAxisStatus_->move(0);
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

			// Initialize the record rate.
			if(!recordRateFeedback_->withinTolerance(controller_->recordRate())) {
				qDebug() << "Initialization: Setting record rate to " << controller_->recordRate();
				recordRateFeedback_->move(controller_->recordRate());
			}

			if(!recordRateSetpoint_->withinTolerance(controller_->recordRate())) {
				qDebug() << "Initialization: Setting record rate setpoint to " << controller_->recordRate();
				recordRateSetpoint_->move(controller_->recordRate());
			}

			// Initialize data recorder active state
			double dataRecorderActiveValue = controller_->isDataRecorderActive() ? 1.0 : 0.0;
			if(dataRecorderActive_->withinTolerance(dataRecorderActiveValue)) {
				qDebug() << "Initialization: Setting data recorder active status to " << dataRecorderActiveValue;
				dataRecorderActive_->move(dataRecorderActiveValue);
			}
		}

		connect(systemVelocitySetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onSystemVelocitySetpointChanged(double)));
		connect(recordRateSetpoint_, SIGNAL(valueChanged(double)), this, SLOT(onRecordRateSetpointPVChanged(double)));
		connect(dataRecorderActive_, SIGNAL(valueChanged(double)), this, SLOT(onDataRecorderActivePVChanged(double)));
		connect(stopAll_, SIGNAL(valueChanged(double)), this, SLOT(onStopAll(double)));

		connect(controller_, SIGNAL(moveStarted(AMGCS2::AxisMovementStatuses)), this, SLOT(onMotionStartedChanged(AMGCS2::AxisMovementStatuses)));
		connect(controller_, SIGNAL(moveFailed(AMGCS2::AxisMovementStatuses)), this, SLOT(onMotionFailed(AMGCS2::AxisMovementStatuses)));
		connect(controller_, SIGNAL(moveComplete()), this, SLOT(onMotionCompleted()));
		connect(controller_, SIGNAL(positionUpdate(AMPIC887AxisMap<double>)), this, SLOT(onPositionUpdate(AMPIC887AxisMap<double>)));
		connect(controller_, SIGNAL(systemVelocityChanged(double)), this, SLOT(onSystemVelocityChanged(double)));
		connect(controller_, SIGNAL(recordRateChanged(double)), this, SLOT(onControllerRecordRateChanged(double)));
		connect(controller_, SIGNAL(dataRecorderActiveStateChanged(bool)), this, SLOT(onControllerDataRecorderActiveChanged(bool)));
	}
}

void AMPIC887EpicsCoordinator::onTrajectoryMove()
{
	qDebug() << "Moving to :";
	AMPIC887AxisMap<double> targetPositions = trajectoryCoordinator_->currentTrajectory();
	qDebug() << targetPositions.toString("Position");

	if(!targetPositions.isEmpty()) {
		controller_->move(targetPositions);
	}
}

void AMPIC887EpicsCoordinator::onRecordRateSetpointPVChanged(double value)
{
	if(qAbs(value - controller_->recordRate()) > recordRateSetpoint_->tolerance()) {

		qDebug() << "Setting controller record rate to " << value;
		controller_->setRecordRate(value);
	}
}

void AMPIC887EpicsCoordinator::onControllerRecordRateChanged(double value)
{
	if(!recordRateFeedback_->withinTolerance(value)) {

		qDebug() << "Setting record rate feedback to " << value;
		recordRateFeedback_->move(value);
	}
}

void AMPIC887EpicsCoordinator::onDataRecorderActivePVChanged(double value)
{
	bool isActive;
	if(qAbs(value - 1.0) < 0.5) {

		isActive = true;
	} else {

		isActive = false;
	}

	if(controller_->isDataRecorderActive() != isActive) {

		qDebug() << "Setting the controller data recorder active state to " << isActive;
		controller_->setDataRecorderActive(isActive);
	}
}

void AMPIC887EpicsCoordinator::onControllerDataRecorderActiveChanged(bool isActive)
{
	double isActiveValue = isActive ? 1.0 : 0.0;

	if(!dataRecorderActive_->withinTolerance(isActiveValue)) {

		qDebug() << "Setting the data recorder active PV value to " << isActiveValue;
		dataRecorderActive_->move(isActiveValue);
	}
}

void AMPIC887EpicsCoordinator::parseRecordedPositionData()
{
	QVector<AMPIC887HexapodPosition> recordedData = controller_->lastRecordedPositionData();

	// The structure of each PV is for each waveform to contain 1025 records. The
	// first will be the number of element recorded in the previous move, followed
	// by up to 1024 elements of data.
	QVector<double> waveformXData = QVector<double>(1025, 0);
	QVector<double> waveformYData = QVector<double>(1025, 0);
	QVector<double> waveformZData = QVector<double>(1025, 0);
	QVector<double> waveformUData = QVector<double>(1025, 0);
	QVector<double> waveformVData = QVector<double>(1025, 0);
	QVector<double> waveformWData = QVector<double>(1025, 0);
	QVector<double> waveformTimeData = QVector<double>(1025, 0);

	bool atEndOfData = false;

	for(int iPositionData = 0; iPositionData < 1024 && !atEndOfData; ++iPositionData) {

		AMPIC887HexapodPosition currentPosition = recordedData[iPositionData];

		if(currentPosition.isValid()) {

			waveformXData[iPositionData + 1] = currentPosition.xAxisPosition();
			waveformYData[iPositionData + 1] = currentPosition.yAxisPosition();
			waveformZData[iPositionData + 1] = currentPosition.zAxisPosition();
			waveformUData[iPositionData + 1] = currentPosition.uAxisPosition();
			waveformVData[iPositionData + 1] = currentPosition.vAxisPosition();
			waveformWData[iPositionData + 1] = currentPosition.wAxisPosition();
			waveformTimeData[iPositionData + 1] = currentPosition.time();
		} else {

			waveformXData[0] = iPositionData;
			waveformYData[0] = iPositionData;
			waveformZData[0] = iPositionData;
			waveformUData[0] = iPositionData;
			waveformVData[0] = iPositionData;
			waveformWData[0] = iPositionData;
			waveformTimeData[0] = iPositionData;
			atEndOfData = true;
		}
	}

	xAxisRecorderFeedback_->setValues(waveformXData);
	yAxisRecorderFeedback_->setValues(waveformYData);
	zAxisRecorderFeedback_->setValues(waveformZData);
	uAxisRecorderFeedback_->setValues(waveformUData);
	vAxisRecorderFeedback_->setValues(waveformVData);
	wAxisRecorderFeedback_->setValues(waveformWData);
	timeRecorderFeedback_->setValues(waveformTimeData);
}

















