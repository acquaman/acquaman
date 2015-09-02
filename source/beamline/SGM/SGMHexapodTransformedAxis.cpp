#include "SGMHexapodTransformedAxis.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPVControl.h"

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
													 const QString &name,
													 const QString &units,
													 QObject *parent,
													 const QString &description)
	: AM3DRotatedSystemControl(axis, globalXAxisSetpoint, globalYAxisSetpoint, globalZAxisSetpoint, name, units, parent, description )
{
	trajectoryStartControl_ = trajectoryStartControl;
	globalXAxisFeedback_ = globalXAxisFeedback;
	globalYAxisFeedback_ = globalYAxisFeedback;
	globalZAxisFeedback_ = globalZAxisFeedback;

	globalXAxisStatus_ = globalXAxisStatus;
	globalYAxisStatus_ = globalYAxisStatus;
	globalZAxisStatus_ = globalZAxisStatus;

	addChildControl(globalXAxisFeedback_);
	addChildControl(globalYAxisFeedback_);
	addChildControl(globalZAxisFeedback_);

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
