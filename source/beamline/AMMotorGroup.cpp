#include "AMMotorGroup.h"

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
