#include "REIXSSampleMotor.h"

#include "beamline/CLS/CLSMDriveMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"
#include "util/AMGeometry.h"
#include "util/AMRange.h"
#include <math.h>
REIXSSampleMotor::REIXSSampleMotor(AMMotorGroupObject::MotionDirection direction,
                                   const QString& name,
                                   const QString& units,
		                           AMControl* horizontalTranslationControl,
		                           AMControl* normalTranslationControl,
		                           AMControl* verticalRotationControl,
		                           QObject *parent,
		                           const QString& description) :

    AMPseudoMotorControl(name, units, parent, description)
{
	direction_ = direction;

	if(direction_ == AMMotorGroupObject::VerticalMotion) {

		AMErrorMon::alert(this,
		                  REIXS_SAMPLE_MOTOR_INVALID_DIRECTION,
		                  QString("REIXS Sample Motor initialized in vertical axis"));
	}

	horizontalTranslationControl_ = horizontalTranslationControl;
	normalTranslationControl_ = normalTranslationControl;
	verticalRotationControl_  = verticalRotationControl;

	angleOffset_ = 0;
	connectedOnce_ = false;

	addChildControl(horizontalTranslationControl_);
	addChildControl(normalTranslationControl_);
	addChildControl(verticalRotationControl_);

	connect(verticalRotationControl_, SIGNAL(valueChanged(double)),
	        this, SLOT(onVerticalAxisRotated(double)));
}


double REIXSSampleMotor::angleOffset() const
{
	return angleOffset_;
}

double REIXSSampleMotor::totalRotation() const
{
	if(connected_) {

		double combinedRotation = verticalRotationControl_->value() + angleOffset_;

		// Ensure values lie within 0 and 360
		return fmod(combinedRotation, 360);
	}	

	return 0;
}


AMMotorGroupObject::MotionDirection REIXSSampleMotor::direction() const
{
	return direction_;
}

bool REIXSSampleMotor::canMeasure() const
{
	return shouldMeasure() &&
	        isConnected();
}

bool REIXSSampleMotor::shouldMeasure() const
{
	if(horizontalTranslationControl_ && normalTranslationControl_) {
		return horizontalTranslationControl_->shouldMeasure() &&
				normalTranslationControl_->shouldMeasure();
	} else {

		return false;
	}
}

bool REIXSSampleMotor::canMove() const
{
	return shouldMove() &&
	        isConnected();
}

bool REIXSSampleMotor::shouldMove() const
{
	if(horizontalTranslationControl_ && normalTranslationControl_) {

		return horizontalTranslationControl_->shouldMove() &&
		        normalTranslationControl_->shouldMove();
	} else {

		return false;
	}
}

bool REIXSSampleMotor::canStop() const
{
	return shouldStop() &&
	        isConnected();
}

bool REIXSSampleMotor::shouldStop() const
{
	if(horizontalTranslationControl_ && normalTranslationControl_) {

		return horizontalTranslationControl_->shouldStop() &&
		        normalTranslationControl_->shouldStop();
	} else {

		return false;
	}
}

bool REIXSSampleMotor::validValue(double value) const
{
	QVector3D globalVector = createGlobalMovementVector(value);

	AMRange xRange(REIXS_SAMPLE_HORIZONTAL_MIN, REIXS_SAMPLE_HORIZONTAL_MAX);
	AMRange yRange(REIXS_SAMPLE_NORMAL_MIN, REIXS_SAMPLE_NORMAL_MAX);

	return xRange.withinRange(globalVector.x()) && yRange.withinRange(globalVector.y());
}

void REIXSSampleMotor::setAngleOffset(double offset)
{
	angleOffset_ = offset;
	rotateSystem();
}

