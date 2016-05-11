#include "AM3DCoordinatedSystemControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

AM3DCoordinatedSystemControl::AM3DCoordinatedSystemControl(AxisDesignation axis,
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

bool AM3DCoordinatedSystemControl::shouldMeasure() const
{

	return globalXAxis_ && globalXAxis_->shouldMeasure() &&
			globalYAxis_ && globalYAxis_->shouldMeasure() &&
			globalZAxis_ && globalZAxis_->shouldMeasure();
}

bool AM3DCoordinatedSystemControl::shouldMove() const
{
	return globalXAxis_ && globalXAxis_->shouldMove() &&
			globalYAxis_ && globalYAxis_->shouldMove() &&
			globalZAxis_ && globalZAxis_->shouldMove();
}

bool AM3DCoordinatedSystemControl::shouldStop() const
{
	return globalXAxis_ && globalXAxis_->shouldStop() &&
			globalYAxis_ && globalYAxis_->shouldStop() &&
			globalZAxis_ && globalZAxis_->shouldStop();
}

bool AM3DCoordinatedSystemControl::canMeasure() const
{
	return globalXAxis_ && globalXAxis_->canMeasure() &&
			globalYAxis_ && globalYAxis_->canMeasure() &&
			globalZAxis_ && globalZAxis_->canMeasure();
}

bool AM3DCoordinatedSystemControl::canMove() const
{
	return globalXAxis_ && globalXAxis_->canMove() &&
			globalYAxis_ && globalYAxis_->canMove() &&
			globalZAxis_ && globalZAxis_->canMove();
}

bool AM3DCoordinatedSystemControl::canStop() const
{
	return globalXAxis_ && globalXAxis_->canStop() &&
			globalYAxis_ && globalYAxis_->canStop() &&
			globalZAxis_ && globalZAxis_->canStop();
}

bool AM3DCoordinatedSystemControl::addChildControl(AMControl *control)
{
	AMPseudoMotorControl::addChildControl(control);

	if(control) {
		connect(control, SIGNAL(setpointChanged(double)), this, SLOT(updateSetpoint()));
	}
}

void AM3DCoordinatedSystemControl::updateConnected()
{
	setConnected(globalXAxis_ && globalXAxis_->isConnected() &&
			globalYAxis_ && globalYAxis_->isConnected() &&
			globalZAxis_ && globalZAxis_->isConnected());

	updateMinimumValue();
	updateMaximumValue();
}

void AM3DCoordinatedSystemControl::updateValue()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		QVector3D globalValues(globalXAxis_->value(), globalYAxis_->value(), globalZAxis_->value());
		QVector3D primeValues = globalAxisToPrime(globalValues);

		double newValue = designatedAxisValue(primeValues);

		setValue(newValue);
	}
}

void AM3DCoordinatedSystemControl::updateMoving()
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

void AM3DCoordinatedSystemControl::updateMinimumValue()
{
	// These have to be done at the same time, as a tranform might lead us to
	// a smaller max than min, in which case we have to reverse them.
	updateMaximumAndMinimumValues();
}

void AM3DCoordinatedSystemControl::updateMaximumValue()
{
	// These have to be done at the same time, as a tranform might lead us to
	// a smaller max than min, in which case we have to reverse them.
	updateMaximumAndMinimumValues();
}

void AM3DCoordinatedSystemControl::updateMaximumAndMinimumValues()
{
	setMinimumValue(-100);
	setMaximumValue(100);
}


void AM3DCoordinatedSystemControl::updateSetpoint()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {
		QVector3D globalSetpoints(globalXAxis_->setpoint(), globalYAxis_->setpoint(), globalZAxis_->setpoint());
		QVector3D primeSetpoints = globalAxisToPrime(globalSetpoints);

		double newSetpoint = designatedAxisValue(primeSetpoints);

		setSetpoint(newSetpoint);
	}
}

bool AM3DCoordinatedSystemControl::affectedByMotionsInX() const
{
	QVector3D testStartPrimePosition(0,0,0);
	QVector3D testGlobalPosition = primeAxisToGlobal(testStartPrimePosition);

	testGlobalPosition.setX(1);
	QVector3D testEndPrimePosition = globalAxisToPrime(testGlobalPosition);

	double thisAxisStartPosition = designatedAxisValue(testStartPrimePosition);
	double thisAxisEndPosition = designatedAxisValue(testEndPrimePosition);

	return (qAbs(thisAxisStartPosition - thisAxisEndPosition) > 0.001);

}

bool AM3DCoordinatedSystemControl::affectedByMotionsInY() const
{
	QVector3D testStartPrimePosition(0,0,0);
	QVector3D testGlobalPosition = primeAxisToGlobal(testStartPrimePosition);

	testGlobalPosition.setY(1);
	QVector3D testEndPrimePosition = globalAxisToPrime(testGlobalPosition);

	double thisAxisStartPosition = designatedAxisValue(testStartPrimePosition);
	double thisAxisEndPosition = designatedAxisValue(testEndPrimePosition);

	return (qAbs(thisAxisStartPosition - thisAxisEndPosition) > 0.001);
}

bool AM3DCoordinatedSystemControl::affectedByMotionsInZ() const
{

	QVector3D testStartPrimePosition(0,0,0);
	QVector3D testGlobalPosition = primeAxisToGlobal(testStartPrimePosition);

	testGlobalPosition.setZ(1);
	QVector3D testEndPrimePosition = globalAxisToPrime(testGlobalPosition);

	double thisAxisStartPosition = designatedAxisValue(testStartPrimePosition);
	double thisAxisEndPosition = designatedAxisValue(testEndPrimePosition);

	return (qAbs(thisAxisStartPosition - thisAxisEndPosition) > 0.001);
}

AMAction3 * AM3DCoordinatedSystemControl::createMoveAction(double setpoint)
{
	AMListAction3* action = 0;

	if(globalXAxis_ && globalYAxis_ && globalZAxis_) {

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
	}

	return action;
}

double AM3DCoordinatedSystemControl::designatedAxisValue(const QVector3D &vector) const
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
