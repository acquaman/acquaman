/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMMotorGroup.h"

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlStopAction.h"
#include "actions3/AMListAction3.h"

////////////////////////////////////////////////////////////////////////////////
// Motor Group Motion
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupMotion::AMMotorGroupMotion(const QString& name, AMControl* motor, QObject* parent)
	: QObject(parent)
{
	name_ = name;
	motor_ = motor;

	if (motor_->isUnitsInitialized())
		setPositionUnits(motor->units());
}

QString AMMotorGroupMotion::name() const
{
	return name_;
}

QString AMMotorGroupMotion::positionUnits() const
{
	if(!motor_) {
		return QString();
	}

	if(!positionUnits_.isEmpty()) {
		return positionUnits_;
	}

	return motor_->units();
}

void AMMotorGroupMotion::setPositionUnits(const QString &positionUnits)
{
	positionUnits_ = positionUnits;
}

AMControl * AMMotorGroupMotion::motor() const
{
	return motor_;
}


////////////////////////////////////////////////////////////////////////////////
// Motor Group Axis
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupAxis::AMMotorGroupAxis(AMMotorGroupMotion* translationalMotion,
								   AMMotorGroupMotion* rotationalMotion,
								   QObject* parent)
	: QObject(parent)
{

	translationalMotion_ = translationalMotion;
	rotationalMotion_ = rotationalMotion;

	if(translationalMotion_) {
		translationalMotion_->setParent(this);
	}

	if(translationalMotion_ && translationalMotion_->motor()) {

		connect(translationalMotion_->motor(), SIGNAL(connected(bool)),
				this, SLOT(onTranslationConnectedStateChanged(bool)));

		connect(translationalMotion_->motor(), SIGNAL(error(int)),
				this, SLOT(onTranslationMotorError(int)));

		connect(translationalMotion_->motor(), SIGNAL(movingChanged(bool)),
				this, SLOT(onTranslationMovingStateChanged(bool)));

		connect(translationalMotion_->motor(), SIGNAL(unitsChanged(QString)),
				this, SLOT(onTranslationPositionUnitsChanged(QString)));

		connect(translationalMotion_->motor(), SIGNAL(valueChanged(double)),
				this, SLOT(onTranslationPositionValueChanged(double)));
	}

	if(rotationalMotion_) {
		rotationalMotion_->setParent(this);
	}

	if(rotationalMotion_ && rotationalMotion_->motor()) {

		connect(rotationalMotion_->motor(), SIGNAL(connected(bool)),
				this, SLOT(onRotationConnectedStateChanged(bool)));

		connect(rotationalMotion_->motor(), SIGNAL(error(int)),
				this, SLOT(onRotationMotorError(int)));

		connect(rotationalMotion_->motor(), SIGNAL(movingChanged(bool)),
				this, SLOT(onRotationMovingStateChanged(bool)));

		connect(rotationalMotion_->motor(), SIGNAL(unitsChanged(QString)),
				this, SLOT(onRotationPositionUnitsChanged(QString)));

		connect(rotationalMotion_->motor(), SIGNAL(valueChanged(double)),
				this, SLOT(onRotationPositionValueChanged(double)));

	}
}

QString AMMotorGroupAxis::translationName() const
{

	if(translationalMotion_) {
		return translationalMotion_->name();
	} else {
		return QString();
	}
}

QString AMMotorGroupAxis::rotationName() const
{
	if(rotationalMotion_) {
		return rotationalMotion_->name();
	} else {
		return QString();
	}
}

double AMMotorGroupAxis::currentTranslationPosition() const
{
	if(translationalMotion_ && translationalMotion_->motor()) {
		return translationalMotion_->motor()->value();
	} else {
		return 0;
	}
}

double AMMotorGroupAxis::currentRotationPosition() const
{
	if(rotationalMotion_ && rotationalMotion_->motor()) {
		return rotationalMotion_->motor()->value();
	} else {
		return 0;
	}
}

QString AMMotorGroupAxis::translationPositionUnits() const
{
	if(translationalMotion_ && translationalMotion_->motor()) {
		return translationalMotion_->positionUnits();
	} else {
		return QString();
	}
}

QString AMMotorGroupAxis::rotationPositionUnits() const
{
	if(rotationalMotion_ && rotationalMotion_->motor()) {
		return rotationalMotion_->positionUnits();
	} else {
		return QString();
	}
}

