#ifndef BIOXASMIRRORCONTROL_H
#define BIOXASMIRRORCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"

class BioXASMirrorControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorControl();

	/// Returns true if the current mirror lengths are valid, false otherwise.
	bool validLengths() const;

	/// Returns the upstream mirror length.
	double upstreamLength() const { return upstreamLength_; }
	/// Returns the downstream mirror length.
	double downstreamLength() const { return downstreamLength_; }

	/// Returns the upstream inboard motor control.
	AMControl* upstreamInboardMotor() const { return upstreamInboardMotor_; }
	/// Returns the upstream outboard motor control.
	AMControl* upstreamOutboardMotor() const { return upstreamOutboardMotor_; }
	/// Returns the downstream motor control.
	AMControl* downstreamMotor() const { return downstreamMotor_; }
	/// Returns the yaw motor control.
	AMControl* yawMotor() const { return yawMotor_; }
	/// Returns the lateral motor control.
	AMControl* lateralMotor() const { return lateralMotor_; }
	/// Returns the upstream bender motor control.
	AMControl* upstreamBenderMotor() const { return upstreamBenderMotor_; }
	/// Returns the downstream bender motor control.
	AMControl* downstreamBenderMotor() const { return downstreamBenderMotor_; }

signals:
	/// Notifier that the upstream mirror length has changed.
	void upstreamLengthChanged(double newValue);
	/// Notifier that the downstream mirror length has changed.
	void downstreamLengthChanged(double newValue);

	/// Notifier that the vertical upstream inboard motor control has changed.
	void upstreamInboardMotorChanged(AMControl *newControl);
	/// Notifier that the vertical upstream outboard motor control has changed.
	void upstreamOutboardMotorChanged(AMControl *newControl);
	/// Notifier that the vertical downstream motor control has changed.
	void downstreamMotorChanged(AMControl *newControl);
	/// Notifier that the yaw motor control has changed.
	void yawMotorChanged(AMControl *newControl);
	/// Notifier that the lateral motor control has changed.
	void lateralMotorChanged(AMControl *newControl);
	/// Notifier that the upstream bender motor control has changed.
	void upstreamBenderMotorChanged(AMControl *newControl);
	/// Notifier that the downstream bender motor control has changed.
	void downstreamBenderMotorChanged(AMControl *newControl);

public slots:
	/// Sets the upstream mirror length.
	virtual void setUpstreamLength(double newValue);
	/// Sets the downstream mirror length.
	virtual void setDownstreamLength(double newValue);

	/// Sets the upstream inboard motor control.
	virtual void setUpstreamInboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the upstream outboard motor control.
	virtual void setUpstreamOutboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the downstream motor control.
	virtual void setDownstreamMotor(BioXASMirrorMotor *newControl);
	/// Sets the yaw motor control.
	virtual void setYawMotor(AMControl *newControl);
	/// Sets the lateral motor control.
	virtual void setLateralMotor(AMControl *newControl);
	/// Sets the upstream bender control.
	virtual void setUpstreamBenderMotor(AMControl *newControl);
	/// Sets the downstream bender control.
	virtual void setDownstreamBenderMotor(AMControl *newControl);

protected slots:
	/// Sets the connected state. Reimplemented to set the setpoint to be the same as the value on startup.
	virtual void setConnected(bool newState);

protected:
	/// Calculates and returns the mirror pitch from the given motor positions.
	double calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ) const;
	/// Calculates and returns the mirror roll from the given motor positions.
	double calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ) const;
	/// Calculates and returns the mirror height from the given motor positions.
	double calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ) const;
	/// Calculates and returns the lateral displacement according to the given mirror lengths, lateral motor position and yaw motor position.
	double calculateLateral(double upstreamLength, double downstreamLength, double lateralPosition, double yawPosition) const;
	/// Calculates and returns the yaw for the given yaw motor position and mirror lengths.
	double calculateYaw(double upstreamLength, double downstreamLength, double yawPosition) const;

	/// Calculates and returns the upstream inboard motor position from the given pitch, roll, and height.
	double calculateUpstreamInboardPosition(double upstreamInboardX, double upstreamInboardY, double pitch, double roll, double height) const;
	/// Calculates and returns the upstream outboard motor position from the given pitch, roll, and height.
	double calculateUpstreamOutboardPosition(double upstreamOutboardX, double upstreamOutboardY, double pitch, double roll, double height) const;
	/// Calculates and returns the downstream motor position from the given pitch, roll, and height.
	double calculateDownstreamPosition(double downstreamX, double downstreamY, double pitch, double roll, double height) const;
	/// Calculates and returns the yaw motor position for the given yaw control value and mirror lengths.
	double calculateYawPosition(double yaw, double upstreamLength, double downstreamLength) const;
	/// Calculates and returns the yaw motor position for the given lateral control value, lateral motor position, and mirror lengths.
	double calculateYawPositionFromLateral(double lateral, double upstreamLength, double downstreamLength, double lateralPosition) const;
	/// Calculates and returns the lateral motor position for the given lateral displacement, mirror lengths, and yaw control value.
	double calculateLateralPosition(double lateral, double upstreamLength, double downstreamLength, double yaw) const;

protected:
	/// The upstream mirror length.
	double upstreamLength_;
	/// The downstream mirror length.
	double downstreamLength_;

	/// The vertical upstream inboard motor control.
	BioXASMirrorMotor *upstreamInboardMotor_;
	/// The vertical upstream outboard motor control.
	BioXASMirrorMotor *upstreamOutboardMotor_;
	/// The vertical downstream motor control.
	BioXASMirrorMotor *downstreamMotor_;
	/// The stripe selection (lateral) motor control.
	AMControl *lateralMotor_;
	/// The yaw motor control.
	AMControl *yawMotor_;
	/// The upstream bender control.
	AMControl *upstreamBenderMotor_;
	/// The downstream bender control.
	AMControl *downstreamBenderMotor_;
};

#endif // BIOXASMIRRORCONTROL_H
