#include "SGMHexapodTransformedAxis.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMBeamline.h"

SGMHexapodTransformedAxis::SGMHexapodTransformedAxis(AxisDesignation axis,
													 AMControl* globalXAxisSetpoint,
													 AMControl* globalXAxisFeedback,
													 AMControl* globalXAxisStatus,
													 AMControl* globalYAxisSetpoint,
													 AMControl* globalYAxisFeedback,
													 AMControl* globalYAxisStatus,
													 AMControl* globalZAxisSetpoint,
													 AMControl* globalZAxisFeedback,
													 AMControl* globalZAxisStatus,
													 AMControl* trajectoryStartControl,
                                                     AMControl* systemVelocityControl,
                                                     AMControl* dataRecorderRateControl,
				                                     AMControl* dataRecorderStatusControl,
													 const QString &name,
													 const QString &units,
													 QObject *parent,
													 const QString &description)
	: AM3DRotatedSystemControl(axis, globalXAxisSetpoint, globalYAxisSetpoint, globalZAxisSetpoint, name, units, parent, description )
{
	setAttemptMoveWhenWithinTolerance(false);
	trajectoryStartControl_ = trajectoryStartControl;
	globalXAxisFeedback_ = globalXAxisFeedback;
	globalYAxisFeedback_ = globalYAxisFeedback;
	globalZAxisFeedback_ = globalZAxisFeedback;

	globalXAxisStatus_ = globalXAxisStatus;
	globalYAxisStatus_ = globalYAxisStatus;
	globalZAxisStatus_ = globalZAxisStatus;

	systemVelocity_ = systemVelocityControl;

	dataRecorderRate_ = dataRecorderRateControl;
	dataRecorderStatus_ = dataRecorderStatusControl;

	addChildControl(globalXAxisFeedback_);
	addChildControl(globalYAxisFeedback_);
	addChildControl(globalZAxisFeedback_);

}


bool SGMHexapodTransformedAxis::shouldPerformCoordinatedMovement() const
{
	return true;
}

#include <QDebug>
bool SGMHexapodTransformedAxis::canPerformCoordinatedMovement() const
{
	qDebug() << "SGMHexapodTransformedAxis::canPerformCoordinatedMovement()?" << isConnected();
	return isConnected();
}

