#ifndef BIOXASMIRRORMOTORCONTROL_H
#define BIOXASMIRRORMOTORCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"

class BioXASMirrorMotorControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorMotorControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorMotorControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the upstream inboard motor control.
	AMControl* upstreamInboardControl() const { return upstreamInboard_; }
	/// Returns the upstream outboard motor control.
	AMControl* upstreamOutboardControl() const { return upstreamOutboard_; }
	/// Returns the downstream motor control.
	AMControl* downstreamControl() const { return downstream_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

signals:
	/// Notifier that the vertical upstream inboard motor control has changed.
	void upstreamInboardControlChanged(AMControl *newControl);
	/// Notifier that the vertical upstream outboard motor control has changed.
	void upstreamOutboardControlChanged(AMControl *newControl);
	/// Notifier that the vertical downstream motor control has changed.
	void downstreamControlChanged(AMControl *newControl);

public slots:
	/// Sets the upstream inboard motor control.
	void setUpstreamInboardControl(BioXASMirrorMotor *newControl);
	/// Sets the upstream outboard motor control.
	void setUpstreamOutboardControl(BioXASMirrorMotor *newControl);
	/// Sets the downstream motor control.
	void setDownstreamControl(BioXASMirrorMotor *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue() = 0;
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Calculates and returns the mirror pitch from the given motor positions.
	static double calculatePitch(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the mirror roll from the given motor positions.
	static double calculateRoll(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);
	/// Calculates and returns the mirror height from the given motor positions.
	static double calculateHeight(double upstreamInboardX, double upstreamInboardY, double upstreamInboardZ, double upstreamOutboardX, double upstreamOutboardY, double upstreamOutboardZ, double downstreamX, double downstreamY, double downstreamZ);

	/// Calculates and returns the upstream inboard motor position from the given pitch, roll, and height.
	static double calculateUpstreamInboardZ(double upstreamInboardX, double upstreamInboardY, double pitch, double roll, double height);
	/// Calculates and returns the upstream outboard motor position from the given pitch, roll, and height.
	static double calculateUpstreamOutboardZ(double upstreamOutboardX, double upstreamOutboardY, double pitch, double roll, double height);
	/// Calculates and returns the downstream motor position from the given pitch, roll, and height.
	static double calculateDownstreamZ(double downstreamX, double downstreamY, double pitch, double roll, double height);

protected:
	/// The vertical upstream inboard motor control.
	BioXASMirrorMotor *upstreamInboard_;
	/// The vertical upstream outboard motor control.
	BioXASMirrorMotor *upstreamOutboard_;
	/// The vertical downstream motor control.
	BioXASMirrorMotor *downstream_;
};

#endif // BIOXASMIRRORMOTORCONTROL_H
