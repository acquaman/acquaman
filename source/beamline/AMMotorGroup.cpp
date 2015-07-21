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
AMMotorGroupMotion::AMMotorGroupMotion(const QString& name,
									   AMControl* motor)
	: QObject(0)
{
	name_ = name;
	motor_ = motor;
}

QString AMMotorGroupMotion::name() const
{
	return name_;
}

QString AMMotorGroupMotion::positionUnits() const
{
	if(motor_) {
		return motor_->units();
	}
}

AMControl * AMMotorGroupMotion::motor() const
{
	return motor_;
}


////////////////////////////////////////////////////////////////////////////////
// Motor Group Axis
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupAxis::AMMotorGroupAxis(AMMotorGroupMotion* translationalMotion,
								   AMMotorGroupMotion* rotationalMotion)
	: QObject(0)
{

	translationalMotion_ = translationalMotion;
	translationalMotion_->setParent(this);
	rotationalMotion_ = rotationalMotion;
	rotationalMotion_->setParent(this);

	if(translationalMotion_ && translationalMotion_->motor()) {
		connect(translationalMotion_->motor(), SIGNAL(connected(bool)),
				this, SIGNAL(translationConnectedStateChanged(bool)));

		connect(translationalMotion_->motor(), SIGNAL(displayPrecisionChanged(int)),
				this, SIGNAL(translationDisplayPrecisionChanged(int)));

		connect(translationalMotion_->motor(), SIGNAL(error(QString)),
				this, SIGNAL(translationError(QString)));

		connect(translationalMotion_->motor(), SIGNAL(moveFailed(int)),
				this, SIGNAL(translationMoveFailed(int)));

		connect(translationalMotion_->motor(), SIGNAL(movingChanged(bool)),
				this, SIGNAL(translationMovingStateChanged(bool)));

		connect(translationalMotion_->motor(), SIGNAL(unitsChanged(QString)),
				this, SIGNAL(translationUnitsChanged(QString)));
	}

	if(rotationalMotion_ && rotationalMotion_->motor()) {

		connect(rotationalMotion_->motor(), SIGNAL(connected(bool)),
				this, SIGNAL(rotationConnectedStateChanged(bool)));

		connect(rotationalMotion_->motor(), SIGNAL(displayPrecisionChanged(int)),
				this, SIGNAL(rotationDisplayPrecisionChanged(int)));

		connect(rotationalMotion_->motor(), SIGNAL(error(QString)),
				this, SIGNAL(rotationError(QString)));

		connect(rotationalMotion_->motor(), SIGNAL(moveFailed(int)),
				this, SIGNAL(rotationMoveFailed(int)));

		connect(rotationalMotion_->motor(), SIGNAL(movingChanged(bool)),
				this, SIGNAL(rotationMovingStateChanged(bool)));

		connect(rotationalMotion_->motor(), SIGNAL(unitsChanged(QString)),
				this, SIGNAL(translationUnitsChanged(QString)));
	}
}

QString AMMotorGroupAxis::translationName() const
{
	if(!canTranslate()) {
		return QString();
	}

	return translationalMotion_->name();
}

QString AMMotorGroupAxis::rotationName() const
{
	if(!canRotate()) {
		return QString();
	}

	return rotationalMotion_->name();
}

double AMMotorGroupAxis::currentTranslationPosition() const
{
	if(!canTranslate()) {
		return 0;
	}

	return translationalMotion_->motor()->value();
}

double AMMotorGroupAxis::currentRotationPosition() const
{
	if(!canRotate()) {
		return 0;
	}

	return rotationalMotion_->motor()->value();
}

QString AMMotorGroupAxis::translationPositionUnits() const
{
	if(!canTranslate()) {
		return QString();
	}

	return translationalMotion_->positionUnits();
}