double AMMotorGroupAxis::minRotationPosition() const
{
	if(rotationalMotion_ && rotationalMotion_->motor()) {
		return rotationalMotion_->motor()->minimumValue();
	} else {
		return 0;
	}
}

double AMMotorGroupAxis::minTranslationPosition() const
{
	if(translationalMotion_ && translationalMotion_->motor()) {
		return translationalMotion_->motor()->minimumValue();
	} else {
		return 0;
	}
}

double AMMotorGroupAxis::maxRotationPosition() const
{
	if(rotationalMotion_ && rotationalMotion_->motor()) {
		return rotationalMotion_->motor()->maximumValue();
	} else {
		return 0;
	}
}

double AMMotorGroupAxis::maxTranslationPosition() const
{
	if(translationalMotion_ && translationalMotion_->motor()) {
		return translationalMotion_->motor()->maximumValue();
	} else {
		return 0;
	}
}

bool AMMotorGroupAxis::isMoving() const
{
	return ( isTranslating() || isRotating() );
}

bool AMMotorGroupAxis::isTranslating() const
{
	if( !canTranslate() ) {
		return false;
	}

	return translationalMotion_->motor()->isMoving();
}

bool AMMotorGroupAxis::isRotating() const
{
	if( !canRotate() ) {
		return false;
	}

	return rotationalMotion_->motor()->isMoving();
}

bool AMMotorGroupAxis::canTranslate() const
{
	return translationalMotion_ &&
			translationalMotion_->motor() &&
			translationalMotion_->motor()->isConnected();
}

bool AMMotorGroupAxis::canRotate() const
{
	return rotationalMotion_ &&
			rotationalMotion_->motor() &&
			rotationalMotion_->motor()->isConnected();
}

AMControl * AMMotorGroupAxis::translationMotor() const
{
	if(translationalMotion_) {
		return translationalMotion_->motor();
	} else {
		return 0;
	}
}

AMControl * AMMotorGroupAxis::rotationMotor() const
{
	if(rotationalMotion_) {
		return rotationalMotion_->motor();
	} else {
		return 0;
	}
}

AMAction3 * AMMotorGroupAxis::createTranslateMoveAction(double position)
{
	if( !canTranslate() ) {
		return 0;
	}

	return AMActionSupport::buildControlMoveAction(translationalMotion_->motor(),
												   position);
}

AMAction3 * AMMotorGroupAxis::createRotateMoveAction(double position)
{
	if( !canRotate() ) {
		return 0;
	}

	return AMActionSupport::buildControlMoveAction(rotationalMotion_->motor(),
												   position);
}

AMAction3 * AMMotorGroupAxis::createStopTranslateAction()
{
	if( !canTranslate() ) {
		return 0;
	}

	AMControlInfo translationMotorInfo = translationalMotion_->motor()->toInfo();

	AMControlStopActionInfo* stopActionInfo =
			new AMControlStopActionInfo(translationMotorInfo);

	return new AMControlStopAction(stopActionInfo, translationalMotion_->motor());
}

AMAction3 * AMMotorGroupAxis::createStopRotateAction()
{
	if( !canRotate() ) {
		return 0;
	}

	AMControlInfo rotationMotorInfo = rotationalMotion_->motor()->toInfo();
	AMControlStopActionInfo* stopActionInfo =
			new AMControlStopActionInfo(rotationMotorInfo);

	return new AMControlStopAction(stopActionInfo, rotationalMotion_->motor());

}

void AMMotorGroupAxis::stopTranslation()
{
	if(canTranslate()) {
		translationalMotion_->motor()->stop();
	}
}

void AMMotorGroupAxis::stopRotation()
{
	if(canRotate()) {
		rotationalMotion_->motor()->stop();
	}
}

void AMMotorGroupAxis::setTranslatePosition(double position)
{
	if(canTranslate()) {
		translationalMotion_->motor()->move(position);
	}
}

void AMMotorGroupAxis::setRotatePosition(double position)
{
	if(canRotate()) {
		rotationalMotion_->motor()->move(position);
	}
}

void AMMotorGroupAxis::setTranslationPositionUnits(const QString &positionUnits)
{
	if(translationalMotion_) {
		if(translationPositionUnits() != positionUnits) {
			translationalMotion_->setPositionUnits(positionUnits);
			emit positionUnitsChanged(AMMotorGroupAxis::TranslationalMotion, positionUnits);
		}
	}
}

