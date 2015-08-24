#include "AM3DMotionPseudoMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"


#include <QDebug>
AM3DMotionPseudoMotorControl::AM3DMotionPseudoMotorControl(AxisDesignation axis,
														   AMControl* globalXAxis,
														   AMControl* globalYAxis,
														   AMControl* globalZAxis,
														   const QString &name,
														   const QString &units,
														   QObject *parent,
														   const QString &description) :
	AMPseudoMotorControl(name, units, parent, description)
{
	axis_ = axis;
	globalXAxis_ = globalXAxis;
	globalYAxis_ = globalYAxis;
	globalZAxis_ = globalZAxis;

	setAllowsMovesWhileMoving(false);

	addChildControl(globalXAxis_);
	addChildControl(globalYAxis_);
	addChildControl(globalZAxis_);

	setTolerance(qMax(qMax(globalXAxis_->tolerance(), globalYAxis_->tolerance()), globalZAxis_->tolerance()));

	updateStates();
}

bool AM3DMotionPseudoMotorControl::shouldMeasure() const
{

	return globalXAxis_ && globalXAxis_->shouldMeasure() &&
			globalYAxis_ && globalYAxis_->shouldMeasure() &&
			globalZAxis_ && globalZAxis_->shouldMeasure();
}

bool AM3DMotionPseudoMotorControl::shouldMove() const
{
	return globalXAxis_ && globalXAxis_->shouldMove() &&
			globalYAxis_ && globalYAxis_->shouldMove() &&
			globalZAxis_ && globalZAxis_->shouldMove();
}

bool AM3DMotionPseudoMotorControl::shouldStop() const
{
	return globalXAxis_ && globalXAxis_->shouldStop() &&
			globalYAxis_ && globalYAxis_->shouldStop() &&
			globalZAxis_ && globalZAxis_->shouldStop();
}

bool AM3DMotionPseudoMotorControl::canMeasure() const
{
	return globalXAxis_ && globalXAxis_->canMeasure() &&
			globalYAxis_ && globalYAxis_->canMeasure() &&
			globalZAxis_ && globalZAxis_->canMeasure();
}

bool AM3DMotionPseudoMotorControl::canMove() const
{
	return globalXAxis_ && globalXAxis_->canMove() &&
			globalYAxis_ && globalYAxis_->canMove() &&
			globalZAxis_ && globalZAxis_->canMove();
}

bool AM3DMotionPseudoMotorControl::canStop() const
{
	return globalXAxis_ && globalXAxis_->canStop() &&
			globalYAxis_ && globalYAxis_->canStop() &&
			globalZAxis_ && globalZAxis_->canStop();
}

void AM3DMotionPseudoMotorControl::addChildControl(AMControl *control)
{
	AMPseudoMotorControl::addChildControl(control);

	if(control) {
		connect(control, SIGNAL(setpointChanged(double)), this, SLOT(updateSetpoint()));
	}
}

void AM3DMotionPseudoMotorControl::updateConnected()
{
	setConnected(globalXAxis_ && globalXAxis_->isConnected() &&
			globalYAxis_ && globalYAxis_->isConnected() &&
			globalZAxis_ && globalZAxis_->isConnected());

	updateMinimumValue();
	updateMaximumValue();
}

void AM3DMotionPseudoMotorControl::updateValue()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		QVector3D globalValues(globalXAxis_->value(), globalYAxis_->value(), globalZAxis_->value());
		QVector3D primeValues = globalAxisToPrime(globalValues);

		double newValue = designatedAxisValue(primeValues);

		if(!withinTolerance(newValue)) {
			setValue(newValue);
		}
	}
}

void AM3DMotionPseudoMotorControl::updateMoving()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		bool isNowMoving = false;

		isNowMoving |= (globalXAxis_->isMoving() && affectedByMotionsInX());
		isNowMoving |= (globalYAxis_->isMoving() && affectedByMotionsInY());
		isNowMoving |= (globalZAxis_->isMoving() && affectedByMotionsInZ());

		if(isNowMoving != isMoving()) {
			setIsMoving(isNowMoving);
		}
	}
}

