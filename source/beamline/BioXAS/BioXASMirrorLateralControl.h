#ifndef BIOXASMIRRORLATERALCONTROL_H
#define BIOXASMIRRORLATERALCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class BioXASMirrorLateralControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorLateralControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorLateralControl();

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

	/// Returns the upstream mirror length.
	double upstreamLength() const { return upstreamLength_; }
	/// Returns the downstream mirror length.
	double downstreamLength() const { return downstreamLength_; }

	/// Returns the stripe selection motor control.
	AMControl* stripeSelectionControl() const { return stripeSelect_; }
	/// Returns the yaw motor control.
	AMControl* yawControl() const { return yaw_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const { Q_UNUSED(value) return true; }
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const { Q_UNUSED(value) return true; }

signals:
	/// Notifier that the upstream mirror length has changed.
	void upstreamLengthChanged(double newValue);
	/// Notifier that the downstream mirror length has changed.
	void downstreamLengthChanged(double newValue);
	/// Notifier that the stripe selection motor control has changed.
	void stripeSelectionControlChanged(AMControl *newControl);
	/// Notifier that the yaw motor control has changed.
	void yawControlChanged(AMControl *newControl);

public slots:
	/// Sets the upstream mirror length.
	void setUpstreamLength(double newLength);
	/// Sets the downstream mirror length.
	void setDownstreamLength(double newLength);
	/// Sets the stripe selection motor control.
	void setStripeSelectionControl(AMControl *newControl);
	/// Sets the yaw motor control.
	void setYawControl(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Calculates and returns the lateral motor position for the given lateral displacement, mirror lengths, and yaw motor position.
	double calculateLateralPosition(double lateralDisplacement, double upstreamLength, double downstreamLength, double yawPosition);
	/// Calculates and returns the yaw motor position for the given lateral displacement, mirror lengths, and lateral motor position.
	double calculateYawPosition(double lateralDisplacement, double upstreamLength, double downstreamLength, double lateralPosition);
	/// Calculates and returns the lateral displacement according to the given mirror lengths, lateral motor and yaw motor positions.
	double calculateLateralDisplacement(double upstreamLength, double downstreamLength, double lateralPosition, double yawPosition);

protected:
	/// The upstream mirror length.
	double upstreamLength_;
	/// The downstream mirror length.
	double downstreamLength_;
	/// The stripe selection (lateral) motor control.
	AMControl *stripeSelect_;
	/// The yaw motor control.
	AMControl *yaw_;

};

#endif // BIOXASMIRRORLATERALCONTROL_H