void AMMotorGroupAxis::setRotationPositionUnits(const QString &positionUnits)
{
	if(rotationalMotion_) {
		if(rotationPositionUnits() != positionUnits) {
			rotationalMotion_->setPositionUnits(positionUnits);
			emit positionUnitsChanged(AMMotorGroupAxis::RotationalMotion, positionUnits);
		}
	}
}

void AMMotorGroupAxis::onTranslationConnectedStateChanged(bool isConnected)
{
	emit connectedStateChanged(AMMotorGroupAxis::TranslationalMotion, isConnected);
}

void AMMotorGroupAxis::onTranslationMotorError(const int errorCode)
{
	emit motorError(AMMotorGroupAxis::TranslationalMotion, errorCode);
}

void AMMotorGroupAxis::onTranslationMovingStateChanged(bool isMoving)
{
	emit movingStateChanged(AMMotorGroupAxis::TranslationalMotion, isMoving);
}

void AMMotorGroupAxis::onTranslationPositionUnitsChanged(const QString &positionUnits)
{
	// The position units is the one incoming from the motor control. If this
	// is different from the one for the translational position units, then the
	// position units has been manually set in this group, and we don't need to
	// emit the signal
	if(translationalMotion_->positionUnits() == positionUnits) {
		emit positionUnitsChanged(AMMotorGroupAxis::TranslationalMotion, positionUnits);
	}
}

void AMMotorGroupAxis::onTranslationPositionValueChanged(double positionValue)
{
	emit positionValueChanged(AMMotorGroupAxis::TranslationalMotion, positionValue);
}

void AMMotorGroupAxis::onRotationConnectedStateChanged(bool isConnected)
{
	emit connectedStateChanged(AMMotorGroupAxis::RotationalMotion, isConnected);
}

void AMMotorGroupAxis::onRotationMotorError(int errorCode)
{
	emit motorError(AMMotorGroupAxis::RotationalMotion, errorCode);
}

void AMMotorGroupAxis::onRotationMovingStateChanged(bool isMoving)
{
	emit movingStateChanged(AMMotorGroupAxis::RotationalMotion, isMoving);
}

void AMMotorGroupAxis::onRotationPositionUnitsChanged(const QString &positionUnits)
{
	// The position units is the one incoming from the motor control. If this
	// is different from the one for the rotational position units, then the
	// position units has been manually set in this group, and we don't need to
	// emit the signal
	if(rotationalMotion_->positionUnits() == positionUnits) {
		emit positionUnitsChanged(AMMotorGroupAxis::TranslationalMotion, positionUnits);
	}
}

void AMMotorGroupAxis::onRotationPositionValueChanged(double positionValue)
{
	emit positionValueChanged(AMMotorGroupAxis::RotationalMotion, positionValue);
}

////////////////////////////////////////////////////////////////////////////////
// Motor Group Object
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupObject::AMMotorGroupObject(const QString &name, QObject* parent)
	: QObject(parent)
{
	name_ = name;
}

AMMotorGroupObject::AMMotorGroupObject(const QString& name, const QMap<MotionDirection, AMMotorGroupAxis*>& axes)
{
	name_ = name;
	axes_ = axes;

	// Take ownership of axes by setting outselves as their QObject parent.
	QList<MotionDirection> containedDirections = axes.keys();
	foreach(MotionDirection currentDirection, containedDirections) {

		AMMotorGroupAxis* currentAxis = axes.value(currentDirection);
		currentAxis->setParent(this);
		connectAxisToSlots(currentDirection, currentAxis);
	}
}

void AMMotorGroupObject::setDirectionAxis(AMMotorGroupObject::MotionDirection motionDirection,
										  AMMotorGroupAxis *axisDetails)
{

	// Do we already have an entry for this motion direction?
	AMMotorGroupAxis* currentAxis = axes_.value(motionDirection, 0);

	if(currentAxis == axisDetails) {

		// Current Axis is the same as the one we've just been passed. Nothing
		// is required.
		return;

	} else if (currentAxis != 0) {

		// Current axis exists, but is different. Need to free the current axis'
		// resources before we replace it.
		currentAxis->deleteLater();
		currentAxis->setParent(0);
	}

	if(axisDetails) {
		// Take ownership of the new axis before we add it to the map.
		axisDetails->setParent(this);

		// Set up the connections
		connectAxisToSlots(motionDirection, axisDetails);

		axes_.insert(motionDirection, axisDetails);
	} else {
		axes_.remove(motionDirection);
	}

}

void AMMotorGroupObject::setDirectionAxis(MotionDirection motionDirection,
										  const QString& translationalName,
										  AMControl* translationalMotor,
										  const QString& rotationalName,
										  AMControl* rotationalMotor)
{
	AMMotorGroupMotion* translation = 0;
	if(translationalMotor) {
		translation = new AMMotorGroupMotion(translationalName,
											 translationalMotor);
	}

	AMMotorGroupMotion* rotation = 0;
	if(rotationalMotor) {
			rotation = new AMMotorGroupMotion(rotationalName,
											  rotationalMotor);
	}

	setDirectionAxis(motionDirection, new AMMotorGroupAxis(translation, rotation));
}

int AMMotorGroupObject::axisCount() const
{
	return axes_.count();
}

int AMMotorGroupObject::dimensionality() const
{
	QList<MotionDirection> containedDesignations = axes_.keys();
	int dimensionCount = 0;

	foreach(MotionDirection currentAxisDesignation, containedDesignations) {

		AMMotorGroupAxis* currentAxis = axes_.value(currentAxisDesignation);
		if(currentAxis->canRotate()) {
			++dimensionCount;
		}

		if(currentAxis->canTranslate()) {
			++dimensionCount;
		}
	}

	return dimensionCount;
}

QString AMMotorGroupObject::name() const
{
	return name_;
}

AMMotorGroupAxis * AMMotorGroupObject::axis(AMMotorGroupObject::MotionDirection motionDirection) const
{
	return axes_.value(motionDirection, 0);
}

AMMotorGroupAxis * AMMotorGroupObject::horizontalAxis() const
{
	return axis(HorizontalMotion);
}

AMMotorGroupAxis * AMMotorGroupObject::normalAxis() const
{
	return axis(NormalMotion);
}

AMMotorGroupAxis * AMMotorGroupObject::verticalAxis() const
{
	return axis(VerticalMotion);
}

bool AMMotorGroupObject::hasAxis(AMMotorGroupObject::MotionDirection motionDirection) const
{
	return axes_.contains(motionDirection);
}

bool AMMotorGroupObject::hasHorizontalAxis() const
{
	return hasAxis(HorizontalMotion);
}

bool AMMotorGroupObject::hasNormalAxis() const
{
	return hasAxis(NormalMotion);
}

bool AMMotorGroupObject::hasVerticalAxis() const
{
	return hasAxis(VerticalMotion);
}

bool AMMotorGroupObject::isMoving() const
{
	bool isMoving = false;

	if(hasHorizontalAxis()) {
		isMoving |= horizontalAxis()->isMoving();
	}

	if(hasNormalAxis()) {
		isMoving |= normalAxis()->isMoving();
	}

	if(hasVerticalAxis()) {
		isMoving |= horizontalAxis()->isMoving();
	}

	return isMoving;
}

AMAction3 * AMMotorGroupObject::createStopAllAction()
{
	AMListActionInfo3* stopAllActionInfo = new AMListActionInfo3(
				"Stop all Motors",
				QString("Stop motors of %1").arg(name_));

	AMListAction3* stopAllAction = new AMListAction3(
				stopAllActionInfo,
				AMListAction3::Parallel);

	QList<MotionDirection> axisDesignations = axes_.keys();

	foreach(MotionDirection currentDesignation, axisDesignations) {

		if(hasAxis(currentDesignation)) {

			AMMotorGroupAxis* currentAxis = axis(currentDesignation);

			if(currentAxis->canTranslate()) {
				stopAllAction->addSubAction(currentAxis->createStopTranslateAction());
			}

			if(currentAxis->canRotate()) {
				stopAllAction->addSubAction(currentAxis->createStopRotateAction());
			}
		}

	}

	return stopAllAction;
}

void AMMotorGroupObject::stopAll()
{
	if(hasVerticalAxis()) {
		verticalAxis()->stopRotation();
		verticalAxis()->stopTranslation();
	}

	if(hasHorizontalAxis()) {
		horizontalAxis()->stopRotation();
		horizontalAxis()->stopTranslation();
	}

	if(hasNormalAxis()) {
		normalAxis()->stopRotation();
		normalAxis()->stopTranslation();
	}
}

void AMMotorGroupObject::onHorizontalConnectedStateChanged(AMMotorGroupAxis::MotionType motionType,
														   bool isConnected)
{
	emit connectedStateChanged(AMMotorGroupObject::HorizontalMotion,
							   motionType,
							   isConnected);
}

