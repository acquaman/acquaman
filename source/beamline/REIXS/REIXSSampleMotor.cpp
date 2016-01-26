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

void REIXSSampleMotor::updateConnected()
{
	setConnected(horizontalTranslationControl_ && horizontalTranslationControl_->isConnected()
	             && normalTranslationControl_ && normalTranslationControl_->isConnected()
	             && verticalRotationControl_ && verticalRotationControl_->isConnected());

	if(!connectedOnce_ && isConnected()) {

		connectedOnce_ = true;
		updateMinimumAndMaximum();
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

		double  verticalRotation = verticalRotationControl_->value() + angleOffset_;

		QVector3D vX(1, 0, 0);
		QVector3D vY(0, 1, 0);
		QVector3D vZ(0, 0, 1);

		QQuaternion qX = QQuaternion::fromAxisAndAngle(vX, 0);
		QQuaternion qY = QQuaternion::fromAxisAndAngle(vY, 0);
		QQuaternion qZ = QQuaternion::fromAxisAndAngle(vZ, verticalRotation);

		rotationQuaternion_ = (qX * qY) * qZ;
		rotationQuaternion_.normalize();

		updateMinimumValue();
		updateMaximumValue();
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

		QVector3D maxGlobalVector(horizontalTranslationControl_->maximumValue(),
		                          normalTranslationControl_->maximumValue(),
		                          0);

		QVector3D minGlobalVector(horizontalTranslationControl_->minimumValue(),
		                          normalTranslationControl_->minimumValue(),
		                          0);

		QVector3D maxPrimeVector = globalSystemToPrime(maxGlobalVector);
		QVector3D minPrimeVector = globalSystemToPrime(minGlobalVector);

		setMinimumValue(qMin(maxPrimeVector.x(), minPrimeVector.x()));
		setMaximumValue(qMax(maxPrimeVector.x(), minPrimeVector.x()));
	}
}
