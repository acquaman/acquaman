#ifndef BIOXASMIRRORROLLCONTROL_H
#define BIOXASMIRRORROLLCONTROL_H

#include "beamline/BioXAS/BioXASMirrorPseudoMotorControl.h"

class BioXASMirrorRollControl : public BioXASMirrorMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorRollControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorRollControl();

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASMIRRORROLLCONTROL_H
