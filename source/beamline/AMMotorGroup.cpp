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

#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlStopAction.h"
#include "actions3/AMListAction3.h"

// AMMotorGroupObject
////////////////////////////////////////

 AMMotorGroupObject::~AMMotorGroupObject(){}
AMMotorGroupObject::AMMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	prefixes_ << prefix;
	units_ << units;
	controls_ << control;
	orientations_ << orientation;
	motionTypes_ << motionType;
}

AMMotorGroupObject::AMMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	prefixes_ = prefixes;
	units_ = units;
	controls_ = controls;
	orientations_ = orientations;
	motionTypes_ = motionTypes;
}

int AMMotorGroupObject::horizontalIndex() const
{
	int index = -1;

	for (int i = 0, size = orientations_.size(); i < size; i++)
		if (orientations_.at(i) == Horizontal)
			index = i;

	return index;
}

int AMMotorGroupObject::verticalIndex() const
{
	int index = -1;

	for (int i = 0, size = orientations_.size(); i < size; i++)
		if (orientations_.at(i) == Vertical)
			index = i;

	return index;
}

int AMMotorGroupObject::normalIndex() const
{
	int index = -1;

	for (int i = 0, size = orientations_.size(); i < size; i++)
		if (orientations_.at(i) == Normal)
			index = i;

	return index;
}

QString AMMotorGroupObject::horizontalPrefix() const
{
	int index = horizontalIndex();

	if (index == -1)
		return "";

	return prefixes_.at(index);
}

QString AMMotorGroupObject::verticalPrefix() const
{
	int index = verticalIndex();

	if (index == -1)
		return "";

	return prefixes_.at(index);
}

QString AMMotorGroupObject::normalPrefix() const
{
	int index = normalIndex();

	if (index == -1)
		return "";

	return prefixes_.at(index);
}

QString AMMotorGroupObject::horizontalUnits() const
{
	int index = horizontalIndex();

	if (index == -1)
		return "";

	return units_.at(index);
}

QString AMMotorGroupObject::verticalUnits() const
{
	int index = verticalIndex();

	if (index == -1)
		return "";

	return units_.at(index);
}

QString AMMotorGroupObject::normalUnits() const
{
	int index = normalIndex();

	if (index == -1)
		return "";

	return units_.at(index);
}

AMControl *AMMotorGroupObject::horizontalControl() const
{
	int index = horizontalIndex();

	if (index == -1)
		return 0;

	return controls_.at(index);
}

AMControl *AMMotorGroupObject::verticalControl() const
{
	int index = verticalIndex();

	if (index == -1)
		return 0;

	return controls_.at(index);
}

AMControl *AMMotorGroupObject::normalControl() const
{
	int index = normalIndex();

	if (index == -1)
		return 0;

	return controls_.at(index);
}

AMMotorGroupObject::MotionType AMMotorGroupObject::horizontalMotionType() const
{
	int index = horizontalIndex();

	if (index == -1)
		return None;

	return motionTypes_.at(index);
}

AMMotorGroupObject::MotionType AMMotorGroupObject::verticalMotionType() const
{
	int index = verticalIndex();

	if (index == -1)
		return None;

	return motionTypes_.at(index);
}

AMMotorGroupObject::MotionType AMMotorGroupObject::normalMotionType() const
{
	int index = normalIndex();

	if (index == -1)
		return None;

	return motionTypes_.at(index);
}

AMAction3 *AMMotorGroupObject::createHorizontalMoveAction(double position)
{
	if (!horizontalControl()->isConnected())
		return 0;

	AMControlInfo setpoint = horizontalControl()->toInfo();
	setpoint.setValue(position);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, horizontalControl());

	return action;
}

AMAction3 *AMMotorGroupObject::createHorizontalStopAction()
{
	if (!horizontalControl()->isConnected())
		return 0;

	AMControlInfo info = horizontalControl()->toInfo();
	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
	AMAction3 *action = new AMControlStopAction(actionInfo, horizontalControl());

	return action;
}

AMAction3 *AMMotorGroupObject::createVerticalMoveAction(double position)
{
	if (!verticalControl()->isConnected())
		return 0;

	AMControlInfo setpoint = verticalControl()->toInfo();
	setpoint.setValue(position);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, verticalControl());

	return action;
}

AMAction3 *AMMotorGroupObject::createVerticalStopAction()
{
	if (!verticalControl()->isConnected())
		return 0;

	AMControlInfo info = verticalControl()->toInfo();
	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
	AMAction3 *action = new AMControlStopAction(actionInfo, verticalControl());

	return action;
}

AMAction3 *AMMotorGroupObject::createNormalMoveAction(double position)
{
	if (!normalControl()->isConnected())
		return 0;

	AMControlInfo setpoint = normalControl()->toInfo();
	setpoint.setValue(position);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, normalControl());

	return action;
}

AMAction3 *AMMotorGroupObject::createNormalStopAction()
{
	if (!normalControl()->isConnected())
		return 0;

	AMControlInfo info = normalControl()->toInfo();
	AMControlStopActionInfo *actionInfo = new AMControlStopActionInfo(info);
	AMAction3 *action = new AMControlStopAction(actionInfo, normalControl());

	return action;
}

AMAction3 *AMMotorGroupObject::createStopAllAction()
{
	if (!(horizontalControl()->isConnected() && verticalControl()->isConnected() && normalControl()->isConnected()))
		return 0;

	AMListAction3 *list = new AMListAction3(new AMListActionInfo3("Stop all motors.", "Stop all motors in the motor group."), AMListAction3::Parallel);
	list->addSubAction(createHorizontalStopAction());
	list->addSubAction(createVerticalStopAction());
	list->addSubAction(createNormalStopAction());

	return list;
}

#include "ui/AMMotorGroupView.h"
AMMotorGroupObjectView* AMMotorGroupObject::createMotorGroupObjectView(){
	return new AMMotorGroupObjectView(this);
}

// AMMotorGroup
//////////////////////////////////////////////////

AMMotorGroup::AMMotorGroup(QObject *parent)
	: QObject(parent)
{

}

AMMotorGroupObject *AMMotorGroup::motorGroupObject(const QString &name) const
{
	return infoMap_.value(name, 0);
}

QString AMMotorGroup::name(AMMotorGroupObject *motorObject) const
{
	return infoMap_.key(motorObject, QString());
}

void AMMotorGroup::addMotorGroupObject(const QString &name, AMMotorGroupObject *object)
{
	infoMap_.insert(name, object);
}
