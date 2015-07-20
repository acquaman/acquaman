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

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const = 0;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const = 0;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const = 0;

	/// Returns true if the current mirror lengths are valid, false otherwise.
	bool validLengths() const;

	/// Returns the upstream mirror length.
	double upstreamLength() const { return upstreamLength_; }
	/// Returns the downstream mirror length.
	double downstreamLength() const { return downstreamLength_; }
	/// Returns the upstream inboard motor control.
	AMControl* upstreamInboardMotor() const { return upstreamInboard_; }
	/// Returns the upstream outboard motor control.
	AMControl* upstreamOutboardMotor() const { return upstreamOutboard_; }
	/// Returns the downstream motor control.
	AMControl* downstreamMotor() const { return downstream_; }
	/// Returns the stripe selection (lateral) motor control.
	AMControl* stripeSelectionMotor() const { return stripeSelect_; }
	/// Returns the yaw motor control.
	AMControl* yawMotor() const { return yaw_; }
	/// Returns the upstream bender motor control.
	AMControl* upstreamBenderMotor() const { return upstreamBender_; }
	/// Returns the downstream bender motor control.
	AMControl* downstreamBenderMotor() const { return downstreamBender_; }

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
	/// Notifier that the stripe selection motor control has changed.
	void stripeSelectionMotorChanged(AMControl *newControl);
	/// Notifier that the yaw motor control has changed.
	void yawMotorChanged(AMControl *newControl);
	/// Notifier that the upstream bender motor control has changed.
	void upstreamBenderMotorChanged(AMControl *newControl);
	/// Notifier that the downstream bender motor control has changed.
	void downstreamBenderMotorChanged(AMControl *newControl);

public slots:
	/// Sets the upstream mirror length.
	void setUpstreamLength(double newValue);
	/// Sets the downstream mirror length.
	void setDownstreamLength(double newValue);
	/// Sets the upstream inboard motor control.
	void setUpstreamInboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the upstream outboard motor control.
	void setUpstreamOutboardMotor(BioXASMirrorMotor *newControl);
	/// Sets the downstream motor control.
	void setDownstreamMotor(BioXASMirrorMotor *newControl);
	/// Sets the stripe selection motor control.
	void setStripeSelectionMotor(AMControl *newControl);
	/// Sets the yaw motor control.
	void setYawMotor(AMControl *newControl);
	/// Sets the upstream bender control.
	void setUpstreamBenderMotor(AMControl *newControl);
	/// Sets the downstream bender control.
	void setDownstreamBenderMotor(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected() = 0;
	/// Updates the current value.
	virtual void updateValue() = 0;
	/// Updates the moving state.
	virtual void updateMoving() = 0;

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Calculates and returns the mirror pitch from the given motor positions.
	static double calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the mirror roll from the given motor positions.
	static double calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the mirror height from the given motor positions.
	static double calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the lateral displacement according to the given mirror lengths, lateral motor position and yaw motor position.
	static double calculateLateral(double upstreamLength, double downstreamLength, double lateralPosition, double yawPosition);
	/// Calculates and returns the yaw for the given yaw motor position and mirror lengths.
	static double calculateYaw(double upstreamLength, double downstreamLength, double yawPosition);

	/// Calculates and returns the upstream inboard motor position from the given pitch, roll, and height.
	static double calculateUpstreamInboardPosition(double upstreamInboardX, double upstreamInboardY, double pitch, double roll, double height);
	/// Calculates and returns the upstream outboard motor position from the given pitch, roll, and height.
	static double calculateUpstreamOutboardPosition(double upstreamOutboardX, double upstreamOutboardY, double pitch, double roll, double height);
	/// Calculates and returns the downstream motor position from the given pitch, roll, and height.
	static double calculateDownstreamPosition(double downstreamX, double downstreamY, double pitch, double roll, double height);
	/// Calculates and returns the lateral motor position for the given lateral displacement, mirror lengths, and yaw control value.
	static double calculateLateralPosition(double lateral, double upstreamLength, double downstreamLength, double yaw);
	/// Calculates and returns the yaw motor position for the given yaw control value and mirror lengths.
	static double calculateYawPosition(double yaw, double upstreamLength, double downstreamLength);
	/// Calculates and returns the yaw motor position for the given lateral control value, lateral motor position, and mirror lengths.
	static double calculateYawPositionFromLateral(double lateral, double upstreamLength, double downstreamLength, double lateralPosition);

protected:
	/// The upstream mirror length.
	double upstreamLength_;
	/// The downstream mirror length.
	double downstreamLength_;
	/// The vertical upstream inboard motor control.
	BioXASMirrorMotor *upstreamInboard_;
	/// The vertical upstream outboard motor control.
	BioXASMirrorMotor *upstreamOutboard_;
	/// The vertical downstream motor control.
	BioXASMirrorMotor *downstream_;
	/// The stripe selection (lateral) motor control.
	AMControl *stripeSelect_;
	/// The yaw motor control.
	AMControl *yaw_;
	/// The upstream bender control.
	AMControl *upstreamBender_;
	/// The downstream bender control.
	AMControl *downstreamBender_;
};

#endif // BIOXASMIRRORCONTROL_H