AMAction3 * SGMHexapodTransformedAxis::createSetParametersActions(double startPoint, double endPoint, double deltaTime)
{
	AMListAction3* action = 0;
	lastStartPoint_ = startPoint;
	lastEndPoint_ = endPoint;
	lastDeltaTime_ = deltaTime;

	qDebug() << "Checking on some axis stuff for SGMHexapodTransformedAxis::createSetParameterActions for axis " << axis_;
	if(globalXAxis_)
		qDebug() << "Have globalXAxis_";
	if(globalYAxis_)
		qDebug() << "Have globalYAxis_";
	if(globalZAxis_)
		qDebug() << "Have globalZAxis_";
	if(trajectoryStartControl_)
		qDebug() << "Have trajectoryStartControl_";
	if(globalXAxis_ && globalYAxis_ && globalZAxis_ && trajectoryStartControl_) {

		// Setting the start position

		action = new AMListAction3(new AMListActionInfo3("Setting hexapod movement parameters",
		                                                 "Setting hexapod movement parameters"),
		                           AMListAction3::Sequential);



		// Grab the current global positions:
		QVector3D currentGlobalSetpoints(globalXAxis_->setpoint(), globalYAxis_->setpoint(), globalZAxis_->setpoint());
		// Transform it to our system:
		QVector3D primeSetpoints = globalAxisToPrime(currentGlobalSetpoints);

		// Set the value in terms of our system based on the axis we are:
		switch(axis_) {
		case XAxis:
			primeSetpoints.setX(startPoint);
			break;
		case YAxis:
			primeSetpoints.setY(startPoint);
			break;
		case ZAxis:
			primeSetpoints.setZ(startPoint);

		}

		// Transform back the the global system:
		QVector3D newGlobalStartSetpoints = primeAxisToGlobal(primeSetpoints);

		// Create the required move actions in the global system:
		AMListAction3* startMoveActions = new AMListAction3(new AMListActionInfo3("Moving parameter controls for start",
		                                                                     "Moving parameter controls for end"),
													   AMListAction3::Parallel);

		startMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalXAxis_, newGlobalStartSetpoints.x()));
		startMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalYAxis_, newGlobalStartSetpoints.y()));
		startMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalZAxis_, newGlobalStartSetpoints.z()));


		action->addSubAction(startMoveActions);

		// Wait for the parameters to take
		AMListAction3* startWaitActions = new AMListAction3(new AMListActionInfo3("Waiting for parameter controls for start",
		                                                                     "Waiting for parameter controls for start"),
		                                               AMListAction3::Parallel);

		startWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(globalXAxis_, newGlobalStartSetpoints.x(), 2, AMControlWaitActionInfo::MatchWithinTolerance));
		startWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(globalYAxis_, newGlobalStartSetpoints.y(), 2, AMControlWaitActionInfo::MatchWithinTolerance));
		startWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(globalZAxis_, newGlobalStartSetpoints.z(), 2, AMControlWaitActionInfo::MatchWithinTolerance));

		action->addSubAction(startWaitActions);

		// Trigger the movement
		AMAction3* trajectoryStartAction = AMActionSupport::buildControlMoveAction(trajectoryStartControl_, 1);
		action->addSubAction(trajectoryStartAction);


		action->addSubAction(AMActionSupport::buildControlWaitAction(trajectoryStartControl_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance));

		// Wait for the move to start point to complete
		action->addSubAction(AMActionSupport::buildControlWaitAction(this, startPoint, 60, AMControlWaitActionInfo::MatchWithinTolerance));

		action->addSubAction(AMActionSupport::buildControlWaitAction(globalXAxisStatus_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance));
		action->addSubAction(AMActionSupport::buildControlWaitAction(globalYAxisStatus_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance));
		action->addSubAction(AMActionSupport::buildControlWaitAction(globalZAxisStatus_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance));

		// Setting the end position

		switch(axis_) {
		case XAxis:
			primeSetpoints.setX(endPoint);
			break;
		case YAxis:
			primeSetpoints.setY(endPoint);
			break;
		case ZAxis:
			primeSetpoints.setZ(endPoint);
		}

		// Transform the end setpoints to the global system:
		QVector3D newGlobalEndSetpoints = primeAxisToGlobal(primeSetpoints);

		// Create the required move actions in the global system:
		AMListAction3* endMoveActions = new AMListAction3(new AMListActionInfo3("Moving parameter controls for end",
		                                                                     "Moving parameter controls for end"),
													   AMListAction3::Parallel);

		endMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalXAxis_, newGlobalEndSetpoints.x()));
		endMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalYAxis_, newGlobalEndSetpoints.y()));
		endMoveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalZAxis_, newGlobalEndSetpoints.z()));


		action->addSubAction(endMoveActions);

		// Wait for the parameters to take
		AMListAction3* endWaitActions = new AMListAction3(new AMListActionInfo3("Waiting for parameter controls for end",
		                                                                     "Waiting for parameter controls for end"),
		                                               AMListAction3::Parallel);

		endWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(globalXAxis_, newGlobalEndSetpoints.x(), 2, AMControlWaitActionInfo::MatchWithinTolerance));
		endWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(globalYAxis_, newGlobalEndSetpoints.y(), 2, AMControlWaitActionInfo::MatchWithinTolerance));
		endWaitActions->addSubAction(AMActionSupport::buildControlWaitAction(globalZAxis_, newGlobalEndSetpoints.z(), 2, AMControlWaitActionInfo::MatchWithinTolerance));

		action->addSubAction(endWaitActions);
		// Save the previous velocity before we alter it
		lastSavedVelocity_ = systemVelocity_->value();

		// Setting the system velocity
		double velocity = qAbs(endPoint - startPoint) / deltaTime;
		action->addSubAction(AMActionSupport::buildControlMoveAction(systemVelocity_, velocity));
		action->addSubAction(AMActionSupport::buildControlWaitAction(systemVelocity_, velocity, 2, AMControlWaitActionInfo::MatchWithinTolerance));

		// Setting up the data recorder
		double requiredRate = HEXAPOD_RECORDER_POINTS_PER_MOVE / deltaTime;
		action->addSubAction(AMActionSupport::buildControlMoveAction(dataRecorderRate_, requiredRate));
		action->addSubAction(AMActionSupport::buildControlWaitAction(dataRecorderRate_, requiredRate, 2.0, AMControlWaitActionInfo::MatchWithinTolerance));

		action->addSubAction(AMActionSupport::buildControlMoveAction(dataRecorderStatus_, 1));
		action->addSubAction(AMActionSupport::buildControlWaitAction(dataRecorderStatus_, 1, 2.0, AMControlWaitActionInfo::MatchWithinTolerance));

	}

	return action;
}

AMAction3 * SGMHexapodTransformedAxis::createInitializeCoordinatedMovementActions()
{
	return 0;
}

AMAction3 * SGMHexapodTransformedAxis::createStartCoordinatedMovementActions()
{
	return  AMActionSupport::buildControlMoveAction(trajectoryStartControl_, 1);
}

AMAction3 * SGMHexapodTransformedAxis::createWaitForCompletionActions()
{
	AMListAction3* waitActions = new AMListAction3(new AMListActionInfo3("Waiting for coordinated move",
	                                                                     "Waiting for coordinated move"),
	                                               AMListAction3::Sequential);

	waitActions->addSubAction(AMActionSupport::buildControlWaitAction(trajectoryStartControl_, 0, lastDeltaTime_+1.5, AMControlWaitActionInfo::MatchWithinTolerance));

	AMListAction3 *recorderActions = new AMListAction3(new AMListActionInfo3("Waiting for data recorders to monitor", "Waiting for data recorders to monitor"), AMListAction3::Parallel);
	// Read action for the detector emulators for data recorders. These detectors are set to AMDetectorDefinitions::WaitRead, which will mean they will only read once they monitor.
	AMAction3 *hexapodXRecorderDetectorReadAction = AMBeamline::bl()->exposedDetectorByName("HexapodXRecorder")->createReadAction();
	hexapodXRecorderDetectorReadAction->setGenerateScanActionMessage(true);
	recorderActions->addSubAction(hexapodXRecorderDetectorReadAction);
	AMAction3 *hexapodYRecorderDetectorReadAction = AMBeamline::bl()->exposedDetectorByName("HexapodYRecorder")->createReadAction();
	hexapodYRecorderDetectorReadAction->setGenerateScanActionMessage(true);
	recorderActions->addSubAction(hexapodYRecorderDetectorReadAction);
	AMAction3 *hexapodZRecorderDetectorReadAction = AMBeamline::bl()->exposedDetectorByName("HexapodZRecorder")->createReadAction();
	hexapodZRecorderDetectorReadAction->setGenerateScanActionMessage(true);
	recorderActions->addSubAction(hexapodZRecorderDetectorReadAction);
	AMAction3 *hexapodTimeRecorderDetectorReadAction = AMBeamline::bl()->exposedDetectorByName("HexapodTimeRecorder")->createReadAction();
	hexapodTimeRecorderDetectorReadAction->setGenerateScanActionMessage(true);
	recorderActions->addSubAction(hexapodTimeRecorderDetectorReadAction);


	AMListAction3 *positionWaitActions = new AMListAction3(new AMListActionInfo3("Waiting for hexapod positions", "Waiting for hexapod positions"), AMListAction3::Parallel);
	// We need to add 1.5 seconds to the delta time, as the data recorder is slow
	AMAction3* positionWaitAction = AMActionSupport::buildControlWaitAction(this, lastEndPoint_, lastDeltaTime_+1.5, AMControlWaitActionInfo::MatchWithinTolerance);

	/*
	AMAction3* statusXWaitAction = AMActionSupport::buildControlWaitAction(globalXAxisStatus_, 0, lastDeltaTime_+1.5, AMControlWaitActionInfo::MatchWithinTolerance);
	AMAction3* statusYWaitAction = AMActionSupport::buildControlWaitAction(globalYAxisStatus_, 0, lastDeltaTime_+1.5, AMControlWaitActionInfo::MatchWithinTolerance);
	AMAction3* statusZWaitAction = AMActionSupport::buildControlWaitAction(globalZAxisStatus_, 0, lastDeltaTime_+1.5, AMControlWaitActionInfo::MatchWithinTolerance);
	*/

	positionWaitActions->addSubAction(positionWaitAction);
	/*
	positionWaitActions->addSubAction(statusXWaitAction);
	positionWaitActions->addSubAction(statusYWaitAction);
	positionWaitActions->addSubAction(statusZWaitAction);
	*/
	/*
	waitActions->addSubAction(positionWaitAction);
	waitActions->addSubAction(statusXWaitAction);
	waitActions->addSubAction(statusYWaitAction);
	waitActions->addSubAction(statusZWaitAction);
	*/

	AMListAction3 *centralParallelActions = new AMListAction3(new AMListActionInfo3("Parallel List of Recorder and Position Waits", "Parallel List of Recorder and Position Waits"), AMListAction3::Parallel);
	centralParallelActions->addSubAction(recorderActions);
	centralParallelActions->addSubAction(positionWaitActions);

	waitActions->addSubAction(centralParallelActions);

	// Return the data recorder to the off state (we need a clean up actions function adding to the AMControl API)
	waitActions->addSubAction(AMActionSupport::buildControlMoveAction(dataRecorderStatus_, 0));
	waitActions->addSubAction(AMActionSupport::buildControlWaitAction(dataRecorderStatus_, 0, 2.0, AMControlWaitActionInfo::MatchWithinTolerance));
	waitActions->addSubAction(AMActionSupport::buildControlMoveAction(systemVelocity_, lastSavedVelocity_));
	waitActions->addSubAction(AMActionSupport::buildControlWaitAction(systemVelocity_, lastSavedVelocity_, 2.0, AMControlWaitActionInfo::MatchWithinTolerance));

	return waitActions;
}

