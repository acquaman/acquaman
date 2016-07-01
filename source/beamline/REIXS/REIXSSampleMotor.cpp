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
	connect(horizontalTranslationControl_, SIGNAL(movingChanged(bool)),
	        this, SLOT(updateMinimumAndMaximum()));
	connect(normalTranslationControl_, SIGNAL(movingChanged(bool)),
	        this, SLOT(updateMinimumAndMaximum()));
	connect(verticalRotationControl_, SIGNAL(movingChanged(bool)),
	        this, SLOT(updateMinimumAndMaximum()));

	connect(horizontalTranslationControl_, SIGNAL(setpointChanged(double)),
	        this, SLOT(onHorizontalTranslationSetpointChanged(double)));
	connect(normalTranslationControl_, SIGNAL(setpointChanged(double)),
	        this, SLOT(onNormalTranslationSetpointChanged(double)));

	updateConnected();
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

	AMRange xRange(horizontalTranslationControl_->minimumValue(), horizontalTranslationControl_->maximumValue());
	AMRange yRange(normalTranslationControl_->minimumValue(), normalTranslationControl_->maximumValue());

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
		updateMinimumAndMaximum();

		updatePerpendicularSetpoint(horizontalTranslationControl_->setpoint(), normalTranslationControl_->setpoint());
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

			min1 = horizontalTranslationControl_->minimumValue();
			max1 = horizontalTranslationControl_->maximumValue();

		} else if (cosRotation == 0) {

			min2 = normalTranslationControl_->minimumValue();
			max2 = normalTranslationControl_->maximumValue();

		} else {
			min1 = (horizontalTranslationControl_->minimumValue() + currentPrimePosition.y() * sinRotation) / cosRotation;
			min2 = (normalTranslationControl_->maximumValue() - currentPrimePosition.y() * cosRotation) / sinRotation;

			max1 = (horizontalTranslationControl_->maximumValue() + currentPrimePosition.y() * sinRotation) / cosRotation;
			max2 = (normalTranslationControl_->minimumValue() - currentPrimePosition.y() * cosRotation) / sinRotation;

		}

	} else if (direction_ == AMMotorGroupObject::NormalMotion) {


		if(sinRotation == 0) {

			min2 = normalTranslationControl_->minimumValue();
			max2 = normalTranslationControl_->maximumValue();

		} else if(cosRotation == 0) {

			min1 = -(horizontalTranslationControl_->maximumValue());
			max1 = -(horizontalTranslationControl_->minimumValue());

		} else {

			min1 = (currentPrimePosition.x() * cosRotation - horizontalTranslationControl_->minimumValue()) / sinRotation;
			min2 = (normalTranslationControl_->minimumValue() - currentPrimePosition.x() * sinRotation) / cosRotation;

			max1 = (currentPrimePosition.x() * cosRotation - horizontalTranslationControl_->maximumValue()) / sinRotation;
			max2 = (normalTranslationControl_->maximumValue() - currentPrimePosition.x() * sinRotation) / cosRotation;
		}

	}




	// The limit is the more restrictive for each value
	if(min1 > max1) {

		if(min2 > max2) {
			setMaximumValue(qMin(min1, min2));
			setMinimumValue(qMax(max1, max2));

		} else {

			setMaximumValue(qMin(min1, max2));
			setMinimumValue(qMax(max1, min2));
		}
	} else {

		if(min2 > max2) {
			setMaximumValue(qMin(max1, min2));
			setMinimumValue(qMax(max2, min1));
		} else {

			setMaximumValue(qMin(max1, max2));
			setMinimumValue(qMax(min1, min2));
		}
	}
}

void REIXSSampleMotor::onHorizontalTranslationSetpointChanged(double setpoint)
{
	updatePerpendicularSetpoint(setpoint, normalTranslationControl_->setpoint());
}

void REIXSSampleMotor::onNormalTranslationSetpointChanged(double setpoint)
{
	updatePerpendicularSetpoint(horizontalTranslationControl_->setpoint(), setpoint);
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

	QVector3D startingPrimeVector;

	// Update the value to the setpoint depending on which axis we are.
	if(direction_ == AMMotorGroupObject::HorizontalMotion) {

		startingPrimeVector.setX(primeSetpoint);
		startingPrimeVector.setY(perpendicularAxisSetpoint_);
	} else if(direction_ == AMMotorGroupObject::NormalMotion ) {

		startingPrimeVector.setY(primeSetpoint);
		startingPrimeVector.setX(perpendicularAxisSetpoint_);
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
#include <QDebug>
void REIXSSampleMotor::updatePerpendicularSetpoint(double horizontalSetpoint, double normalSetpoint)
{
	QVector3D globalVector(
	            horizontalSetpoint,
	            normalSetpoint,
	            0);

	QVector3D primeVector = globalSystemToPrime(globalVector);

	if (direction_ == AMMotorGroupObject::HorizontalMotion) {

		perpendicularAxisSetpoint_ = primeVector.y();

	} else if (direction_ == AMMotorGroupObject::NormalMotion) {

		perpendicularAxisSetpoint_ = primeVector.x();
	}
	qDebug() << "\tUpdated perpendicular setpoint of" << name() << "to" << perpendicularAxisSetpoint_;

}



