#include "AMMotorGroup.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineControlStopAction.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "actions/AMBeamlineListAction.h"

// AMMotorGroupObject
////////////////////////////////////////

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

AMBeamlineActionItem *AMMotorGroupObject::createHorizontalMoveAction(double position)
{
	if (!horizontalControl()->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(horizontalControl());
	action->setSetpoint(position);

	return action;
}

AMBeamlineActionItem *AMMotorGroupObject::createHorizontalStopAction()
{
	if (!horizontalControl()->isConnected())
		return 0;

	return new AMBeamlineControlStopAction(horizontalControl());
}

AMBeamlineActionItem *AMMotorGroupObject::createVerticalMoveAction(double position)
{
	if (!verticalControl()->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(verticalControl());
	action->setSetpoint(position);

	return action;
}

AMBeamlineActionItem *AMMotorGroupObject::createVerticalStopAction()
{
	if (!verticalControl()->isConnected())
		return 0;

	return new AMBeamlineControlStopAction(verticalControl());
}

AMBeamlineActionItem *AMMotorGroupObject::createNormalMoveAction(double position)
{
	if (!normalControl()->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(normalControl());
	action->setSetpoint(position);

	return action;

}

AMBeamlineActionItem *AMMotorGroupObject::createNormalStopAction()
{
	if (!normalControl()->isConnected())
		return 0;

	return new AMBeamlineControlStopAction(normalControl());
}

AMBeamlineActionItem *AMMotorGroupObject::createStopAllAction()
{
	if (!(horizontalControl()->isConnected() && verticalControl()->isConnected() && normalControl()->isConnected()))
		return 0;

	AMBeamlineParallelActionsList *stopAllActionsList = new AMBeamlineParallelActionsList;
	AMBeamlineListAction *stopAllAction = new AMBeamlineListAction(stopAllActionsList);

	stopAllActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	stopAllActionsList->appendAction(0, new AMBeamlineControlStopAction(horizontalControl()));
	stopAllActionsList->appendAction(0, new AMBeamlineControlStopAction(verticalControl()));
	stopAllActionsList->appendAction(0, new AMBeamlineControlStopAction(normalControl()));

	return stopAllAction;
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