AMAction3 * SGMHexapodTransformedAxis::createMoveAction(double setpoint)
{
	AMListAction3* action = 0;

	if(globalXAxis_ && globalYAxis_ && globalZAxis_ && trajectoryStartControl_) {

		action = new AMListAction3(new AMListActionInfo3(QString("Moving %1").arg(name()),
														 QString("Moving %1 from %2 to %3")
														 .arg(value()).arg(setpoint)),
								   AMListAction3::Sequential);

		AMListAction3* moveActions = new AMListAction3(new AMListActionInfo3(QString("Moving %1").arg(name()),
																			 QString("Moving %1").arg(name())),
													   AMListAction3::Parallel);

		// Grab the current global positions:
		QVector3D currentGlobalSetpoints(globalXAxis_->setpoint(), globalYAxis_->setpoint(), globalZAxis_->setpoint());
		// Transform it to our system:
		QVector3D primeSetpoint = globalAxisToPrime(currentGlobalSetpoints);

		// Set the value in terms of our system based on the axis we are:
		switch(axis_) {
		case XAxis:
			primeSetpoint.setX(setpoint);
			break;
		case YAxis:
			primeSetpoint.setY(setpoint);
			break;
		case ZAxis:
			primeSetpoint.setZ(setpoint);

		}

		// Transform back the the global system:
		QVector3D newGlobalSetpoints = primeAxisToGlobal(primeSetpoint);

		// Create the required move actions in the global system:
		moveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalXAxis_, newGlobalSetpoints.x()));
		moveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalYAxis_, newGlobalSetpoints.y()));
		moveActions->addSubAction(AMActionSupport::buildControlMoveAction(globalZAxis_, newGlobalSetpoints.z()));


		action->addSubAction(moveActions);

		AMAction3* trajectoryStartAction = AMActionSupport::buildControlMoveAction(trajectoryStartControl_, 1);
		action->addSubAction(trajectoryStartAction);

		AMAction3* waitAction = AMActionSupport::buildControlWaitAction(this, setpoint, 100, AMControlWaitActionInfo::MatchWithinTolerance);

		AMAction3* waitXAction = AMActionSupport::buildControlWaitAction(globalXAxisStatus_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance);
		AMAction3* waitYAction = AMActionSupport::buildControlWaitAction(globalYAxisStatus_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance);
		AMAction3* waitZAction = AMActionSupport::buildControlWaitAction(globalZAxisStatus_, 0, 100, AMControlWaitActionInfo::MatchWithinTolerance);

		action->addSubAction(waitAction);
		action->addSubAction(waitXAction);
		action->addSubAction(waitYAction);
		action->addSubAction(waitZAction);
	}

	return action;
}

void SGMHexapodTransformedAxis::updateConnected()
{
	setConnected(globalXAxis_->isConnected() &&
				 globalYAxis_->isConnected() &&
				 globalZAxis_->isConnected() &&
				 globalXAxisFeedback_->isConnected() &&
				 globalYAxisFeedback_->isConnected() &&
				 globalZAxisFeedback_->isConnected());

	updateMinimumValue();
	updateMaximumValue();
}

void SGMHexapodTransformedAxis::updateValue()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_ &&
			globalXAxisFeedback_ && globalYAxisFeedback_ && globalZAxisFeedback_) {

		QVector3D globalValues(globalXAxisFeedback_->value(), globalYAxisFeedback_->value(), globalZAxisFeedback_->value());
		QVector3D primeValues = globalAxisToPrime(globalValues);

		double newValue = designatedAxisValue(primeValues);

		if(!withinTolerance(newValue)) {
			setValue(newValue);
		}
	}
}

void SGMHexapodTransformedAxis::updateMoving()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_ &&
			globalXAxisFeedback_ && globalYAxisFeedback_ && globalZAxisFeedback_) {
		bool isNowMoving = false;

		isNowMoving |= (globalXAxisFeedback_->isMoving() && affectedByMotionsInX());
		isNowMoving |= (globalYAxisFeedback_->isMoving() && affectedByMotionsInY());
		isNowMoving |= (globalZAxisFeedback_->isMoving() && affectedByMotionsInZ());


		if(isNowMoving != isMoving()) {
			setIsMoving(isNowMoving);
		}
	}
}