QString AMMotorGroupAxis::rotationPositionUnits() const
{
	if(!canRotate()) {
		return QString();
	}

	return rotationalMotion_->positionUnits();
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

AMAction3 * AMMotorGroupAxis::createTranslateMoveAction(double position)
{
	if( !canTranslate() ) {
		return 0;
	}

	return AMActionSupport::buildControlMoveAction(translationalMotion_->motor(), position);
}

AMAction3 * AMMotorGroupAxis::createRotateMoveAction(double position)
{
	if( !canRotate() ) {
		return 0;
	}

	return AMActionSupport::buildControlMoveAction(rotationalMotion_->motor(), position);
}

AMAction3 * AMMotorGroupAxis::createStopTranslateAction()
{
	if( !canTranslate() ) {
		return 0;
	}

	AMControlInfo translationMotorInfo = translationalMotion_->motor()->toInfo();
	AMControlStopActionInfo* stopActionInfo = new AMControlStopActionInfo(translationMotorInfo);
	return new AMControlStopAction(stopActionInfo, translationalMotion_->motor());
}

AMAction3 * AMMotorGroupAxis::createStopRotateAction()
{
	if( !canRotate() ) {
		return 0;
	}

	AMControlInfo rotationMotorInfo = rotationalMotion_->motor()->toInfo();
	AMControlStopActionInfo* stopActionInfo = new AMControlStopActionInfo(rotationMotorInfo);
	return new AMControlStopAction(stopActionInfo, rotationalMotion_->motor());

}

////////////////////////////////////////////////////////////////////////////////
// Motor Group Object
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupObject::AMMotorGroupObject(const QString &name)
{
	name_ = name;
}

AMMotorGroupObject::AMMotorGroupObject(const QString& name, const QMap<MotionDirection, AMMotorGroupAxis*>& axes)
{
	name_ = name;
	axes_ = axes;

	// Take ownership of axes by setting outselves as their QObject parent.
	QList<MotionDirection> containedDesignations = axes.keys();
	foreach(MotionDirection currentDesignation, containedDesignations) {

		axes.value(currentDesignation)->setParent(this);
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

		// Current exist exists, but is different. Need to free the current axis'
		// resources before we replace it.
		currentAxis->deleteLater();
		currentAxis->setParent(0);
	}

	// Take ownership of the new axis before we add it to the map.
	axisDetails->setParent(this);

	axes_.insert(motionDirection, axisDetails);
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

////////////////////////////////////////////////////////////////////////////////
// Motor Group Object
////////////////////////////////////////////////////////////////////////////////

AMMotorGroup::AMMotorGroup(QObject *parent)
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

void AMMotorGroup::addMotorGroupObject(AMMotorGroupObject *object)
{
	if(object == 0) {
		return;
	}

	AMMotorGroupObject* currentObject = motorGroupObject(object->name());

	if(currentObject == object) {

		// The object we have with this name is the same as the one we've just
		// been passed. Do nothing.
		return;
	} else if ( currentObject != 0 ) {

		// We already had a different entry for this name, free its resources
		currentObject->setParent(0);
		currentObject->deleteLater();
	}

	// Take ownership of the passed object and add it to our map.
	object->setParent(this);
	groupObjects_.insert(object->name(), object);
}

//// AMMotorGroupObject
//////////////////////////////////////////

// AMMotorGroupObject::~AMMotorGroupObject(){}
//AMMotorGroupObject::AMMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent)
//	: QObject(parent)
//{
//	name_ = name;
//	prefixes_ << prefix;
//	units_ << units;
//	controls_ << control;
//	orientations_ << orientation;
//	motionTypes_ << motionType;
//}

//AMMotorGroupObject::AMMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, QObject *parent)
//	: QObject(parent)
//{
//	name_ = name;
//	prefixes_ = prefixes;
//	units_ = units;
//	controls_ = controls;
//	orientations_ = orientations;
//	motionTypes_ = motionTypes;
//}

//int AMMotorGroupObject::horizontalIndex() const
//{
//	int index = -1;

//	for (int i = 0, size = orientations_.size(); i < size; i++)
//		if (orientations_.at(i) == Horizontal)
//			index = i;

//	return index;
//}

//int AMMotorGroupObject::verticalIndex() const
//{
//	int index = -1;

//	for (int i = 0, size = orientations_.size(); i < size; i++)
//		if (orientations_.at(i) == Vertical)
//			index = i;

//	return index;
//}

//int AMMotorGroupObject::normalIndex() const
//{
//	int index = -1;

//	for (int i = 0, size = orientations_.size(); i < size; i++)
//		if (orientations_.at(i) == Normal)
//			index = i;

//	return index;
//}

//int AMMotorGroupObject::otherIndex() const
//{
//	int index = -1;

//	for (int i = 0, size = orientations_.size(); i < size; i++)
//		if (orientations_.at(i) == Other)
//			index = i;

//	return index;
//}

//QString AMMotorGroupObject::horizontalPrefix() const
//{
//	int index = horizontalIndex();

//	if (index == -1)
//		return "";

//	return prefixes_.at(index);
//}

//QString AMMotorGroupObject::verticalPrefix() const
//{
//	int index = verticalIndex();

//	if (index == -1)
//		return "";

//	return prefixes_.at(index);
//}

//QString AMMotorGroupObject::normalPrefix() const
//{
//	int index = normalIndex();

//	if (index == -1)
//		return "";

//	return prefixes_.at(index);
//}

//QString AMMotorGroupObject::horizontalUnits() const
//{
//	int index = horizontalIndex();

//	if (index == -1)
//		return "";

//	return units_.at(index);
//}

//QString AMMotorGroupObject::verticalUnits() const
//{
//	int index = verticalIndex();

//	if (index == -1)
//		return "";

//	return units_.at(index);
//}

//QString AMMotorGroupObject::normalUnits() const
//{
//	int index = normalIndex();

//	if (index == -1)
//		return "";

//	return units_.at(index);
//}

//AMControl *AMMotorGroupObject::horizontalControl() const
//{
//	int index = horizontalIndex();

//	if (index == -1)
//		return 0;

//	return controls_.at(index);
//}

//AMControl *AMMotorGroupObject::verticalControl() const
//{
//	int index = verticalIndex();

//	if (index == -1)
//		return 0;

//	return controls_.at(index);
//}

//AMControl *AMMotorGroupObject::normalControl() const
//{
//	int index = normalIndex();

//	if (index == -1)
//		return 0;

//	return controls_.at(index);
//}

//AMMotorGroupObject::MotionType AMMotorGroupObject::horizontalMotionType() const
//{
//	int index = horizontalIndex();

//	if (index == -1)
//		return None;

//	return motionTypes_.at(index);
//}

//AMMotorGroupObject::MotionType AMMotorGroupObject::verticalMotionType() const
//{
//	int index = verticalIndex();

//	if (index == -1)
//		return None;

//	return motionTypes_.at(index);
//}

//AMMotorGroupObject::MotionType AMMotorGroupObject::normalMotionType() const
//{
//	int index = normalIndex();

//	if (index == -1)
//		return None;

//	return motionTypes_.at(index);
//}

//bool AMMotorGroupObject::isMotorMoving() const
//{
//	bool isMoving = false;

//	foreach (AMControl *control, controls())
//		isMoving |= control->isMoving();

//	return isMoving;
//}

//AMAction3 *AMMotorGroupObject::createHorizontalMoveAction(double position)
//{
//	if (!horizontalControl()->isConnected())
//		return 0;

//	return AMActionSupport::buildControlMoveAction(horizontalControl(), position);
//}

//AMAction3 *AMMotorGroupObject::createHorizontalStopAction()
//{
//	if (!horizontalControl()->isConnected())
//		return 0;

//	AMControlInfo info = horizontalControl()->toInfo();
//	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
//	AMAction3 *action = new AMControlStopAction(actionInfo, horizontalControl());

//	return action;
//}

//AMAction3 *AMMotorGroupObject::createVerticalMoveAction(double position)
//{
//	if (!verticalControl()->isConnected())
//		return 0;

//	return AMActionSupport::buildControlMoveAction(verticalControl(), position);
//}

//AMAction3 *AMMotorGroupObject::createVerticalStopAction()
//{
//	if (!verticalControl()->isConnected())
//		return 0;

//	AMControlInfo info = verticalControl()->toInfo();
//	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
//	AMAction3 *action = new AMControlStopAction(actionInfo, verticalControl());

//	return action;
//}

//AMAction3 *AMMotorGroupObject::createNormalMoveAction(double position)
//{
//	if (!normalControl()->isConnected())
//		return 0;

//	return AMActionSupport::buildControlMoveAction(normalControl(), position);
//}

//AMAction3 *AMMotorGroupObject::createNormalStopAction()
//{
//	if (!normalControl()->isConnected())
//		return 0;

//	AMControlInfo info = normalControl()->toInfo();
//	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
//	AMAction3 *action = new AMControlStopAction(actionInfo, normalControl());

//	return action;
//}

//AMAction3 *AMMotorGroupObject::createStopAllAction()
//{
//	if (!(horizontalControl()->isConnected() && verticalControl()->isConnected() && normalControl()->isConnected()))
//		return 0;

//	AMListAction3 *list = new AMListAction3(new AMListActionInfo3("Stop all motors.", "Stop all motors in the motor group."), AMListAction3::Parallel);
//	list->addSubAction(createHorizontalStopAction());
//	list->addSubAction(createVerticalStopAction());
//	list->addSubAction(createNormalStopAction());

//	return list;
//}

//#include "ui/AMMotorGroupView.h"
//AMMotorGroupObjectView* AMMotorGroupObject::createMotorGroupObjectView(){
//	return new AMMotorGroupObjectView(this);
//}

//// AMMotorGroup
////////////////////////////////////////////////////

//AMMotorGroup::AMMotorGroup(QObject *parent)
//	: QObject(parent)
//{

//}

//AMMotorGroupObject *AMMotorGroup::motorGroupObject(const QString &name) const
//{
//	return infoMap_.value(name, 0);
//}

//QString AMMotorGroup::name(AMMotorGroupObject *motorObject) const
//{
//	return infoMap_.key(motorObject, QString());
//}

//void AMMotorGroup::addMotorGroupObject(const QString &name, AMMotorGroupObject *object)
//{
//	infoMap_.insert(name, object);
//}
