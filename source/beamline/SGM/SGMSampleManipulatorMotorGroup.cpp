#include "SGMSampleManipulatorMotorGroup.h"

SGMSampleManipulatorMotorGroupObject::SGMSampleManipulatorMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent) :
	AMMotorGroupObject(name, prefix, units, control, orientation, motionType, parent)
{

}

SGMSampleManipulatorMotorGroupObject::SGMSampleManipulatorMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, QObject *parent) :
	AMMotorGroupObject(name, prefixes, units, controls, orientations, motionTypes, parent)
{

}

#include "ui/SGM/SGMSampleManipulatorMotorGroupView.h"
AMMotorGroupObjectView* SGMSampleManipulatorMotorGroupObject::createMotorGroupObjectView(){
	//return new SGMSampleManipulatorMotorGroupObjectView(this);
	SGMSampleManipulatorMotorGroupObjectView *retVal = new SGMSampleManipulatorMotorGroupObjectView(this);
	retVal->setControlSetpointPrecision(2);
	return retVal;
}