void AM3DMotionPseudoMotorControl::updateMinimumValue()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		QVector3D globalMinimumValues(globalXAxis_->minimumValue(), globalYAxis_->minimumValue(), globalZAxis_->minimumValue());
		QVector3D primeMinimumValues = globalAxisToPrime(globalMinimumValues);

		double newMinimumValue = designatedAxisValue(primeMinimumValues);

		if(qAbs(minimumValue() - newMinimumValue) > tolerance()) {
			setMinimumValue(newMinimumValue);
		}
	}
}

void AM3DMotionPseudoMotorControl::updateMaximumValue()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		QVector3D globalMaximumValues(globalXAxis_->maximumValue(), globalYAxis_->maximumValue(), globalZAxis_->maximumValue());
		QVector3D primeMaximumValues = globalAxisToPrime(globalMaximumValues);

		double newMaximumValue = designatedAxisValue(primeMaximumValues);

		if(qAbs(maximumValue() - newMaximumValue) > tolerance()) {
			setMaximumValue(newMaximumValue);
		}
	}
}

void AM3DMotionPseudoMotorControl::updateSetpoint()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		QVector3D globalSetpoints(globalXAxis_->setpoint(), globalYAxis_->setpoint(), globalZAxis_->setpoint());
		QVector3D primeSetpoints = globalAxisToPrime(globalSetpoints);

		double newSetpoint = designatedAxisValue(primeSetpoints);

		if(qAbs(setpoint() - newSetpoint) > tolerance()) {
			setSetpoint(newSetpoint);
		}
	}
}

bool AM3DMotionPseudoMotorControl::affectedByMotionsInX() const
{
	QVector3D globalXMotionVector(10, 0, 0);
	QVector3D primeXMotionVector = globalAxisToPrime(globalXMotionVector);

	double thisAxisValue = designatedAxisValue(primeXMotionVector);

	return (qAbs(thisAxisValue) > 0.0001);

}

bool AM3DMotionPseudoMotorControl::affectedByMotionsInY() const
{

	QVector3D globalYMotionVector(0, 10, 0);
	QVector3D primeYMotionVector = globalAxisToPrime(globalYMotionVector);

	double thisAxisValue = designatedAxisValue(primeYMotionVector);

	return (qAbs(thisAxisValue) > 0.0001);
}

bool AM3DMotionPseudoMotorControl::affectedByMotionsInZ() const
{

	QVector3D globalZMotionVector(0, 0, 10);
	QVector3D primeZMotionVector = globalAxisToPrime(globalZMotionVector);

	double thisAxisValue = designatedAxisValue(primeZMotionVector);

	return (qAbs(thisAxisValue) > 0.0001);
}

AMAction3 * AM3DMotionPseudoMotorControl::createMoveAction(double setpoint)
{
	AMListAction3* action = 0;

	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {

		action = new AMListAction3(new AMListActionInfo3(QString("Moving %1").arg(name()),
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
		if(qAbs(globalXAxis_->setpoint() - newGlobalSetpoints.x()) < tolerance()) {
			action->addSubAction(AMActionSupport::buildControlMoveAction(globalXAxis_, newGlobalSetpoints.x()));
		}

		if(qAbs(globalYAxis_->setpoint() - newGlobalSetpoints.y()) < tolerance()) {
			action->addSubAction(AMActionSupport::buildControlMoveAction(globalYAxis_, newGlobalSetpoints.y()));
		}

		if(qAbs(globalZAxis_->setpoint() - newGlobalSetpoints.z()) < tolerance()) {
			action->addSubAction(AMActionSupport::buildControlMoveAction(globalZAxis_, newGlobalSetpoints.z()));
		}

	}

	return action;
}

double AM3DMotionPseudoMotorControl::designatedAxisValue(const QVector3D &vector) const
{
	switch(axis_) {
	case XAxis:
		return vector.x();
	case YAxis:
		return vector.y();
	case ZAxis:
		return vector.z();
	}

	return 0;
}

