#include "VESPERSMotorGroup.h"

// VESPERSMotorGroupInfo
////////////////////////////////////////
VESPERSMotorGroupInfo::VESPERSMotorGroupInfo(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	firstPrefix_ = prefix;
	secondPrefix_ = "";
	units_ = units;
	firstControl_ = control;
	secondControl_ = 0;
	orientation_ = orientation;
	motionType_ = motionType;
}

VESPERSMotorGroupInfo::VESPERSMotorGroupInfo(const QString &name, const QString &firstPrefix, const QString &secondPrefix, const QString &units, AMControl *firstControl, AMControl *secondControl, MotionType motionType, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	firstPrefix_ = firstPrefix;
	secondPrefix_ = secondPrefix;
	units_ = units;
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