void AMMotorGroupObject::onHorizontalMotorError(AMMotorGroupAxis::MotionType motionType,
												int errorCode)
{
	emit motorError(AMMotorGroupObject::HorizontalMotion,
					 motionType,
					 errorCode);
}

void AMMotorGroupObject::onHorizontalMovingStateChanged(AMMotorGroupAxis::MotionType motionType,
														bool isMoving)
{
	emit movingStateChanged(AMMotorGroupObject::HorizontalMotion,
							motionType,
							isMoving);
}

void AMMotorGroupObject::onHorizontalPositionUnitsChanged(AMMotorGroupAxis::MotionType motionType,
														  const QString &positionUnits)
{
	emit positionUnitsChanged(AMMotorGroupObject::HorizontalMotion,
							  motionType,
							  positionUnits);
}

void AMMotorGroupObject::onHorizontalPositionValueChanged(AMMotorGroupAxis::MotionType motionType,
														  double positionValue)
{
	emit positionValueChanged(AMMotorGroupObject::HorizontalMotion,
							  motionType,
							  positionValue);
}

void AMMotorGroupObject::onVerticalConnectedStateChanged(AMMotorGroupAxis::MotionType motionType,
														 bool isConnected)
{
	emit connectedStateChanged(AMMotorGroupObject::VerticalMotion,
							   motionType,
							   isConnected);
}

void AMMotorGroupObject::onVerticalMotorError(AMMotorGroupAxis::MotionType motionType,
											  int errorCode)
{
	emit motorError(AMMotorGroupObject::VerticalMotion,
					 motionType,
					 errorCode);
}

void AMMotorGroupObject::onVerticalMovingStateChanged(AMMotorGroupAxis::MotionType motionType,
													  bool isMoving)
{
	emit movingStateChanged(AMMotorGroupObject::VerticalMotion,
							motionType,
							isMoving);
}

void AMMotorGroupObject::onVerticalPositionUnitsChanged(AMMotorGroupAxis::MotionType motionType,
														const QString &positionUnits)
{
	emit positionUnitsChanged(AMMotorGroupObject::VerticalMotion,
							  motionType,
							  positionUnits);
}

void AMMotorGroupObject::onVerticalPositionValueChanged(AMMotorGroupAxis::MotionType motionType,
														double positionValue)
{
	emit positionValueChanged(AMMotorGroupObject::VerticalMotion,
							  motionType,
							  positionValue);
}

void AMMotorGroupObject::onNormalConnectedStateChanged(AMMotorGroupAxis::MotionType motionType,
													   bool isConnected)
{
	emit connectedStateChanged(AMMotorGroupObject::NormalMotion,
							   motionType,
							   isConnected);
}

void AMMotorGroupObject::onNormalMotorError(AMMotorGroupAxis::MotionType motionType,
											int errorCode)
{
	emit motorError(AMMotorGroupObject::NormalMotion,
					 motionType,
					 errorCode);
}

void AMMotorGroupObject::onNormalMovingStateChanged(AMMotorGroupAxis::MotionType motionType,
													bool isMoving)
{
	emit movingStateChanged(AMMotorGroupObject::NormalMotion,
							motionType,
							isMoving);
}

void AMMotorGroupObject::onNormalPositionUnitsChanged(AMMotorGroupAxis::MotionType motionType,
													  const QString &positionUnits)
{
	emit positionUnitsChanged(AMMotorGroupObject::NormalMotion,
							  motionType,
							  positionUnits);
}

void AMMotorGroupObject::onNormalPositionValueChanged(AMMotorGroupAxis::MotionType motionType,
													  double positionValue)
{
	emit positionValueChanged(AMMotorGroupObject::NormalMotion,
							  motionType,
							  positionValue);
}

