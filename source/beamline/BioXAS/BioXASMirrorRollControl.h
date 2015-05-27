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
	/// Creates and returns a move action iteration.
	virtual AMAction3* createMoveActionIteration(double setpoint);

	/// Calculates and returns the upstream inboard z position (mm) for the given roll (deg) and motor positions.
	static double calculateUpstreamInboardZ(double roll, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the upstream outboard z position (mm) for the given roll (deg) and motor positions.
	static double calculateUpstreamOutboardZ(double roll, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the downstream z position (in mm) for the given roll (deg) and motor positions.
	static double calculateDownstreamZ(double roll, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY);
	/// Calculates and returns the roll from the given motor positions.
	static double calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
};

#endif // BIOXASMIRRORROLLCONTROL_H
