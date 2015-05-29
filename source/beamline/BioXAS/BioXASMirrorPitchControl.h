#ifndef BIOXASMIRRORPITCHCONTROL_H
#define BIOXASMIRRORPITCHCONTROL_H

#include "beamline/BioXAS/BioXASMirrorPseudoMotorControl.h"

class BioXASMirrorPitchControl : public BioXASMirrorMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorPitchControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorPitchControl();

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASMIRRORPITCHCONTROL_H
