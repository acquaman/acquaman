#ifndef SGMSAMPLEMANIPULATORMOTORGROUP_H
#define SGMSAMPLEMANIPULATORMOTORGROUP_H

#include "beamline/AMMotorGroup.h"

class SGMSampleManipulatorMotorGroupObject : public AMMotorGroupObject
{
Q_OBJECT
public:
	/// Constructor.  Builds a new motor group object with a single control.
	SGMSampleManipulatorMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent = 0);
	/// Constructor.  Builds a new motor group object with up to three controls.
	SGMSampleManipulatorMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, QObject *parent = 0);

	virtual AMMotorGroupObjectView* createMotorGroupObjectView();
};

#endif // SGMSAMPLEMANIPULATORMOTORGROUP_H
