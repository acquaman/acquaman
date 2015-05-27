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
	/// Creates and returns a move action iteration.
	virtual AMAction3* createMoveActionIteration(double setpoint);

	/// Calculates and returns the upstream inboard z position (mm) for the given mirror height (mm) and motor positions.
	static double calculateUpstreamInboardZ(double height, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the upstream outboard z position (mm) for the given mirror height (mm) and motor positions.
	static double calculateUpstreamOutboardZ(double height, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the downstream z position (in mm) for the given mirror height (mm) and motor positions.
	static double calculateDownstreamZ(double height, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY);
	/// Calculates and returns the mirror height from the given motor positions.
	static double calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
};

#endif // BIOXASMIRRORHEIGHTCONTROL_H
