#ifndef BIOXASMIRRORHEIGHTCONTROL_H
#define BIOXASMIRRORHEIGHTCONTROL_H

#include "beamline/BioXAS/BioXASMirrorPseudoMotorControl.h"

class BioXASMirrorHeightControl : public BioXASMirrorPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMirrorHeightControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMirrorHeightControl();

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Calculates and returns the upstream inboard z position (mm) for the given mirror height (mm) and motor positions.
	static double calculateUpstreamInboardZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the upstream outboard z position (mm) for the given mirror height (mm) and motor positions.
	static double calculateUpstreamOutboardZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the downstream z position (in mm) for the given mirror height (mm) and motor positions.
	static double calculateDownstreamZ(double pitch, double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY);
	/// Calculates and returns the mirror height from the given motor positions.
	static double calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
};

#endif // BIOXASMIRRORHEIGHTCONTROL_H
