#ifndef CLSPSEUDOMOTORGROUP_H
#define CLSPSEUDOMOTORGROUP_H

#include "beamline/AMMotorGroup.h"

/// This class extends the AMMotorGroupObject for the CLS pseudo motor application.  The main addition is adding the control which resets the pseudo motors since they don't have absolute references.
class CLSPseudoMotorGroupObject : public AMMotorGroupObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds a new motor group object which mimics the AMMotorGroupObject with the addition of a reset control.
	CLSPseudoMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, AMControl *resetControl, QObject *parent = 0);
	/// Constructor.  Builds a new motor group object which mimics the AMMotorGroupObject with up to three controls.
	CLSPseudoMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, AMControl *resetControl, QObject *parent = 0);

	/// Returns the reset control.
	AMControl *resetControl() const { return resetControl_; }

public slots:
	/// Reset this CLSPseudoMotorGroupObject.
	void resetPseudoMotor() const { resetControl_->move(1); }

protected:
	/// The control that handles the reset of the pseudo motor.
	AMControl *resetControl_;
};

#endif // CLSPSEUDOMOTORGROUP_H
