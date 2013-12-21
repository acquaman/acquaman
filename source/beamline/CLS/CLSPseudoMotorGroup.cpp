#include "CLSPseudoMotorGroup.h"

// CLSPseudoMotorGroupObject
////////////////////////////////////////////////////

 CLSPseudoMotorGroupObject::~CLSPseudoMotorGroupObject(){}
CLSPseudoMotorGroupObject::CLSPseudoMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, AMControl *resetControl, QObject *parent)
	: AMMotorGroupObject(name, prefix, units, control, orientation, motionType, parent)
{
	resetControl_ = resetControl;
}

CLSPseudoMotorGroupObject::CLSPseudoMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, AMControl *resetControl, QObject *parent)
	: AMMotorGroupObject(name, prefixes, units, controls, orientations, motionTypes, parent)
{
	resetControl_ = resetControl;
}

// CLSPseudoMotorGroup
////////////////////////////////////////////////////

CLSPseudoMotorGroup::CLSPseudoMotorGroup(QObject *parent)
	: AMMotorGroup(parent)
{

}
