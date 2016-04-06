#include "REIXSSampleMotor.h"

#include "beamline/CLS/CLSMDriveMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"
#include <QDebug>
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


AMMotorGroupObject::MotionDirection REIXSSampleMotor::direction() const
{
	return direction_;
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

AMAction3 * REIXSSampleMotor::createMoveAction(double setpoint)
{

	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3(QString("In-plane sample move"),
	                                                                   QString("In-place sample move")),
				                                 AMListAction3::Parallel);

	QVector3D startingGlobalVector(horizontalTranslationControl_->value(),
	                               normalTranslationControl_->value(),
	                               0);

	QVector3D startingPrimeVector = globalSystemToPrime(startingGlobalVector);

	if(direction_ == AMMotorGroupObject::HorizontalMotion) {

		startingPrimeVector.setX(setpoint);
	} else if(direction_ == AMMotorGroupObject::NormalMotion ) {

		startingPrimeVector.setY(setpoint);
	}
	QVector3D targetGlobalVector = primeSystemToGlobal(startingPrimeVector);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(horizontalTranslationControl_,
	                                                                 targetGlobalVector.x()));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(normalTranslationControl_,
	                                                                 targetGlobalVector.y()));

	qDebug() << QString("Moved global axes to [%1, %2]").arg(targetGlobalVector.x()).arg(targetGlobalVector.y());
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

		updateMinimumAndMaximum();
		updateValue();
	}

}

QVector3D REIXSSampleMotor::globalSystemToPrime(const QVector3D &globalVector) const
{
	qDebug() << "\tIncoming global vector =" << globalVector;
	qDebug() << "\tRotated vector =" << rotationQuaternion_.rotatedVector(globalVector);
	return rotationQuaternion_.rotatedVector(globalVector);
}

QVector3D REIXSSampleMotor::primeSystemToGlobal(const QVector3D &primeVector) const
{
	qDebug() << "\tIncoming prime vector =" << primeVector;
	qDebug() << "\tRotated vector=" << rotationQuaternion_.conjugate().rotatedVector(primeVector);
	return rotationQuaternion_.conjugate().rotatedVector(primeVector);
}

void REIXSSampleMotor::updateMinimumAndMaximum()
{
	if(isConnected()) {

		double globalHorizontalMax = 30;
		//double globalHorizontalMin = horizontalTranslationControl_->minimumValue();

		double globalNormalMax = 30;
		//double globalNormalMin = normalTranslationControl_->minimumValue();

		double rotation = totalRotation();

		double angleToCorner = qAbs(radiansToDegrees(atan(globalNormalMax / globalHorizontalMax)));

		if(rotation <= angleToCorner) {

			double primeXMax = globalHorizontalMax / cos(degreesToRadians(rotation));
			double primeXMin = 0;

			QVector3D currentGlobalPosition(horizontalTranslationControl_->value(),
			                                normalTranslationControl_->value(),
			                                0);

			QVector3D currentPrimePosition = globalSystemToPrime(currentGlobalPosition);

			if(currentPrimePosition.x() <= globalNormalMax * sin(degreesToRadians(rotation))) {

				double primeYMax = currentPrimePosition.x() / tan(degreesToRadians(rotation));
				double primeYMin = - (currentPrimePosition.x() * tan(degreesToRadians(rotation)));
				qDebug() << "\tCase I 1: \nY Min" << primeYMin << "Max:" << primeYMax << "\nX Min:" << primeXMin << "Max:" << primeXMax;

			} else if ( globalNormalMax * sin(degreesToRadians(rotation)) <= currentPrimePosition.x() &&
			            globalHorizontalMax * cos(degreesToRadians(rotation)) > currentPrimePosition.x()) {

				double primeYMax = (globalNormalMax / cos(degreesToRadians(rotation))) - currentPrimePosition.x() * tan(degreesToRadians(rotation));
				double primeYMin = - currentPrimePosition.x() * tan(degreesToRadians(rotation));
				qDebug() << "\tCase I 2: \nY Min" << primeYMin << "Max:" << primeYMax << "\nX Min:" << primeXMin << "Max:" << primeXMax;
			} else {

				double primeYMax = (globalNormalMax / cos(degreesToRadians(rotation)) - currentPrimePosition.x() * tan(degreesToRadians(rotation)));
				double primeYMin = -((globalHorizontalMax / cos(degreesToRadians(rotation)) - currentPrimePosition.x()) / tan(degreesToRadians(rotation)));
				qDebug() << "\tCase I 3: \nY Min" << primeYMin << "Max:" << primeYMax << "\nX Min:" << primeXMin << "Max:" << primeXMax;
			}

		} else {

			double primeXMax = globalNormalMax / sin(degreesToRadians(rotation));
			double primeXMin = 0;

			QVector3D currentGlobalPosition(horizontalTranslationControl_->value(),
			                                normalTranslationControl_->value(),
			                                0);

			QVector3D currentPrimePosition = globalSystemToPrime(currentGlobalPosition);

			if(currentPrimePosition.x() <= globalHorizontalMax * cos(degreesToRadians(rotation))) {

				// I
				double primeYMax = currentPrimePosition.x() / tan(degreesToRadians(rotation));
				double primeYMin = - (currentPrimePosition.x() * tan(degreesToRadians(rotation)));
				qDebug() << "\tCase II 1: \nY Min" << primeYMin << "Max:" << primeYMax << "\nX Min:" << primeXMin << "Max:" << primeXMax;

			} else if(globalHorizontalMax * cos(degreesToRadians(rotation) < currentPrimePosition.x() &&
			                                    globalNormalMax * sin(degreesToRadians(rotation)) > currentPrimePosition.x())) {

				// II
				double primeYMin = - (globalHorizontalMax / sin(degreesToRadians(rotation)) - currentPrimePosition.x() / tan(degreesToRadians(rotation)));
				double primeYMax = currentPrimePosition.x() / tan(degreesToRadians(rotation));
				qDebug() << "\tCase II 2: \nY Min" << primeYMin << "Max:" << primeYMax << "\nX Min:" << primeXMin << "Max:" << primeXMax;

			} else {

				// III
				double primeYMin = - (globalHorizontalMax / sin(degreesToRadians(rotation) - currentPrimePosition.x() / tan(degreesToRadians(rotation))));
				double primeYMax = (globalNormalMax / sin(degreesToRadians(rotation)) - currentPrimePosition.x()) / tan(degreesToRadians(rotation));
				qDebug() << "\tCase II 3: \nY Min" << primeYMin << "Max:" << primeYMax << "\nX Min:" << primeXMin << "Max:" << primeXMax;
			}

		}


	}
}

double REIXSSampleMotor::boundsForQuardrant(double firstBound,
                                            double secondBound,
                                            double rotationAngle)
{
	/// Angle from the axis rotated from, to the point(firstBound, secondBound)
	double angleToCorner = qAbs(radiansToDegrees(atan(secondBound / firstBound)));


	if(rotationAngle <= angleToCorner) {

		return firstBound / cos(degreesToRadians(rotationAngle));
	} else {

		return secondBound / cos(degreesToRadians(90 - rotationAngle));
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

double REIXSSampleMotor::radiansToDegrees(double radians)
{
	return radians * 57.295779513082;
}

double REIXSSampleMotor::degreesToRadians(double degrees)
{
	return degrees * 0.017453292519;
}



