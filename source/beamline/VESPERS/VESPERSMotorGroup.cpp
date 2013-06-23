#include "VESPERSMotorGroup.h"

// VESPERSMotorGroupInfo
////////////////////////////////////////
VESPERSMotorGroupInfo::VESPERSMotorGroupInfo(const QString &name, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	firstControl_ = control;
	orientation_ = orientation;
	motionType_ = motionType;
}

VESPERSMotorGroupInfo::VESPERSMotorGroupInfo(const QString &name, AMControl *firstControl, AMControl *secondControl, MotionType motionType, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	firstControl_ = firstControl;
	secondControl_ = secondControl;
	orientation_ = None;
	motionType_ = motionType;
}

// VESPERSMotorGroup
//////////////////////////////////////////////////

VESPERSMotorGroup::VESPERSMotorGroup(QObject *parent)
	: QObject(parent)
{

}

VESPERSMotorGroupInfo *VESPERSMotorGroup::motorGroupInfo(const QString &name) const
{
	return infoMap_.value(name, 0);
}

QString VESPERSMotorGroup::name(VESPERSMotorGroupInfo *motorInfo) const
{
	return infoMap_.key(motorInfo, QString());
}

void VESPERSMotorGroup::addMotorGroupInfo(const QString &name, VESPERSMotorGroupInfo *info)
{
	infoMap_.insert(name, info);
}
