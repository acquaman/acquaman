#ifndef BIOXASSIDEM1PITCHCONTROL_H
#define BIOXASSIDEM1PITCHCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASSideM1PitchControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideM1PitchControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideM1PitchControl();

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
	void setUpstreamInboardControl(AMControl *newControl);
	/// Sets the upstream outboard motor control.
	void setUpstreamOutboardControl(AMControl *newControl);
	/// Sets the downstream motor control.
	void setDownstreamControl(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateIsMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Calculates and returns the pitch from the given motor positions.
	static double calculatePitch(double upstreamInboardX, upstreamInboardY, upstreamInboardZ, upstreamOutboardX, upstreamOutboardY, upstreamOutboardZ, downstreamX, downstreamY, downstreamZ);

protected:
	/// The vertical upstream inboard motor control.
	AMControl *upstreamInboard_;
	/// The vertical upstream outboard motor control.
	AMControl *upstreamOutboard_;
	/// The vertical downstream motor control.
	AMControl *downstream_;
};

#endif // BIOXASSIDEM1PITCHCONTROL_H