void REIXSSampleMotor::updateConnected()
{
	setConnected(direction_ != AMMotorGroupObject::VerticalMotion
	        && horizontalTranslationControl_ && horizontalTranslationControl_->isConnected()
	        && normalTranslationControl_ && normalTranslationControl_->isConnected()
	        && verticalRotationControl_ && verticalRotationControl_->isConnected());

	if(!connectedOnce_ && isConnected()) {

		connectedOnce_ = true;
		rotateSystem();
		updateValue();
	}
}

void REIXSSampleMotor::updateValue()
{
	if(isConnected()) {

		QVector3D globalVector = QVector3D(horizontalTranslationControl_->value(),
		                                   normalTranslationControl_->value(),
		                                   0);

		QVector3D primeVector = globalSystemToPrime(globalVector);

		setValue(valueForDirection(primeVector));
	}
}

void REIXSSampleMotor::updateMoving()
{
	// We include the vertical rotation because you really shouldn't me moving
	// in plane while the plane is being rotated.
	setIsMoving(horizontalTranslationControl_->isMoving() ||
	            normalTranslationControl_->isMoving() ||
	            verticalRotationControl_->isMoving());
}

void REIXSSampleMotor::onVerticalAxisRotated(double /*value*/)
{
	rotateSystem();
}

void REIXSSampleMotor::onMoveFailed(QObject *action)
{
	AMPseudoMotorControl::onMoveFailed(action);
	updateMinimumAndMaximum();
}

void REIXSSampleMotor::onMoveSucceeded(QObject *action)
{
	AMPseudoMotorControl::onMoveSucceeded(action);
	updateMinimumAndMaximum();
}

AMAction3 * REIXSSampleMotor::createMoveAction(double setpoint)
{

	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3(QString("In-plane sample move"),
	                                                                   QString("In-plane sample move")),
				                                 AMListAction3::Parallel);


	QVector3D targetGlobalVector = createGlobalMovementVector(setpoint);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(horizontalTranslationControl_,
	                                                                 targetGlobalVector.x()));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(normalTranslationControl_,
	                                                                 targetGlobalVector.y()));

	return moveAction;
}

void REIXSSampleMotor::rotateSystem()
{
	if(isConnected()) {

		QVector3D vX(1, 0, 0);
		QVector3D vY(0, 1, 0);
		QVector3D vZ(0, 0, 1);

		QQuaternion qX = QQuaternion::fromAxisAndAngle(vX, 0);
		QQuaternion qY = QQuaternion::fromAxisAndAngle(vY, 0);
		QQuaternion qZ = QQuaternion::fromAxisAndAngle(vZ, -totalRotation());

		rotationQuaternion_ = (qX * qY) * qZ;
		rotationQuaternion_.normalize();

		updateValue();
		updateMinimumAndMaximum();
	}

}

QVector3D REIXSSampleMotor::globalSystemToPrime(const QVector3D &globalVector) const
{
	return rotationQuaternion_.rotatedVector(globalVector);
}

QVector3D REIXSSampleMotor::primeSystemToGlobal(const QVector3D &primeVector) const
{
	return rotationQuaternion_.conjugate().rotatedVector(primeVector);
}

QVector3D REIXSSampleMotor::createGlobalMovementVector(double primeSetpoint) const
{
	// Get the current position in the global system
	QVector3D startingGlobalVector(horizontalTranslationControl_->value(),
	                               normalTranslationControl_->value(),
	                               0);

	// Translate that into the prime system
	QVector3D startingPrimeVector = globalSystemToPrime(startingGlobalVector);

	// Update the value to the setpoint depending on which axis we are.
	if(direction_ == AMMotorGroupObject::HorizontalMotion) {

		startingPrimeVector.setX(primeSetpoint);
	} else if(direction_ == AMMotorGroupObject::NormalMotion ) {

		startingPrimeVector.setY(primeSetpoint);
	}

	// Return the prime motion in terms of the global system
	return primeSystemToGlobal(startingPrimeVector);
}

