#include "REIXSSampleMotor.h"

#include "beamline/CLS/CLSMDriveMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"

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

		AMErrorMon::alert(this, REIXS_SAMPLE_MOTOR_INVALID_DIRECTION, QString("REIXS Sample Motor initialized in vertical axis"));
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

void REIXSSampleMotor::setAngleOffset(double offset)
{
	angleOffset_ = offset;
	rotateSystem();
}

double REIXSSampleMotor::totalRotation() const
{
	if(connected_) {

		double combinedRotation = verticalRotationControl_->value() + angleOffset_;

		while(combinedRotation > 360) {
			combinedRotation -= 360;
		}

		while(combinedRotation < 0) {
			combinedRotation += 360;
		}
		return combinedRotation;
	}

	return 0;
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

void REIXSSampleMotor::updateMinimumValue()
{
	updateMinimumAndMaximum();
}

void REIXSSampleMotor::updateMaximumValue()
{
	updateMinimumAndMaximum();
}

void REIXSSampleMotor::onVerticalAxisRotated(double /*value*/)
{
	rotateSystem();
}
#include <QDebug>
AMAction3 * REIXSSampleMotor::createMoveAction(double setpoint)
{
	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3(QString("In-plane sample move"),
	                                                                   QString("In-place sample move")),
				                                 AMListAction3::Parallel);

	QVector3D primeVector(0,0,0);
	if(direction_ == AMMotorGroupObject::HorizontalMotion) {

		primeVector.setX(setpoint);
	} else if(direction_ == AMMotorGroupObject::NormalMotion ) {

		primeVector.setY(setpoint);
	}
	QVector3D globalVector = primeSystemToGlobal(primeVector);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(horizontalTranslationControl_,
	                                                                 globalVector.x()));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(normalTranslationControl_,
	                                                                 globalVector.y()));

	//return coordinatedMoveAction;
	qDebug() << QString("Would have moved global axes to [%1, %2]").arg(globalVector.x()).arg(globalVector.y());

	return 0;

}

void REIXSSampleMotor::rotateSystem()
{
	if(isConnected()) {

		QVector3D vX(1, 0, 0);
		QVector3D vY(0, 1, 0);
		QVector3D vZ(0, 0, 1);

		QQuaternion qX = QQuaternion::fromAxisAndAngle(vX, 0);
		QQuaternion qY = QQuaternion::fromAxisAndAngle(vY, 0);
		QQuaternion qZ = QQuaternion::fromAxisAndAngle(vZ, totalRotation());

		rotationQuaternion_ = (qX * qY) * qZ;
		rotationQuaternion_.normalize();

		updateMinimumAndMaximum();
		updateValue();
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

void REIXSSampleMotor::updateMinimumAndMaximum()
{
	if(isConnected()) {
		double xMax = horizontalTranslationControl_->maximumValue();
		double xMin = horizontalTranslationControl_->minimumValue();
		double yMax = normalTranslationControl_->maximumValue();
		double yMin = normalTranslationControl_->minimumValue();

		qDebug() << "\tNative maxima and minima:";
		qDebug() << "\t\tX Max:" << xMax;
		qDebug() << "\t\tX Min:" << xMin;
		qDebug() << "\t\tY Max:" << yMax;
		qDebug() << "\t\tY Min:" << yMin;

		double rotation = totalRotation();

		qDebug() << "\tRotated By:";
		qDebug() << "\t\t"<<rotation;

		qDebug() << "\tDirection:";
		if(direction_ == AMMotorGroupObject::HorizontalMotion) {
			qDebug() << "\t\tHorizontal (X)";
		} else {
			qDebug() << "\t\tNormal (Y)";
		}

		// Obtain the bounding values for each quadrant of the cartesian system.

		double firstQuadrantBounds;
		double secondQuadrantBounds;
		double thirdQuadrantBounds;
		double fourthQuadrantBounds;

		if(direction_ == AMMotorGroupObject::HorizontalMotion) {
			firstQuadrantBounds = boundsForQuardrant(xMax, yMax, rotation);
			secondQuadrantBounds = boundsForQuardrant(yMax, xMin, rotation);
			thirdQuadrantBounds = boundsForQuardrant(xMin, yMin, rotation);
			fourthQuadrantBounds = boundsForQuardrant(yMin, xMax, rotation);

		} else {
			firstQuadrantBounds = boundsForQuardrant(yMax, xMin, rotation);
			secondQuadrantBounds = boundsForQuardrant(xMin, yMin, rotation);
			thirdQuadrantBounds = boundsForQuardrant(yMin, xMax, rotation);
			fourthQuadrantBounds = boundsForQuardrant(xMax, yMax, rotation);
		}

		if((rotation >= 0 && rotation < 90) || (rotation >= 180 && rotation < 270)) {

			if(direction_ == AMMotorGroupObject::HorizontalMotion) {

				// first and third quadrants contain the X bounds
				setMaximumValue(qMax(firstQuadrantBounds, thirdQuadrantBounds));
				setMinimumValue(qMin(firstQuadrantBounds, thirdQuadrantBounds));
			} else if(direction_ == AMMotorGroupObject::NormalMotion) {

				// second and fourth quadrants contain the Y bounds
				setMaximumValue(qMax(secondQuadrantBounds, fourthQuadrantBounds));
				setMinimumValue(qMin(secondQuadrantBounds, fourthQuadrantBounds));
			}

		} else {

			if(direction_ == AMMotorGroupObject::HorizontalMotion) {

				// second and fourth quadrants contain the X bounds
				setMaximumValue(qMax(secondQuadrantBounds, fourthQuadrantBounds));
				setMinimumValue(qMin(secondQuadrantBounds, fourthQuadrantBounds));
			} else if(direction_ == AMMotorGroupObject::NormalMotion) {

				// first and third quadrants contain the Y bounds
				setMaximumValue(qMax(firstQuadrantBounds, thirdQuadrantBounds));
				setMinimumValue(qMin(firstQuadrantBounds, thirdQuadrantBounds));
			}
		}

		qDebug() << "\tCalculated max and min:";
		qDebug() << "\t\tMaximum:"<<maximumValue();
		qDebug() << "\t\tMinimum:"<<minimumValue();

	}
}

double REIXSSampleMotor::boundsForQuardrant(double firstBound,
                                            double secondBound,
                                            double rotationAngle)
{
	/// Angle from the axis rotated from, to the point(firstBound, secondBound)
	double angleToCorner = atan(secondBound / firstBound);

	if(rotationAngle <= angleToCorner) {

		return firstBound / cos(rotationAngle);
	} else {

		return secondBound / cos(90 - rotationAngle);
	}

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


