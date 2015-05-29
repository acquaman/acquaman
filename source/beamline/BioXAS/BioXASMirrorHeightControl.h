#ifndef BIOXASMIRRORHEIGHTCONTROL_H
#define BIOXASMIRRORHEIGHTCONTROL_H

#include "beamline/BioXAS/BioXASMirrorPseudoMotorControl.h"

class BioXASMirrorHeightControl : public BioXASMirrorMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorHeightControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorHeightControl();

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASMIRRORHEIGHTCONTROL_H
