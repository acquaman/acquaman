#include "REIXSSampleMotor.h"

#include "beamline/CLS/CLSMDriveMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

REIXSSampleMotor::REIXSSampleMotor(const QString& name,
                                   const QString& units,
		                           AMControl* horizontalTranslationControl,
		                           AMControl* normalTranslationControl,
		                           AMControl* verticalRotationControl,
		                           QObject *parent,
		                           const QString& description) :

    AMPseudoMotorControl(name, units, parent, description)
{
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

		// qAbs as the sign of the return value of the % operator is implementation dependent.
		return qAbs(combinedRotation % 360);
	}

	return 0;
}

void REIXSSampleMotor::updateConnected()
{
	setConnected(horizontalTranslationControl_ && horizontalTranslationControl_->isConnected()
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

		setValue(primeVector.x());
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
	AMAction3* moveAction = 0;

	QVector3D primeVector(setpoint, 0, 0);

	QVector3D globalVector = primeSystemToGlobal(primeVector);

	AMListAction3* coordinatedMoveAction = new AMListAction3(new AMListActionInfo3(QString("In-plane sample move"),
	                                                                               QString("In-place sample move")),
	                                                         AMListAction3::Parallel);

	coordinatedMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(horizontalTranslationControl_,
	                                                                            globalVector.x()));

	coordinatedMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(normalTranslationControl_,
	                                                                            globalVector.y()));

	return coordinatedMoveAction;

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

		double rotation = totalRotation();

		// Because the rotation might end up with us having mins > maxs we'll refer
		// to them in generic terms here, and then check the larger of the two later.

		double firstQuadrantBounds = boundsForQuardrant(xMax, yMax, rotation % 90);
		double secondQuadrantBounds = boundsForQuardrant(yMax, xMin, rotation % 90);
		double thirdQuadrantBounds = boundsForQuardrant(xMin, yMin, rotation % 90);
		double fourthQuadrantBounds = boundsForQuardrant(yMin, xMax, rotation % 90);

		if((rotation >= 0 && rotation < 90) || (rotation >= 180 && rotation < 270)) {

			// first and third quadrants contain the X bounds
			setMaximumValue(qMax(firstQuadrantBounds, thirdQuadrantBounds));
			setMinimumValue(qMin(firstQuadrantBounds, thirdQuadrantBounds));


		} else {

			// second and fourth quadrants contain the X bounds
			setMaximumValue(qMax(secondQuadrantBounds, fourthQuadrantBounds));
			setMinimumValue(qMin(secondQuadrantBounds, fourthQuadrantBounds));
		}

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