void AMMotorGroupObject::connectAxisToSlots(MotionDirection motionDirection,
											AMMotorGroupAxis* axisDetails)
{

	if (motionDirection == VerticalMotion) {

		connect(axisDetails, SIGNAL(connectedStateChanged(AMMotorGroupAxis::MotionType,bool)),
				this, SLOT(onVerticalConnectedStateChanged(AMMotorGroupAxis::MotionType,bool)));
		connect(axisDetails, SIGNAL(motorError(AMMotorGroupAxis::MotionType,int)),
				this, SLOT(onVerticalMotorError(AMMotorGroupAxis::MotionType,int)));
		connect(axisDetails, SIGNAL(movingStateChanged(AMMotorGroupAxis::MotionType,bool)),
				this, SLOT(onVerticalMovingStateChanged(AMMotorGroupAxis::MotionType,bool)));
		connect(axisDetails, SIGNAL(positionUnitsChanged(AMMotorGroupAxis::MotionType,QString)),
				this, SLOT(onVerticalPositionUnitsChanged(AMMotorGroupAxis::MotionType,QString)));
		connect(axisDetails, SIGNAL(positionValueChanged(AMMotorGroupAxis::MotionType,double)),
				this, SLOT(onVerticalPositionValueChanged(AMMotorGroupAxis::MotionType,double)));

	} else if (motionDirection == HorizontalMotion) {

		connect(axisDetails, SIGNAL(connectedStateChanged(AMMotorGroupAxis::MotionType,bool)),
				this, SLOT(onHorizontalConnectedStateChanged(AMMotorGroupAxis::MotionType,bool)));
		connect(axisDetails, SIGNAL(motorError(AMMotorGroupAxis::MotionType,int)),
				this, SLOT(onHorizontalMotorError(AMMotorGroupAxis::MotionType,int)));
		connect(axisDetails, SIGNAL(movingStateChanged(AMMotorGroupAxis::MotionType,bool)),
				this, SLOT(onHorizontalMovingStateChanged(AMMotorGroupAxis::MotionType,bool)));
		connect(axisDetails, SIGNAL(positionUnitsChanged(AMMotorGroupAxis::MotionType,QString)),
				this, SLOT(onHorizontalPositionUnitsChanged(AMMotorGroupAxis::MotionType,QString)));
		connect(axisDetails, SIGNAL(positionValueChanged(AMMotorGroupAxis::MotionType,double)),
				this, SLOT(onHorizontalPositionValueChanged(AMMotorGroupAxis::MotionType,double)));

	} else {

		connect(axisDetails, SIGNAL(connectedStateChanged(AMMotorGroupAxis::MotionType,bool)),
				this, SLOT(onNormalConnectedStateChanged(AMMotorGroupAxis::MotionType,bool)));
		connect(axisDetails, SIGNAL(motorError(AMMotorGroupAxis::MotionType,int)),
				this, SLOT(onNormalMotorError(AMMotorGroupAxis::MotionType,int)));
		connect(axisDetails, SIGNAL(movingStateChanged(AMMotorGroupAxis::MotionType,bool)),
				this, SLOT(onNormalMovingStateChanged(AMMotorGroupAxis::MotionType,bool)));
		connect(axisDetails, SIGNAL(positionUnitsChanged(AMMotorGroupAxis::MotionType,QString)),
				this, SLOT(onNormalPositionUnitsChanged(AMMotorGroupAxis::MotionType,QString)));
		connect(axisDetails, SIGNAL(positionValueChanged(AMMotorGroupAxis::MotionType,double)),
				this, SLOT(onNormalPositionValueChanged(AMMotorGroupAxis::MotionType,double)));

	}
}

////////////////////////////////////////////////////////////////////////////////
// Motor Group
////////////////////////////////////////////////////////////////////////////////

AMMotorGroup::AMMotorGroup(QObject *parent)
	: QObject(parent)
{

}

QList<QString> AMMotorGroup::names() const
{
	return groupObjects_.keys();
}

QList<AMMotorGroupObject *> AMMotorGroup::motorGroupObjects() const
{
	return groupObjects_.values();
}

int AMMotorGroup::size() const
{
	return groupObjects_.size();
}

AMMotorGroupObject * AMMotorGroup::motorGroupObject(const QString &name) const
{
	return groupObjects_.value(name, 0);
}

QString AMMotorGroup::name(AMMotorGroupObject *motorObject) const
{
	return groupObjects_.key(motorObject, QString());
}

AMMotorGroupObject* AMMotorGroup::addMotorGroupObject(AMMotorGroupObject *object)
{
	if(object == 0) {
		return 0;
	}

	AMMotorGroupObject* replacedObject = motorGroupObject(object->name());

	if(replacedObject == object) {

		// The object we have with this name is the same as the one we've just
		// been passed. Returns 0 to indicate nothing has been replaced.
		return 0;
	}

	// Add the object to the map and return the replaced object (which might be
	// 0 if nothing was replaced).
	groupObjects_.insert(object->name(), object);
	return replacedObject;
}