double REIXSSampleMotor::valueForDirection(const QVector3D &vector)
{
	switch (direction_) {

	case AMMotorGroupObject::HorizontalMotion:
		return vector.x();
	case AMMotorGroupObject::VerticalMotion:
		return vector.z();		// This shouldn't happen, but we include it anyway.
	case AMMotorGroupObject::NormalMotion:
		return vector.y();
	}

	return 0;
}

void REIXSSampleMotor::updateMinimumAndMaximum()
{
	if(!isConnected()) {
		return;
	}

	QVector3D currentGlobalPosition(horizontalTranslationControl_->value(),
	                                normalTranslationControl_->value(),
	                                0);

	QVector3D currentPrimePosition = globalSystemToPrime(currentGlobalPosition);
	double rotationRadians = AMGeometry::degreesToRadians(totalRotation());
	double sinRotation = sin(rotationRadians);
	double cosRotation = cos(rotationRadians);

	// All of the ranges have two potential limits, one constrained by the global
	// X limit, one by the global Y limit. We calculate both, then figure out which
	// is the most restrictive.
	double min1 = -DBL_MAX;
	double min2 = -DBL_MAX;

	double max1 = DBL_MAX;
	double max2 = DBL_MAX;

	if(direction_ == AMMotorGroupObject::HorizontalMotion) {

		// Avoid the divide by zero cases (which represents the point at which
		// an axis is parrallel to your prime axis and the distance till you intersect
		// it is infinite.
		if(sinRotation == 0) {

			min1 = (REIXS_SAMPLE_HORIZONTAL_MIN - currentPrimePosition.y() * sinRotation) / cosRotation;
			max1 = (REIXS_SAMPLE_HORIZONTAL_MAX - currentPrimePosition.y() * sinRotation) / cosRotation;

		} else if (cosRotation == 0) {

			min2 = (currentPrimePosition.y() * cosRotation - REIXS_SAMPLE_NORMAL_MIN) / sinRotation;
			max2 = (currentPrimePosition.y() * cosRotation - REIXS_SAMPLE_NORMAL_MIN) / sinRotation;

		} else {
			min1 = (REIXS_SAMPLE_HORIZONTAL_MIN - currentPrimePosition.y() * sinRotation) / cosRotation;
			min2 = (currentPrimePosition.y() * cosRotation - REIXS_SAMPLE_NORMAL_MIN) / sinRotation;

			max1 = (REIXS_SAMPLE_HORIZONTAL_MAX - currentPrimePosition.y() * sinRotation) / cosRotation;
			max2 = (currentPrimePosition.y() * cosRotation - REIXS_SAMPLE_NORMAL_MIN) / sinRotation;

		}

	} else if (direction_ == AMMotorGroupObject::NormalMotion) {


		if(sinRotation == 0) {

			min2 = (REIXS_SAMPLE_NORMAL_MIN + currentPrimePosition.x() * sinRotation) / cosRotation;
			max2 = (REIXS_SAMPLE_NORMAL_MAX + currentPrimePosition.x() * sinRotation) / cosRotation;
		} else if(cosRotation == 0) {

			min1 = (REIXS_SAMPLE_NORMAL_MIN - currentPrimePosition.x() * cosRotation) / sinRotation;
			max1 = (REIXS_SAMPLE_HORIZONTAL_MAX - currentPrimePosition.x() * cosRotation) / sinRotation;
		} else {

			min1 = (REIXS_SAMPLE_NORMAL_MIN - currentPrimePosition.x() * cosRotation) / sinRotation;
			min2 = (REIXS_SAMPLE_NORMAL_MIN + currentPrimePosition.x() * sinRotation) / cosRotation;

			max1 = (REIXS_SAMPLE_HORIZONTAL_MAX - currentPrimePosition.x() * cosRotation) / sinRotation;
			max2 = (REIXS_SAMPLE_NORMAL_MAX + currentPrimePosition.x() * sinRotation) / cosRotation;
		}

	}

	// The limit is the more restrictive for each value
	setMaximumValue(qMin(max1, max2));
	setMinimumValue(qMax(min1, min2));
}








